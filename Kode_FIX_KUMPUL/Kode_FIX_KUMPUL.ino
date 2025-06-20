// PROGRAM FIX
#include <Servo.h>
#include <string.h>

Servo servoKanan;
Servo servoKiri;
// Pin motor pada Arduino
int trigPin = 7;
int echoPin = 8;

int in1 = 46;   // Pin kontrol IN1 motor 1
int in2 = 48;   // Pin kontrol IN2 motor 1
int enA = 12;   // Pin Enable A (PWM) untuk motor 1

int in3 = 50;   // Pin kontrol IN3 motor 2
int in4 = 52;   // Pin kontrol IN4 motor 2
int enB = 11;   // Pin Enable B (PWM) untuk motor 2

// Pin sensor garis
int sens_tengah = 26;  // Sensor tengah
int sens_kanan1 = 30; // Sensor kanan ujung
int sens_kanan2 = 28;  // Sensor kanan sebelum tengah
int sens_kiri2 = 24;   // Sensor kiri setelah tengah
int sens_kiri1 = 22;   // Sensor kiri ujung

long durasi;
int jarak;

//For stop
unsigned long lastDetectionTime = 0;
const unsigned long noDetectionTimeout = 2500; //default 700, custom 1 = 1500
//For backward
unsigned long readyToBackward = 0;
const unsigned long timesUpBackward = 5000;

//Menghitung waktu untuk berjalan mundur setelah berhenti
unsigned long timeOpenServo;
unsigned long readyToOpenServo;
const unsigned long minTimeOpenServo = 1000;
const unsigned long maxTimeOpenServo = 2000;
unsigned long interval = 15;

int servo1pos;
int servo2pos;

bool mundurTroli = true;
bool nangkap = false;
bool servoTutup = false;
bool readytoFinish = false;
String lastAction = "stop";
String lastActionMundur = "stop";

void setup() {
  // Set pin motor sebagai output
  servoKanan.attach(9);
  servoKiri.attach(10);
  
  servo1pos = 190;
  servo2pos = 10;

  servoKanan.write(servo1pos);
  servoKiri.write(servo2pos);

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
  while(mundurTroli == true){
    cekUltrasonik();
    
    if (jarak >= 10 && jarak <= 13){
    Serial.print("Masuk ke dalam jarak");
      if (servoTutup == false){
        melambat();  
        bukaServoPerlahan(); //tutup servo dari kondisi buka
        servoTutup = true;
        majuCariGaris();
        mundurTroli = false;
      }
    }
    else if (jarak > 13){
      Serial.print("Mundur hingga menabrak troli");
      mundurNabrak();
    } 
  }
  
  bacaSensor();
}

//Method untuk memperoleh data jarak dari ultrasonik
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
  Serial.print("Jarak: ");
  Serial.print(jarak);
  Serial.println(" cm");
  delay(1);
}

//Method untuk menutup servo kemudian mengait kaki troli
void TutupServoPerlahan(){
  // bacaSensor();
  for(servo1pos=10;servo1pos<=200;servo1pos++){
    servoKanan.write(servo1pos);
    servo2pos = 200 - servo1pos;
    servoKiri.write(servo2pos);
    delay(5);
  }
}

//Method untuk membuka servo dan melepas pengait dari kaki troli
void bukaServoPerlahan() {
  for(servo1pos=190;servo1pos>=10;servo1pos--){
    servoKanan.write(servo1pos);
    servo2pos = servo2pos+1;//200 - servo1pos;
    servoKiri.write(servo2pos);
    delay(5);
  }
}

// Method membaca sensor garis
void bacaSensor() {
  bool garisTerdeteksi = false;
  //   1   1   0   1   1
  if (digitalRead(sens_tengah) == 0) { 
    gerakMaju();
    // Serial.println("Sensor tengah mendeteksi garis, motor bergerak maju");
    garisTerdeteksi = true;       //Menetapkan kondisi penetapan garis
    lastDetectionTime = millis(); //Mulai menghitung waktu 
    // lastAction = "maju";          //Menyimpan kondisi terbaru
  }
  ///////BELOK KANANN////////
  //   1   1   1   1   0
  else if (digitalRead(sens_kanan1) == 0) {
    belokKanan();
    // Serial.println("Sensor kanan mendeteksi garis, motor belok kanan");
    garisTerdeteksi = true;
    lastDetectionTime = millis();
    lastAction = "belokKanan";
  }
  //   1   1   1   0   0
  else if (digitalRead(sens_kanan1) == 0 && digitalRead(sens_kanan2) == 0) { 
    belokKanan();
    // Serial.println("Sensor kanan mendeteksi garis, motor belok kanan");
    garisTerdeteksi = true;
    lastDetectionTime = millis();
    lastAction = "belokKanan";
  }
  //   1   1   1   0   1
  else if (digitalRead(sens_kanan2) == 0) {
    serongKanan();
    // Serial.println("Sensor kanan mendeteksi garis, motor serong kanan");
    garisTerdeteksi = true;
    lastDetectionTime = millis();
    lastAction = "serongKanan";
  }
  //   1   1   0   0   1
  else if (digitalRead(sens_tengah) == 0 && digitalRead(sens_kanan2) == 0) {
    serongKanan();
    // Serial.println("Sensor kanan mendeteksi garis, motor serong kanan");
    garisTerdeteksi = true;
    lastDetectionTime = millis();
    lastAction = "serongKanan";
  }
  //////BELOK KIRIII///////
  //   0   1   1   1   1
  else if (digitalRead(sens_kiri1) == 0) {
    belokKiri();
    // Serial.println("Sensor kiri mendeteksi garis, motor belok kiri");
    garisTerdeteksi = true;
    lastDetectionTime = millis();
    lastAction = "belokKiri";
  }
  //   0   0   1   1   1
  else if (digitalRead(sens_kiri1) == 0 && digitalRead(sens_kiri2) == 0) {
    belokKiri();
    // Serial.println("Sensor kiri mendeteksi garis, motor belok kiri");
    garisTerdeteksi = true;
    lastDetectionTime = millis();
    lastAction = "belokKiri";
  }
  //   1   0   1   1   1
  else if (digitalRead(sens_kiri2) == 0) {
    serongKiri();
    // Serial.println("Sensor kiri mendeteksi garis, motor serong kiri");
    garisTerdeteksi = true;
    lastDetectionTime = millis();
    lastAction = "serongKiri";
  }
  //   1   0   0   1   1
  else if (digitalRead(sens_tengah) == 0 && digitalRead(sens_kiri2) == 0) {
    serongKiri();
    // Serial.println("Sensor kiri mendeteksi garis, motor belok kiri");
    garisTerdeteksi = true;
    lastDetectionTime = millis();
    lastAction = "serongKiri";
  }
  else if (digitalRead(sens_tengah) == 1 && digitalRead(sens_kiri1) == 1 && digitalRead(sens_kiri2) == 1 && digitalRead(sens_kanan1) && digitalRead(sens_kanan2) == 1){
    //Selama rentang waktu 2 detik, robot akan menggunakan kondisi terbarunya untuk jalan
    //Jika tidak mendeteksi garis selama 2 detik maka robot akan berhenti
    if (millis() - lastDetectionTime < noDetectionTimeout){
      if (lastAction == "belokKanan"){
        belokKanan();
      } 
      // else if (lastAction == "maju") {
      //   gerakMaju();
      else if (lastAction == "serongKanan") {
        serongKanan();
      } else if (lastAction == "belokKiri") {
        belokKiri();
      } else if (lastAction == "serongKiri") {
        serongKiri();
      }
    } else {
      if (readytoFinish == false){
        stopMotors();
        // delay(500);
        putarBalik();
        // delay(500);
        TutupServoPerlahan();
        // delay(500);
        stopMotors();
        readytoFinish = true;
      }
      else if(readytoFinish == true){
        stopMotors();
      }
    } 
  }
  delay(1);
}

void sensBack(){ 
  if (digitalRead(sens_tengah) == 0) {
    moveBackward();
    Serial.println("Sensor tengah mendeteksi garis, motor bergerak maju");
    lastActionMundur = "moveback";
  } 
  ///////BELOK KANANN////////
  // Cek sensor kiri untuk belok kanan
  else if (digitalRead(sens_kiri1) == 0 || digitalRead(sens_kiri2) == 0 || (digitalRead(sens_kiri2) && digitalRead(sens_kiri1))) {
    rightBackward();
    Serial.println("Sensor kanan mendeteksi garis, motor serong kanan");
    lastActionMundur = "kananBelakang";
  }
  //////BELOK KIRIII///////
  // Cek sensor kanan untuk belok kiri
  else if (digitalRead(sens_kanan1) == 0 || digitalRead(sens_kanan2) == 0 || (digitalRead(sens_kanan1) && digitalRead(sens_kanan2))) {
    leftBackward();
    Serial.println("Sensor kanan mendeteksi garis, motor belok kanan");
    lastActionMundur = "belokKiri";
  }
  // Jika tidak ada garis, gunakan kondisi sebelumnya
  else if (digitalRead(sens_tengah) == 1 && digitalRead(sens_kiri1) == 1 && digitalRead(sens_kiri2) == 1 && digitalRead(sens_kanan1) == 1 && digitalRead(sens_kanan2) == 1){
    Serial.println("Tidak ada garis, motor berhenti");
    if (lastActionMundur == "moveback"){
      moveBackward();
    } else if (lastActionMundur == "kananBelakang") {
      rightBackward();
    } else if (lastActionMundur == "kiriBelakang") {
      leftBackward();
    } 
  }
  delay(1);  
}

// Fungsi untuk memutar motor maju (lurus)
void gerakMaju() {
  // Motor 1 dan motor 2 maju
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 200);   

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 200);  
}

void melambat(){ //melambat secara mundur
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 30);  

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 30);  
  delay(500);
  stopMotors();
}

// Fungsi untuk belok kanan
void belokKanan() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 0);  

  digitalWrite(in3, LOW);  
  digitalWrite(in4, HIGH);
  analogWrite(enB, 255);  
}

// Fungsi untuk belok kanan
void serongKanan() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 20);  

  digitalWrite(in3, LOW);  
  digitalWrite(in4, HIGH);
  analogWrite(enB, 200); 
}

// Fungsi untuk belok kiri
void belokKiri() {
  digitalWrite(in1, LOW);  
  digitalWrite(in2, HIGH);
  analogWrite(enA, 255);  

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 0);  
}

// Fungsi untuk belok kiri
void serongKiri() {
  digitalWrite(in1, LOW);  
  digitalWrite(in2, HIGH);
  analogWrite(enA, 200);  

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 20);  
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  delay(1000);
}

void putarBalik() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 100);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 100);
  // Putar sampai sensor kanan mendeteksi garis
  while (true) {
    // Cek apakah sensor kanan mendeteksi garis
    if (digitalRead(sens_kiri1) == 0 || digitalRead(sens_kiri2) == 0) {
      // Jika mendeteksi garis, berhenti memutar
      stopMotors();
      Serial.println("Sensor kanan mendeteksi garis, menghentikan putar balik.");
      break; // Keluar dari loop
    }
  }
}

void moveBackward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 250);

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 250);
  delay(2000);
  stopMotors();
}

void leftBackward() {
  digitalWrite(in1, HIGH);  
  digitalWrite(in2, LOW);
  analogWrite(enA, 60);  

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 40);  
}

void rightBackward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 40);

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 60);
}

void mundur(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 250);

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 250);
  delay(1000);
  stopMotors();
  delay(1000);
  putarBalik();
}

// Robot mundur hingga menyentuh troli
void mundurNabrak(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 45);

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 45);
}

// Robot maju hingga menemukan sebuah garis
void majuCariGaris(){
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(enA, 40); 

    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enB, 40);  

    while (true) {
      // Cek apakah sensor kanan mendeteksi garis
      if (digitalRead(sens_tengah) == 0 || digitalRead(sens_kiri1) == 0 || digitalRead(sens_kiri2) == 0 || digitalRead(sens_kanan1) == 0 || digitalRead(sens_kanan2) == 0) {
        // Jika mendeteksi garis, berhenti memutar
        Serial.println("Sensor tengah mendeteksi garis, mengikuti jalur menuju finish.");
        mundurTroli = false;
        break; // Keluar dari loop
    }
  }
}