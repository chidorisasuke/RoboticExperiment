// Definisikan pin trig dan echo
const int trigPin = 7;
const int echoPin = 8;

// Variabel untuk menyimpan durasi dan jarak
long duration;
int distance;

void setup() {
  // Set pin trig sebagai output dan pin echo sebagai input
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Mulai komunikasi serial untuk menampilkan data di Serial Monitor
  Serial.begin(9600);
}

void loop() {
  // Bersihkan pin trig
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Kirim sinyal ultrasonik (trigger) selama 10 mikrodetik
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Baca durasi waktu pantulan (echo)
  duration = pulseIn(echoPin, HIGH);
  
  // Hitung jarak (durasi * kecepatan suara / 2)
  distance = duration * 0.034 / 2;

  // Tampilkan jarak di Serial Monitor
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // Tambahkan sedikit jeda sebelum pengulangan berikutnya
  delay(500);
}