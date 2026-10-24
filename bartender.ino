#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_PWMServoDriver.h>

// === LCD ===
// LCD I2C (alamat biasanya 0x27 atau 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === PCA9685 ===
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40); // PCA9685 default 0x40

// === Pin Tombol ===
const int btnRasa1 = 39;  
const int btnRasa2 = 37; 
const int btnRasa3 = 35;
const int btnStart = 33;

// === Relay ===
const int relay1 = 6; 
const int relay2 = 7;
const int relay3 = 8;

// Servo channel pada PCA9685
#define SERVO_GELAS   0  // servo kanan-kiri (harus halus)
#define SERVO_PUTAR1  1  // servo putar 30 derajat
#define SERVO_PUTAR2  2
#define SERVO_PUTAR3  3

// === Variabel Global ===
int pilihan = 0;

// === Fungsi Servo ===
int angleToPulse(int ang) {
  return map(ang, 0, 180, 102, 512); 
}

void setServo(int ch, int angle) {
  pwm.setPWM(ch, 0, angleToPulse(angle));
}

// Fungsi tambahan untuk pergerakan halus servo
void smoothMove(int ch, int startAngle, int endAngle, int stepDelay = 15) {
  if (startAngle < endAngle) {
    for (int pos = startAngle; pos <= endAngle; pos++) {
      setServo(ch, pos);
      delay(stepDelay);
    }
  } else {
    for (int pos = startAngle; pos >= endAngle; pos--) {
      setServo(ch, pos);
      delay(stepDelay);
    }
  }
}

void setup() {
  // I2C hardware
  Wire.begin();

  // LCD
  lcd.init();
  lcd.backlight();

  // Tombol
  pinMode(btnRasa1, INPUT_PULLUP);
  pinMode(btnRasa2, INPUT_PULLUP);
  pinMode(btnRasa3, INPUT_PULLUP);
  pinMode(btnStart, INPUT_PULLUP);

  // Relay
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
 
  // PCA9685
  pwm.begin();
  pwm.setPWMFreq(50);

  // Posisi awal servo
  setServo(SERVO_GELAS, 90);
  setServo(SERVO_PUTAR1, 65);
  setServo(SERVO_PUTAR2, 10);
  setServo(SERVO_PUTAR3, 0);

  lcd.setCursor(0,0);
  lcd.print("Robot Bartender");
  lcd.setCursor(0,1);
  lcd.print("Pilih Rasa...");
}

void loop() {
  if (digitalRead(btnRasa1) == LOW) {
    pilihan = 1;
    tampilPilihan("Kelapa");
  }
  if (digitalRead(btnRasa2) == LOW) {
    pilihan = 2;
    tampilPilihan("Melon"); 
  }
  if (digitalRead(btnRasa3) == LOW) {
    pilihan = 3;
    tampilPilihan("Jeruk");
  }

  if (digitalRead(btnStart) == LOW && pilihan > 0) {
    mulaiProses();
  }
}

void tampilPilihan(String rasa) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Rasa: " + rasa);
  lcd.setCursor(0,1);
  lcd.print("Tekan Start...");
  delay(300);
}

void mulaiProses() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tunggu Ya...");

  // Servo gelas halus 90 -> 160
  smoothMove(SERVO_GELAS, 90, 144, 10);
  delay(300);
  setServo(SERVO_PUTAR1, 120);
  delay(300);
  setServo(SERVO_PUTAR1, 65);
  delay(300);

  // Servo gelas halus 160 -> 20
  smoothMove(SERVO_GELAS, 144, 30, 10);
  delay(300);
  setServo(SERVO_PUTAR2, 65);
  delay(300);
  setServo(SERVO_PUTAR2, 10);
  delay(300);

  // Servo gelas halus 20 -> 90
  smoothMove(SERVO_GELAS, 30, 90, 10);
  delay(300);
  setServo(SERVO_PUTAR3, 55);
  delay(800);

  if (pilihan == 1) digitalWrite(relay1, LOW);
  else if (pilihan == 2) digitalWrite(relay2, LOW);
  else if (pilihan == 3) digitalWrite(relay3, LOW);

  delay(10000);

  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  delay(2000);

  smoothMove(SERVO_PUTAR3, 55, 0, 10);
  delay(500);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Selesai!");
  lcd.setCursor(0,1);
  lcd.print("Nikmati minuman");

  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Robot Bartender");
  lcd.setCursor(0,1);
  lcd.print("Pilih Rasa...");
  pilihan = 0;
}
