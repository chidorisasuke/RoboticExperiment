#include <Servo.h>

Servo myServo;  // Membuat objek servo

void setup() {
  myServo.attach(13);  // Menghubungkan servo ke pin digital 9
  Serial.begin(9600); // Inisialisasi komunikasi serial untuk monitoring
}

void loop() {
  // Menggerakkan servo dari 0 hingga 180 derajat
  for (int angle = 0; angle <= 180; angle += 10) {
    myServo.write(angle);           // Setel sudut servo
    Serial.print("Servo bergerak ke: ");
    Serial.print(angle);
    Serial.println(" derajat");
    delay(500);  // Beri waktu servo untuk bergerak
  }

  // Mengembalikan servo dari 180 hingga 0 derajat
  for (int angle = 180; angle >= 0; angle -= 10) {
    myServo.write(angle);           // Setel sudut servo
    Serial.print("Servo bergerak ke: ");
    Serial.print(angle);
    Serial.println(" derajat");
    delay(500);  // Beri waktu servo untuk bergerak
  }

  delay(1000);  // Jeda 1 detik sebelum siklus ulangi
}
