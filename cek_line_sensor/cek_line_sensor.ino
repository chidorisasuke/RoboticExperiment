// Pin sensor
int sensorKiri = 28;
int sensorTengah = 29;
int sensorKanan = 30;

int checkpoint = 0;  // Variabel untuk menghitung checkpoint

void setup() {
  Serial.begin(9600);

  // Inisialisasi pin sensor sebagai input
  pinMode(sensorKiri, INPUT);
  pinMode(sensorTengah, INPUT);
  pinMode(sensorKanan, INPUT);
}

void loop() {
  // Membaca status sensor
  int kiriStatus = digitalRead(sensorKiri);
  int tengahStatus = digitalRead(sensorTengah);
  int kananStatus = digitalRead(sensorKanan);

  // Jika ketiga sensor mendeteksi garis (nilai 0)
  if (kiriStatus == 0 && tengahStatus == 0 && kananStatus == 0) {
    checkpoint++;  // Tambah checkpoint
    Serial.print("Checkpoint: ");
    Serial.println(checkpoint);

    // Mengeluarkan pesan tertentu pada checkpoint tertentu
    if (checkpoint % 5 == 0) {
      Serial.println("Reached checkpoint 5");
    }
    if (checkpoint % 10 == 0) {
      Serial.println("Reached checkpoint 10");
    }
    if (checkpoint == 20) {
      Serial.println("Reached checkpoint 20, Congratulations!");
    }
    
    // Delay agar tidak menghitung checkpoint yang sama berulang kali
    delay(1000);
  }

  delay(100);  // Delay untuk menghindari pembacaan terlalu cepat
}
