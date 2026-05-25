#include <Wire.h>
#include <MPU6050.h>

// İki farklı MPU6050 sensörü oluşturuldu
MPU6050 mpu1(0x68);
MPU6050 mpu2(0x69);

// MPU sensörlerinden gelen ivme verileri
int16_t ax1, ay1, az1;
int16_t ax2, ay2, az2;

// Sensör açı değerleri
float pitch1 = 0;
float pitch2 = 0;

// İki sensör arasındaki açı farkı
float diff = 0;

// Gürültüyü azaltmak için filtrelenmiş fark değeri
float smoothDiff = 0;

// PIN TANIMLARI

// Flex sensör pini
const int flexPin = A0;

// Buzzer pini
const int buzzerPin = 8;

// Kalibrasyon butonu pini
const int buttonPin = 7;

// Flex sensöründen okunan ham değer
int flexValue = 0;

//  KALİBRASYON DEĞİŞKENLERİ 

// Dik duruş sırasında alınan referans açı farkı
float baseDiff = 0;

// Dik duruş sırasında alınan referans flex değeri
int baseFlex = 0;

// Sistem kalibre edildi mi?
bool calibrated = false;

//  STABİLİTE KONTROLÜ 

// Kaç döngüdür kötü duruş olduğu sayılıyor
int stableCount = 0;

// Kullanıcı kötü duruşta mı?
bool isBad = false;

//  EŞİK DEĞERLERİ 

// Kalibrasyon sırasında izin verilen maksimum açı farkı
const float CALIB_DIFF_LIMIT = 18.0;

// Flex sensörü için minimum kabul edilen değer
const int FLEX_MIN_LIMIT = 250;

// Flex sensörü için maksimum kabul edilen değer
const int FLEX_MAX_LIMIT = 900;

// Kötü duruş için açı fark limiti
const float BAD_DIFF_LIMIT = 10.0;

// Çok kötü duruş için açı fark limiti
const float VERY_BAD_DIFF_LIMIT = 18.0;

// Kötü duruş için flex fark limiti
const int BAD_FLEX_LIMIT = 25;

// Çok kötü duruş için flex fark limiti
const int VERY_BAD_FLEX_LIMIT = 40;

void setup() {

  // Seri haberleşme başlatılıyor
  Serial.begin(9600);

  // I2C haberleşmesi başlatılıyor
  Wire.begin();

  // MPU sensörleri başlatılıyor
  mpu1.initialize();
  mpu2.initialize();

  // Pin modları ayarlanıyor
  pinMode(flexPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // INPUT_PULLUP:
  // Buton normalde HIGH olur,
  // basınca LOW olur
  pinMode(buttonPin, INPUT_PULLUP);

  delay(1000);

  Serial.println("Postur Takip Sistemi Basladi");

  // MPU1 bağlantı kontrolü
  if (!mpu1.testConnection()) {

    Serial.println("HATA: MPU1 baglantisi yok! 0x68 kontrol et.");
    errorBeep();

  } else {

    Serial.println("MPU1 baglandi.");
  }

  // MPU2 bağlantı kontrolü
  if (!mpu2.testConnection()) {

    Serial.println("HATA: MPU2 baglantisi yok! 0x69 kontrol et.");
    errorBeep();

  } else {

    Serial.println("MPU2 baglandi.");
  }

  // Kullanıcıdan dik durup butona basması isteniyor
  Serial.println("Dik dur ve kalibrasyon icin butona bas.");
}

void loop() {

  // Sensörler okunuyor
  readSensors();

  // Eğer sistem henüz kalibre edilmediyse
  if (!calibrated) {

    // Seri monitöre anlık değerler yazdırılıyor
    Serial.print("Kalibrasyon bekleniyor | Diff: ");
    Serial.print(diff);

    Serial.print(" | Flex: ");
    Serial.println(flexValue);

    // Butona basıldı mı kontrolü
    if (buttonPressed()) {

      Serial.println("Butona basildi. Kalibrasyon kontrol ediliyor...");

      // Kullanıcı düzgün pozisyonda mı?
      if (isCalibrationPositionValid()) {

        // Başarılı kısa bip sesi
        successBeepShort();

        Serial.println("Kalibrasyon basliyor. 2 saniye hareket etme...");

        // Referans değerler hesaplanıyor
        calibrateSystem();

        // Sistem artık kalibre edildi
        calibrated = true;

        // Filtre başlangıç değeri
        smoothDiff = baseDiff;

        Serial.print("Kalibrasyon tamamlandi | BaseDiff: ");
        Serial.print(baseDiff);

        Serial.print(" | BaseFlex: ");
        Serial.println(baseFlex);

        // Uzun başarılı bip sesi
        successBeepLong();
      }

      // Kalibrasyon başarısız
      else {

        Serial.println("Kalibrasyon reddedildi!");
        Serial.println("Daha dik dur veya sensorleri kontrol et.");

        errorBeep();
      }

      delay(700);
    }

    delay(300);
    return;
  }

  //  LOW PASS FILTER 
  // Ani değişimleri azaltmak için filtre uygulanıyor

  smoothDiff = 0.8 * smoothDiff + 0.2 * diff;

  // Referans duruş ile mevcut duruş farkı
  float diffError = abs(smoothDiff - baseDiff);

  // Flex sensörü farkı
  int flexError = abs(baseFlex - flexValue);

  //  STABİLİTE KONTROLÜ 
  // Kullanıcı kısa süreli hareket ettiyse yanlış alarm verilmesin diye

  if (diffError > BAD_DIFF_LIMIT || flexError > BAD_FLEX_LIMIT) {

    stableCount++;

  } else {

    stableCount = 0;
  }

  // 3 döngü boyunca kötü duruş varsa
  isBad = stableCount > 3;

  //  DURUŞ KARARI 

  // Çok kötü duruş
  if (diffError > VERY_BAD_DIFF_LIMIT &&
      flexError > VERY_BAD_FLEX_LIMIT) {

    Serial.println("VERY BAD POSTURE");

    // Sürekli yüksek ses
    tone(buzzerPin, 2000);
  }

  // Normal kötü duruş
  else if (isBad) {

    Serial.println("BAD POSTURE");

    // Kesik kesik uyarı sesi
    tone(buzzerPin, 1000);
    delay(100);

    noTone(buzzerPin);
    delay(100);
  }

  // İyi duruş
  else {

    Serial.println("GOOD POSTURE");

    noTone(buzzerPin);
  }

  // Debug için tüm veriler yazdırılıyor
  Serial.print("DiffErr: ");
  Serial.print(diffError);

  Serial.print(" | FlexErr: ");
  Serial.print(flexError);

  Serial.print(" | Diff: ");
  Serial.print(diff);

  Serial.print(" | Flex: ");
  Serial.println(flexValue);

  delay(100);
}

void readSensors() {

  //  MPU1 OKUMA 

  mpu1.getAcceleration(&ax1, &ay1, &az1);

  // Ham veriler g kuvvetine çevriliyor
  float AccX1 = ax1 / 16384.0;
  float AccY1 = ay1 / 16384.0;
  float AccZ1 = az1 / 16384.0;

  // Pitch açısı hesaplanıyor
  pitch1 = atan2(
             AccX1,
             sqrt(AccY1 * AccY1 + AccZ1 * AccZ1)
           ) * 180 / PI;

  //  MPU2 OKUMA 

  mpu2.getAcceleration(&ax2, &ay2, &az2);

  float AccX2 = ax2 / 16384.0;
  float AccY2 = ay2 / 16384.0;
  float AccZ2 = az2 / 16384.0;

  pitch2 = atan2(
             AccX2,
             sqrt(AccY2 * AccY2 + AccZ2 * AccZ2)
           ) * 180 / PI;

  // İki sensör arasındaki açı farkı
  diff = pitch1 - pitch2;

  // Flex sensörü okunuyor
  flexValue = analogRead(flexPin);
}

// Kullanıcı düzgün pozisyonda mı kontrolü
bool isCalibrationPositionValid() {

  readSensors();

  Serial.print("Kontrol Diff: ");
  Serial.print(diff);

  Serial.print(" | Flex: ");
  Serial.println(flexValue);

  // Açı farkı çok fazlaysa
  if (abs(diff) > CALIB_DIFF_LIMIT) {

    return false;
  }

  // Flex değeri sınır dışındaysa
  if (flexValue < FLEX_MIN_LIMIT ||
      flexValue > FLEX_MAX_LIMIT) {

    return false;
  }

  return true;
}

// Referans değerleri hesaplayan fonksiyon
void calibrateSystem() {

  float sumDiff = 0;
  long sumFlex = 0;

  // 100 örnek alınıyor
  for (int i = 0; i < 100; i++) {

    readSensors();

    sumDiff += diff;
    sumFlex += flexValue;

    delay(20);
  }

  // Ortalama değerler hesaplanıyor
  baseDiff = sumDiff / 100.0;
  baseFlex = sumFlex / 100;
}

// Buton debounce kontrolü
bool buttonPressed() {

  // Butona basıldı mı?
  if (digitalRead(buttonPin) == LOW) {

    // Titreşim önleme gecikmesi
    delay(30);

    // Hala basılı mı?
    if (digitalRead(buttonPin) == LOW) {

      // Buton bırakılana kadar bekle
      while (digitalRead(buttonPin) == LOW) {

        delay(10);
      }

      return true;
    }
  }

  return false;
}

// Kısa başarılı bip sesi
void successBeepShort() {

  tone(buzzerPin, 1500);
  delay(150);

  noTone(buzzerPin);
}

// Uzun başarılı bip sesi
void successBeepLong() {

  tone(buzzerPin, 2000);
  delay(400);

  noTone(buzzerPin);
}

// Hata sesi
void errorBeep() {

  tone(buzzerPin, 400);
  delay(500);

  noTone(buzzerPin);
}