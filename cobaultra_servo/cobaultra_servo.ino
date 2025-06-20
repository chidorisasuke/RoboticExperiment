#include <Servo.h>

#define trigPin 6
#define echoPin 7

Servo servo1;  // Servo 1
Servo servo2;  // Servo 2

long duration;
int distance;

void setup() {
  Serial.begin(9600);
  
  // Attach servos to pins
  servo1.attach(8);  // Servo 1 pada pin 3
  servo2.attach(9);  // Servo 2 pada pin 5
  
  // Setup pin modes for Ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Mengirim sinyal ultrasonik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Menghitung waktu respon
  duration = pulseIn(echoPin, HIGH);
  
  // Menghitung jarak (dalam cm)
  distance = duration * 0.034 / 2;
  
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // Jika jarak lebih dari 70 cm, servo1 ke 180 derajat, servo2 ke 0 derajat
  if (distance > 70) {
    servo1.write(180);
    servo2.write(0);
  }
  // Jika jarak kurang dari atau sama dengan 70 cm, servo1 ke 0 derajat, servo2 ke 180 derajat
  else {
    servo1.write(0);
    servo2.write(180);
  }
  
  delay(500);  // Tunggu sebelum mengukur ulang
}


// #include <Servo.h>

// Servo servo1;  // Servo 1
// Servo servo2;  // Servo 1


// void setup() {
//   Serial.begin(9600);

//   // Attach servo ke pin yang lain untuk percobaan
//   servo1.attach(9);  // Coba gunakan pin 9 untuk Servo 1
//   servo2.attach(8);  // Coba gunakan pin 9 untuk Servo 1
  
  
//   Serial.println("Servo Test Mulai");
// }

// void loop() {
//   // Coba gerakkan servo ke 180 derajat
//   servo1.write(0);
//   Serial.println("Servo 1 bergerak ke 0 derajat");
//   delay(500);
//   // servo2.write(80);
//   // Serial.println("Servo 2 bergerak ke 80 derajat");
//   // delay(1000);
  
//   // Coba gerakkan servo ke 0 derajat
//   servo1.write(80);
//   Serial.println("Servo 1 bergerak ke 80 derajat");
//   delay(500);
//   // servo2.write(0);
//   // Serial.println("Servo 2 bergerak ke 0 derajat");
//   // delay(1000);
// }
