#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "wifi_setup.h"

// Khởi tạo LCD (Thay đổi địa chỉ I2C của bạn nếu cần)
LiquidCrystal_I2C lcd(0x27, 16, 2);
Wifi_esp32 wifi("Kiet Huy", "tumot_den9");

// Định nghĩa chân kết nối và loại cảm biến DHT11
#define DHTPIN 12      // Chân IO12 kết nối cảm biến DHT11 với ESP32
#define DHTTYPE DHT11 // Loại cảm biến DHT (DHT11 hoặc DHT22)
#define SOIL_PIN 2
#define RELAY_PIN 14

DHT dht(DHTPIN, DHTTYPE);

float soilMoisturePercent;
float humidityPercent;
float temperature;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(SOIL_PIN, INPUT); // Khai báo chân cảm biến đất là INPUT
  setupLCD();
  wifi.setupWifi();
}

void loop() {
  // Đọc độ ẩm từ cảm biến đất
  int soilMoistureValue = analogRead(SOIL_PIN);
  soilMoisturePercent = map(soilMoistureValue, 0, 4095, 0, 100); // Gán giá trị từ 0-4095 thành phần trăm
  // Đọc độ ẩm và nhiệt độ từ DHT11
  humidityPercent = dht.readHumidity();
  temperature = dht.readTemperature();
  
  lcd.setCursor(2,0);
  lcd.print(humidityPercent);
  lcd.setCursor(9,0);
  lcd.print(temperature);
  lcd.setCursor(9,1);
  lcd.print(soilMoisturePercent);

  delay(2000); // Chờ 2 giây trước khi đọc tiếp
}

void setupLCD(){
  byte degree[8] = {
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
  };
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(1, degree);

  lcd.setCursor(0, 0);
  lcd.print("H:");
  lcd.setCursor(7, 0);
  lcd.print("%|");
  lcd.setCursor(14, 0);
  lcd.write(1);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Moisture: ");
  lcd.setCursor(14, 1);
  lcd.print("%");
}