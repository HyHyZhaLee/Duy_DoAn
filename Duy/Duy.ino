#define BLYNK_TEMPLATE_ID "TMPL6ldAqaCJv"
#define BLYNK_TEMPLATE_NAME "ANHDUY"
#define BLYNK_AUTH_TOKEN "iMZobFN-qOYNqajSEYSWvjKCk056WCpQ"

#define WIFI_SSID "Kiet Huy"
#define WIFI_PASS "tumot_den9"

#define WATER_HOUR 6
#define WATER_MINUTE 32
#define WATER_DURATION 5

#define DHTPIN 12      // Chân IO12 kết nối cảm biến DHT11 với ESP32
#define DHTTYPE DHT11 // Loại cảm biến DHT (DHT11 hoặc DHT22)
#define SOIL_PIN 2
#define RELAY_PIN 14

#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "wifi_setup.h"
#include <TimeLib.h>
#include <BlynkSimpleEsp32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200);

LiquidCrystal_I2C lcd(0x27, 16, 2);
Wifi_esp32 wifi(WIFI_SSID, WIFI_PASS);
DHT dht(DHTPIN, DHTTYPE);

float soilMoisturePercent;
float humidityPercent;
float temperature;
int button;

void setup() {

  Serial.begin(115200);
  dht.begin();
  pinMode(SOIL_PIN, INPUT); // Khai báo chân cảm biến đất là INPUT
  pinMode(RELAY_PIN, OUTPUT); // Khai báo chân relay là OUTPUT
  digitalWrite(RELAY_PIN, LOW); // Khởi đầu, chúng ta sẽ tắt relay
  setupLCD();
  wifi.setupWifi();

  Blynk.begin(BLYNK_AUTH_TOKEN, "Kiet Huy", "tumot_den9");
  timeClient.begin();

}

BLYNK_WRITE(V3) {
  button = param.asInt();
  if(button == 1) {
    digitalWrite(RELAY_PIN , HIGH);
  } else {
    digitalWrite(RELAY_PIN , LOW);
  }
}
bool water = 0;
void loop() {
  Blynk.run(); // Cần phải gọi Blynk.run() thường xuyên để Blynk hoạt động chính xác
  timeClient.update();
  // Kiểm tra nếu là 4PM thì tưới cây trong 5 giây
 if (timeClient.getHours() == WATER_HOUR && timeClient.getMinutes() == WATER_MINUTE) {
    if(water == false){
      digitalWrite(RELAY_PIN, HIGH); // Bật relay
      delay(WATER_DURATION*1000); // Tưới cây 
      digitalWrite(RELAY_PIN, LOW); // Tắt relay}
    }
    water = true;
  }
  else water = false;
  
  // Đọc dữ liệu từ cảm biến và hiển thị lên LCD
  updateSensors();
  // Gửi dữ liệu lên Blynk
  sendDatatoBlynk();
  delay(1000);
}

void updateSensors(){
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
}

void sendDatatoBlynk(){
  // Cập nhật dữ liệu lên Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidityPercent);
  Blynk.virtualWrite(V2, soilMoisturePercent);
  // Bạn có thể cập nhật trạng thái relay bằng cách sử dụng virtualWrite cho V3 tương tự như trên
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