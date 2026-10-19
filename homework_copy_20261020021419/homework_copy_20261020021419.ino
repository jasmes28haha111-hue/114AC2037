/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://docs.arduino.cc/hardware/

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/Blink/
*/
const int PIN_R = 3;
const int PIN_G = 5;
const int PIN_B = 6;
const int buttonPin = 2;
const bool BUTTON_ACTIVE_LOW = true;
int brightness = 0;       // 當前亮度 (0 ~ 255)
const int fadeSpeed = 30;

void setRGB(int r, int g, int b) {
  analogWrite(PIN_R, 255 - constrain(r, 0, 255));
  analogWrite(PIN_G, 255 - constrain(g, 0, 255));
  analogWrite(PIN_B, 255 - constrain(b, 0, 255));
}
void getRainbowColor(byte pos, int brightness, int &r, int &g, int &b) {
  float scale = brightness / 255.0;
  if (pos < 85) {
    r = (pos * 3) * scale;
    g = (255 - pos * 3) * scale;
    b = 0;
  } else if (pos < 170) {
    pos -= 85;
    r = (255 - pos * 3) * scale;
    g = 0;
    b = (pos * 3) * scale;
  } else {
    pos -= 170;
    r = 0;
    g = (pos * 3) * scale;
    b = (255 - pos * 3) * scale;
  }
}

// 狀態追蹤變數
int clickCount = 0;              // 記錄短按次數
int currentMode = 0;             // 鎖定當前長按模式 (0: 藍紫, 1: 藍綠, 2: 彩虹)
byte rainbowWheel = 0;           // 彩虹旋轉相位

unsigned long pressStartTime = 0;
unsigned long releaseTime = 0;
bool lastButtonState = false;
bool isHolding = false;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(buttonPin, INPUT);
  setRGB(0, 0, 0);
}

// the loop function runs over and over again forever
void loop() {
  int rawReading = digitalRead(buttonPin);
  bool isPressed = BUTTON_ACTIVE_LOW ? (rawReading == LOW) : (rawReading == HIGH);
  unsigned long currentMillis = millis();

  // 1. 偵測按下瞬間
  if (isPressed && !lastButtonState) {
    pressStartTime = currentMillis;
  }

  // 2. 偵測放開瞬間
  if (!isPressed && lastButtonState) {
    unsigned long pressDuration = currentMillis - pressStartTime;
    // 短按判定 (30ms ~ 350ms)
    if (pressDuration >= 30 && pressDuration < 350) {
      clickCount++;
      releaseTime = currentMillis;
    }
    // 放開按鈕時復位
    isHolding = false;
    brightness = 0;
  }

  // 3. 連擊超時重置：放開超過 500ms 沒按下一發，清空短按計數
  if (clickCount > 0 && !isPressed) {
    if (currentMillis - releaseTime > 500) {
      clickCount = 0;
    }
  }

  // 4. 長按觸發判定 (按住超過 250ms)
  if (isPressed && (currentMillis - pressStartTime > 250)) {
    if (!isHolding) {
      // 剛進入長按瞬間，根據先前的短按次數決定觸發模式
      isHolding = true;
      if (clickCount == 0) {
        currentMode = 0; // 直接長按 -> 藍紫
      } else if (clickCount == 1) {
        currentMode = 1; // 點一下再長按 -> 藍綠
      } else if (clickCount >= 2) {
        currentMode = 2; // 點兩下再長按 -> 彩虹變色
      }
      clickCount = 0; // 進入長按後歸零計數
    }

    // 漸漸變亮
    if (brightness < 255) {
      brightness++;
    }
  }

  // 5. 輸出對應燈光模式
  if (isHolding && brightness > 0) {
    if (currentMode == 0) {
      // 模式 0：藍紫色 (藍光全亮 + 紅光半亮)
      setRGB(brightness / 2, 0, brightness);
    } 
    else if (currentMode == 1) {
      // 模式 1：藍綠色 (藍光全亮 + 綠光約 7 成)
      setRGB(0, brightness * 0.7, brightness);
    } 
    else if (currentMode == 2) {
      // 模式 2：彩虹七彩漸變
      int r = 0, g = 0, b = 0;
      getRainbowColor(rainbowWheel, brightness, r, g, b);
      setRGB(r, g, b);
      rainbowWheel++; // 彩虹顏色持續流動
    }
  } else {
    // 沒按或放開時全部熄滅
    setRGB(0, 0, 0);
  }

  lastButtonState = isPressed;
  delay(12); // 控制漸亮與彩虹流動的速度
}
