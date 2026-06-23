#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(32, 16, 2);  // 안 되면 0x27로 바꾸기
Servo doorServo;

const int servoPin = 10;
const int pirPin = 11;
const int redLED = 12;
const int greenLED = 13;
const int buzzer = A1;

String password = "1234";
String input = "";
int failCount = 0;
bool locked = true;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  doorServo.attach(servoPin);
  doorServo.write(0);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("SMART DOOR LOCK");
  lcd.setCursor(0, 1);
  lcd.print("Enter Password");
  
  noTone(buzzer);
  digitalWrite(buzzer, LOW);
}

void loop() {
  checkPIR();

  char key = keypad.getKey();

  if (key) {
    if (key == '*') {
      input = "";
      lcd.clear();
      lcd.print("Input Cleared");
      delay(800);
      showReady();
    }
    else if (key == '#') {
      checkPassword();
    }
    else {
      input += key;
      lcd.clear();
      lcd.print("Password:");
      lcd.setCursor(0, 1);
      for (int i = 0; i < input.length(); i++) {
        lcd.print(key);
      }
    }
  }
}

void checkPassword() {
  if (input == password) {
    failCount = 0;
    locked = false;

    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    noTone(buzzer);

    lcd.clear();
    lcd.print("ACCESS GRANTED");
    lcd.setCursor(0, 1);
    lcd.print("Door Open");

    doorServo.write(90);
    delay(3000);

    doorServo.write(0);
    locked = true;
    digitalWrite(greenLED, LOW);

    input = "";
    showReady();
  }
  else {
    failCount++;
    input = "";

    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);

    lcd.clear();
    lcd.print("ACCESS DENIED");
    lcd.setCursor(0, 1);
    lcd.print("Fail: ");
    lcd.print(failCount);

    tone(buzzer, 1000);
    delay(800);
    noTone(buzzer);
    digitalWrite(redLED, LOW);

    if (failCount >= 3) {
      systemLocked();
    }

    showReady();
  }
}

void systemLocked() {
  lcd.clear();
  lcd.print("SYSTEM LOCKED");
  lcd.setCursor(0, 1);
  lcd.print("ALARM!");

  for (int i = 0; i < 6; i++) {
    digitalWrite(redLED, HIGH);
    tone(buzzer, 1200);
    delay(300);
    digitalWrite(redLED, LOW);
    noTone(buzzer);
    delay(300);
  }

  failCount = 0;
}

void checkPIR() {
  if (locked && digitalRead(pirPin) == HIGH) {
    lcd.clear();
    lcd.print("INTRUDER ALERT");
    lcd.setCursor(0, 1);
    lcd.print("Motion Detected");

    digitalWrite(redLED, HIGH);
    tone(buzzer, 1500);
    delay(1000);
    noTone(buzzer);
    digitalWrite(redLED, LOW);

    showReady();
  }
}

void showReady() {
  lcd.clear();
  lcd.print("SMART DOOR LOCK");
  lcd.setCursor(0, 1);
  lcd.print("Enter Password");
}
