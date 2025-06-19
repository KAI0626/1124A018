#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // 若顯示異常，請改 0x3F
const int ledPin = 8;

int time = 30; // 秒，30秒測試
volatile bool running = false;
unsigned long lastBlink = 0;
bool ledState = false;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  showTime(); // 開機自動顯示倒數
}

void loop() {
  static unsigned long lastCount = 0;
  // 偵錯：每次loop都輸出running狀態
  Serial.print("running: ");
  Serial.println(running);
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "START") {
      running = true;
      time = 30;
      ledState = true;
      digitalWrite(ledPin, ledState);
      showTime();
      lastCount = millis();
    } else if (cmd == "PAUSE") {
      running = false;
      digitalWrite(ledPin, LOW);
    } else if (cmd == "RESET") {
      running = false;
      time = 30;
      digitalWrite(ledPin, LOW);
      showTime();
    } else if (cmd == "END") {
      running = false;
      digitalWrite(ledPin, HIGH); // 結束時恆亮
      showEnd();
    }
  }

  // Arduino本地倒數計時
  if (running && time > 0) {
    if (millis() - lastCount >= 1000) {
      lastCount = millis();
      time--;
      showTime();
      if (time == 0) {
        running = false;
        digitalWrite(ledPin, HIGH);
        showEnd();
      }
    }
  }

  // LED 閃爍
  if (running) {
    if (millis() - lastBlink >= 500) {
      lastBlink = millis();
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    }
  }
}

void showTime() {
  int m = time / 60;
  int s = time % 60;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pomodoro");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  if (m < 10) lcd.print('0');
  lcd.print(m);
  lcd.print(':');
  if (s < 10) lcd.print('0');
  lcd.print(s);
}

void showEnd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time's up!");
  lcd.setCursor(0, 1);
  lcd.print("Take a break!");
}
