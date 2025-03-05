#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#define WIFI_SSID "Thu Thao"
#define WIFI_PASSWORD "12052002"
#define DATABASE_URL "esp01-e45db-default-rtdb.firebaseio.com"
#define API_KEY "mCqk75ID2ze3JAay8f94AHfLqw7wdQqtMoYY2VeS"

#include <ArduinoJson.h>
FirebaseData firebaseData;
FirebaseJson json;

#define doam_pin A0 /////Chân cảm biến độ ẩm
#define nutnhan D4  //Chân nút nhấn
#define relay D5    ///Chân relay
#define Mode D7 //Chân chọn chế độ
#include "DHT.h"

#define DHTPIN D6   //Chân Cảm biến DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int giatri,i;
String chedo = " ";
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
long times;
int gioihan = 60;
void setup() {
  Serial.begin(9600);
  pinMode(doam_pin, INPUT);
  pinMode(nutnhan, INPUT_PULLUP);
  pinMode(Mode, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  dht.begin();
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");}
  // put your setup code here, to run once:
Firebase.begin(DATABASE_URL,API_KEY);

  delay(1000);
 

}

void loop() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  int val = analogRead(doam_pin);
  int doam = map(val, 512, 1024, 100, 0); // Chuyển đổi giá trị analog ngưỡng từ 450-1024(8 bit) sang độ ẩm 0 -100%///Giá trị tối đa của analog 10 bit là 1024 nhưng chỉ chọn ngưỡng từi 0 đến 1020
  if(doam >= 100) doam = 100;
  if(doam <= 0) doam = 0;
  
 if( Firebase.getInt(firebaseData,("/DATA/CHE DO"))){
  i = firebaseData.intData();
 // Serial.println("GIA TRI " + String(giatri));
 }
 
 if(digitalRead(Mode) == 0){ //chọn chế độ
  i!=i;
  Firebase.setFloat(firebaseData, "/DATA/CHE DO",i);
  delay(300);
 }
 if(i == 1){  //Chế độ thủ công
  chedo = "THU CONG";
  if( Firebase.getInt(firebaseData,("/DATA/DIEU KHIEN"))){
  giatri = firebaseData.intData();
 }
  if(digitalRead(nutnhan) == 0){
    digitalWrite(relay, !digitalRead(relay));
    Firebase.setFloat(firebaseData, "/DATA/DIEU KHIEN",digitalRead(relay));
    delay(300);
  }
  else digitalWrite(relay, giatri);
 }
 else {
  chedo = "TU DONG";
  if(doam >= gioihan){
    digitalWrite(relay, HIGH);
  }
  else digitalWrite(relay, LOW);
 }
 //////Hiển thị chế độ và thông tin cảm biến lên LCD
  lcd.setCursor(0,0);
  lcd.print("H:    % T: ,   C");
  lcd.setCursor(3,0);
  lcd.print(h);
  lcd.setCursor(10,0);
  lcd.print(t,1);
  lcd.setCursor(0,1);
  lcd.print("M:     % S:     ");
  lcd.setCursor(12,1);
  lcd.print(chedo);
  if(millis() - times >= 5000){//Thời gian đẩy dữ liệu lên fireBase
   Firebase.setInt(firebaseData, "/DATA/DO AM DAT",doam);
  Firebase.setInt(firebaseData, "/DATA/DO AM KHONG KHI",h);
  Firebase.setFloat(firebaseData, "/DATA/NHIET DO",t);
  times = millis();
  }
}
