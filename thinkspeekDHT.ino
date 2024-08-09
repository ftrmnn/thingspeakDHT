#include <DHT.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi LCD dengan alamat I2C yang benar
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "POCO F4"; // Isi dengan SSID Wifi yang akan dikoneksikan
const char* pass = "sayaadalahkamu"; // Isi password SSID Wifi

WiFiClient client;
unsigned long myChannelNumber = 2621117; // Isi dengan Channel ID Thingspeak
const char* myWriteAPIKey = "YMX545N1AWR5HUVK"; // Isi dengan Write Key API Thingspeak

#define DHTPIN D3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

byte delaySend;

void setup() {
  Serial.begin(9600);
  
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  delay(500);  // Tambahkan jeda sebelum menampilkan teks
  lcd.print("Koneksi WiFi");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);

  // Koneksi ke WiFi
  WiFi.begin(ssid, pass);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(" ");
    Serial.println(" ");
    Serial.print("Attempting to connect to SSID: ");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("\nConnected.");
    lcd.print(" Oke...!");
  }

  delay(2000);
  lcd.clear();
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("DHT22 tidak terbaca...!");
    lcd.setCursor(0, 0);
    lcd.print("DHT22");
    lcd.setCursor(0, 1);
    lcd.print("tidak terbaca");
    return;
  }

  // Output suhu dan kelembapan ke Serial Monitor
  Serial.print("Suhu: ");
  Serial.print(t);
  Serial.println(" °C");

  Serial.print("Kelembapan: ");
  Serial.print(h);
  Serial.println(" %");

  // Tampilkan data ke LCD
  lcd.setCursor(0, 0);
  lcd.print("Suhu:");
  lcd.print(t);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humi:");
  lcd.print(h);
  lcd.print("%");

  delay(1000);
  delaySend++;

  if (delaySend < 20) return;

  // Update data ke ThingSpeak
  lcd.clear();
  lcd.print("Update DHT22");
  lcd.setCursor(0, 1);
  lcd.print("ke Thingspeak...");

  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
    lcd.clear();
    lcd.print("Successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code: " + String(x));
    lcd.clear();
    lcd.print("ERROR ");
  }

  delaySend = 0;
  delay(500);
}
