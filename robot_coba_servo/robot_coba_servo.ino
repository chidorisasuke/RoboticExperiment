// PROGRAM FIX
#include <Servo.h>
#include <string.h>

Servo servoKanan;
Servo servoKiri;
// Pin motor pada Arduino
int trigPin = 6;
int echoPin = 7;

int in1 = 46;   // Pin kontrol IN1 motor 1
int in2 = 48;   // Pin kontrol IN2 motor 1
int enA = 12;   // Pin Enable A (PWM) untuk motor 1

int in3 = 50;   // Pin kontrol IN3 motor 2
int in4 = 52;   // Pin kontrol IN4 motor 2
int enB = 11;   // Pin Enable B (PWM) untuk motor 2

// Pin sensor garis
int sens_tengah = 4;  // Sensor tengah
int sens_kanan1 = 10; // Sensor kanan ujung
int sens_kanan2 = 5;  // Sensor kanan sebelum tengah
int sens_kiri2 = 3;   // Sensor kiri setelah tengah
int sens_kiri1 = 2;   // Sensor kiri ujung

long durasi;
float jarak;

unsigned long previousMillis = 0;
const long interval = 20;
int servoSpeed = 1;
int servoClosePos = 10;
int servoOpenPos = 180;

int servo1pos = 0;
int servo2pos = 180;

bool nangkap = false;
bool servoClosing = false;
String lastAction = "stop";

void setup() {
  // Set pin motor sebagai output
  servoKanan.attach(8);
  servoKiri.attach(9);
  

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);

  // Set pin sensor garis sebagai input
  pinMode(sens_tengah, INPUT);
  pinMode(sens_kanan1, INPUT);
  pinMode(sens_kanan2, INPUT);
  pinMode(sens_kiri2, INPUT);
  pinMode(sens_kiri1, INPUT);

  // Awal motor dalam kondisi mati
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  Serial.begin(9600);  // Memulai komunikasi serial
}

void loop() {
  bacaSensor();
  cekUltrasonik();
  if (nangkap){
    TutupServoPerlahanNonBlocking();
  }
}

void cekUltrasonik() {
  // Mengirim sinyal trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Mengukur waktu pantulan echo
  durasi = pulseIn(echoPin, HIGH);

  // Menghitung jarak (cm)
  jarak = durasi * 0.034 / 2;

  // Jika jarak objek kurang dari 15 cm, aktifkan servo
  if (jarak < 15 && !nangkap) {
    // bacaSensor();
    // TutupServoPerlahan();  // Menutup servo secara perlahan
    nangkap = true;   // Menandakan servo sedang mencekram
  }
}

void TutupServoPerlahanNonBlocking() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= servoSpeed) {
    previousMillis = currentMillis;

    if (servo1pos <= servoOpenPos) {
      servoKanan.write(servo1pos);
      // servo2pos = 180 - servo1pos;
      servo2pos = constrain(190 - servo1pos, 0 , 180);
      servoKiri.write(servo2pos);
      servo1pos += 20;
    } else {
      nangkap = false;  // Menandakan servo telah selesai menutup
    }
  }
}

void TutupServoPerlahan(){
  // bacaSensor();
  for(servo1pos=10;servo1pos<=200;servo1pos++){
    servoKanan.write(servo1pos);
    servo2pos = 190 - servo1pos;
    servoKiri.write(servo2pos);
    delay(10);
  }
}

void bukaServoPerlahan() {
  for(servo1pos=190;servo1pos>=10;servo1pos--){
    servoKanan.write(servo1pos);
    servo2pos = servo2pos + 1;
    servoKiri.write(servo2pos);
    delay(20);
  }
}

// Fungsi membaca sensor garis
void bacaSensor() {
  // cekUltrasonik();
  // Cek sensor tengah untuk bergerak lurus
  // sens_kanan2 = kanan after tengah
  // sens_kiri2 = kiri after tengah
  // if (digitalRead(sens_tengah) == 1 && digitalRead(sens_kanan2) == 1 && digitalRead(sens_kiri2) == 1){
  //   // checkpoint++;
  //   delay(1000);
  //   nangkap = false;
  //   bukaServoPerlahan();
  //   // moveBackward();
  //   putarBalik();
  //   checkpoint = 0;
  // }
  if (digitalRead(sens_tengah) == 0) {
    gerakMaju();
    Serial.println("Sensor tengah mendeteksi garis, motor bergerak maju");
    lastAction = "maju";
  }
  ///////BELOK KANANN////////
  // Cek sensor kanan untuk belok kanan
  else if (digitalRead(sens_kanan1) == 0) {
    belokKanan();
    Serial.println("Sensor kanan mendeteksi garis, motor serong kanan");
    lastAction = "belokKanan";
  }
  else if (digitalRead(sens_kanan1) == 0 && digitalRead(sens_kanan2) == 0) {
    belokKanan();
    Serial.println("Sensor kanan mendeteksi garis, motor serong kanan");
    lastAction = "belokKanan";
  }
  else if (digitalRead(sens_kanan2) == 0) {
    serongKanan();
    Serial.println("Sensor kanan mendeteksi garis, motor serong kanan");
    lastAction = "serongKanan";
  }
  else if (digitalRead(sens_tengah) == 0 && digitalRead(sens_kanan2) == 0) {
    serongKanan();
    Serial.println("Sensor kanan mendeteksi garis, motor serong kanan");
    lastAction = "serongKanan";
  }
  //////BELOK KIRIII///////
  // Cek sensor kiri untuk belok kiri
  else if (digitalRead(sens_kiri1) == 0) {
    belokKiri();
    Serial.println("Sensor kanan mendeteksi garis, motor belok kanan");
    lastAction = "belokKiri";
  }
  else if (digitalRead(sens_kiri1) == 0 && digitalRead(sens_kiri2) == 0) {
    belokKiri();
    Serial.println("Sensor kiri mendeteksi garis, motor belok kiri");
    lastAction = "belokKiri";
  }
  else if (digitalRead(sens_kiri2) == 0) {
    serongKiri();
    Serial.println("Sensor kanan mendeteksi garis, motor serong kanan");
    lastAction = "serongKiri";
  }
  else if (digitalRead(sens_tengah) == 0 && digitalRead(sens_kiri2) == 0) {
    serongKiri();
    Serial.println("Sensor kiri mendeteksi garis, motor belok kiri");
    lastAction = "serongKiri";
  }
  // Jika tidak ada garis, berhenti
  else if (digitalRead(sens_tengah) == 1 && digitalRead(sens_kiri1) == 1 && digitalRead(sens_kiri2) == 1 && digitalRead(sens_kanan1) == 1 && digitalRead(sens_kanan2) == 1){
    Serial.println("Tidak ada garis, motor berhenti");
    if (lastAction == "maju"){
      gerakMaju();
    } else if (lastAction == "belokKanan") {
      belokKanan();
    } else if (lastAction == "serongKanan") {
      serongKanan();
    } else if (lastAction == "belokKiri") {
      belokKiri();
    } else if (lastAction == "serongKiri") {
      serongKiri();
    }
  }
  else{
    stopMotors();
    Serial.println("Tidak ada garis, motor berhenti");
    lastAction = "stop";  // Update kondisi terakhir
  }
  delay(100);  // Delay untuk mencegah pembacaan terlalu cepat
}

// Fungsi untuk memutar motor maju (lurus)
void gerakMaju() {
  // Motor 1 dan motor 2 maju
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 255);  // Kecepatan maksimal motor 1

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 255);  // Kecepatan maksimal motor 2
}

// Fungsi untuk belok kanan
void belokKanan() {
  // Motor kiri bergerak maju, motor kanan berhenti
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 10);  // Kecepatan maksimal motor kiri

  digitalWrite(in3, LOW);  // Motor kanan berhenti
  digitalWrite(in4, HIGH);
  analogWrite(enB, 200);  // Kecepatan maksimal motor 2
}

// Fungsi untuk belok kanan
void serongKanan() {
  // Motor kiri bergerak maju, motor kanan berhenti
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 40);  // Kecepatan maksimal motor kiri

  digitalWrite(in3, LOW);  // Motor kanan berhenti
  digitalWrite(in4, HIGH);
  analogWrite(enB, 200);  // Kecepatan maksimal motor 2
}

// Fungsi untuk belok kiri
void belokKiri() {
  // Motor kanan bergerak maju, motor kiri berhenti
  digitalWrite(in1, LOW);  // Motor kiri berhenti
  digitalWrite(in2, HIGH);
  analogWrite(enA, 200);  // Kecepatan maksimal motor 2

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 10);  // Kecepatan maksimal motor kanan
}

// Fungsi untuk belok kiri
void serongKiri() {
  // Motor kanan bergerak maju, motor kiri berhenti
  digitalWrite(in1, LOW);  // Motor kiri berhenti
  digitalWrite(in2, HIGH);
  analogWrite(enA, 200);  // Kecepatan maksimal motor 2

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 40);  // Kecepatan maksimal motor kanan
}

// Fungsi untuk menghentikan kedua motor
void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enA, 0);
  analogWrite(enB, 0);
}

void putarBalik() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 255);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 255);
  delay(2000);
  stopMotors();
}

void moveBackward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 255);

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 255);
  delay(2000);
  stopMotors();
}

