//here is a general code shown here and will be personalised for each teacher

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// RFID Setup
#define SS_PIN 2  // D4
#define RST_PIN 0 // D3
MFRC522 rfc(SS_PIN, RST_PIN);

const char* ssid = "Student Wifi";
const char* password = "Student@2425";
String GOOGLE_SCRIPT_ID = "SheetIdForEachTeacher"; 

void setup() {
  Serial.begin(115200);
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  showMsg("Connecting...", "WiFi");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  SPI.begin();
  rfc.PCD_Init();
  showMsg("System Ready", "Scan Card");
}

void loop() {
  if (!rfc.PICC_IsNewCardPresent() || !rfc.PICC_ReadCardSerial()) return;

  String cardID = "";
  for (byte i = 0; i < rfc.uid.size; i++) {
    cardID += String(rfc.uid.uidByte[i] < 0x10 ? "0" : "");
    cardID += String(rfc.uid.uidByte[i], HEX);
  }
  cardID.toUpperCase();

  showMsg("Scanning...", cardID);
  sendData(cardID);
  delay(3000); 
  showMsg("System Ready", "Scan Card");
}

void sendData(String id) {
  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient http;
  // example for MAths Attendance
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?cardID=" + id + "&subject=Maths";
  
  if (http.begin(client, url)) {
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString(); // getName back
      showMsg("Attendance:", payload);
    } else {
      showMsg("Error", "Server Unreachable");
    }
    http.end();
  }
}

// Helper function to update the OLED screen
void showMsg(String line1, String line2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println(line1);
  display.setTextSize(2);
  display.setCursor(0, 30);
  display.println(line2);
  display.display();
}
