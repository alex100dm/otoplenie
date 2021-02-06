
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
//#include <TimeLib.h>
//#include <WidgetRTC.h>
#include <ElegantOTA.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

//const char* ssid = "CKBEPHA";
//const char* password = "6001018p";
//const char* ssid = "SSID";
//const char* password = "555666777";
const char* ssid = "CBEPX CEKPETHO";
const char* password = "555666777";
//char auth[] = "ZeNApkzJEqzgRcMkdAYS1rXSm4r5DoG4";
char auth[] = "U8BqPsa1IJdXfhanNrlXdPK3nl4XSNZx "; // кеотельная в основном

#define BLYNK_NO_FLOAT  // Disable float operations
#define ONE_WIRE_BUS_D_KETEL 5  // D1
#define ONE_WIRE_BUS_TO_HOUSE 4      // D2
#define CLK 12   //  D6 
#define DIO 14   //  D5
#define KNOPA 0    //    D3
#include "GyverTM1637.h"
GyverTM1637 disp(CLK, DIO);

#define REQUIRESNEW false     // установите значение true, чтобы включить код для операторов new и delete
#define REQUIRESALARMS false  // установите значение true, чтобы включить код, реализующий функции поиска сигналов тревоги

WidgetBridge bridge1(V0);
WidgetLCD lcd(V1);
#define BLYNK_PIN_Grafik V2
#define BLYNK_PIN_period_get_temp V3
#define BLYNK_PIN_Ota V4
#define BLYNK_PIN_temp_out_d_kotel V5
#define BLYNK_PIN_temp_in_d_kotel V6
#define BLYNK_PIN_temp_to_house V7
#define BLYNK_PIN_temp_iz_house V8


unsigned long timer_FuncGrafik;
unsigned long lastConnectionAttempt;
unsigned long timer_get_temp = 0;   //
uint32_t btnTimer = 0;

unsigned int period_Grafika = 20000;
unsigned int period_get_temp = 6000;
  

uint8_t btn_pli = 0;
uint8_t WiFi_Faled_Counter = 0;
uint8_t Disp_data_idex = 0;

//uint8_t DispData[] = { 0, 0, 0, 0 };

unsigned long timer_izm = 0;   //

int temp_out_d_kotel;
int temp_in_d_kotel;
int temp_to_house;
int temp_iz_house;

byte hrs = 21, mins = 55;
uint32_t tmrClock;
  
uint8_t Err_temp_out_d_kotel;
uint8_t Err_temp_in_d_kotel;
uint8_t Err_temp_to_house;
uint8_t Err_temp_iz_house;

uint8_t myHour;
uint8_t myMin;

float tempC;

bool state = true;
bool flag_btn = false;
boolean Ota = false;
boolean app_connect;
bool flagClock = false;


OneWire oneWire_D_KETEL(ONE_WIRE_BUS_D_KETEL);
OneWire oneWire_TO_HOUSE(ONE_WIRE_BUS_TO_HOUSE);
DallasTemperature sensor_D_KETEL(&oneWire_D_KETEL);
DallasTemperature sensor_TO_HOUSE (&oneWire_TO_HOUSE);
DeviceAddress Addr_DS_temp_out_d_kotel = {  0x28, 0x33, 0xD0, 0xD8, 0x1E, 0x19, 0x01, 0x57  }; // a4
DeviceAddress Addr_DS_temp_in_d_kotel  = {  0x28, 0x96, 0x41, 0xB7, 0x1E, 0x19, 0x01, 0x90  }; // a5
DeviceAddress Addr_DS_temp_to_house    = { 0x28, 0x9D, 0x59, 0xFF, 0x00, 0x00, 0x00, 0x68 }; // dds18b20 без провода m1
DeviceAddress Addr_DS_temp_iz_house    = { 0x28, 0xFF, 0x64, 0x1D, 0x00, 0x6E, 0xA9, 0x56 }; // dds18b20 без провода m2
ESP8266WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

BLYNK_CONNECTED() {
  if ( millis() < 60000) Blynk.syncAll();
  bridge1.setAuthToken("Ar_abuKKjGRIYF3ATQQgDE5Bovun61eN"); // 
}
BLYNK_WRITE(BLYNK_PIN_period_get_temp) { //
  period_get_temp = param.asInt() * 1000;
}
BLYNK_WRITE(BLYNK_PIN_Grafik) { // 
  period_Grafika = param.asInt() * 1000;
}
BLYNK_WRITE(BLYNK_PIN_Ota) { // 0-1 
  Ota = param.asInt();
}
/*
BLYNK_APP_CONNECTED(){
  app_connect = 1;
}
BLYNK_APP_DISCONNECTED(){
  app_connect = 0;
}

void My_serial() {
  Serial.print(F("a4 (out_d_kotel):")); // oП
  Serial.print( temp_out_d_kotel ); // a4
  Serial.print(F(" | a5 (in_d_kotel):")); // оО
  Serial.print(temp_in_d_kotel);  // a4
  Serial.print(F("   |   m1 (to_house):")); // "-П:"
  Serial.print( temp_to_house ); // m1
  Serial.print(F(" | m2 (iz_house):")); // 
  Serial.println(temp_iz_house); // m2
}*/
void FuncGrafik() {
  if (millis() - timer_FuncGrafik > period_Grafika ) {
    bridge1.virtualWrite(V50, temp_out_d_kotel);
    bridge1.virtualWrite(V51, temp_in_d_kotel);
    bridge1.virtualWrite(V52, temp_to_house);
    bridge1.virtualWrite(V53, temp_iz_house);
    /*
    Blynk.virtualWrite(BLYNK_PIN_temp_out_d_kotel, temp_out_d_kotel); //
    Blynk.virtualWrite(BLYNK_PIN_temp_in_d_kotel, temp_in_d_kotel);
    Blynk.virtualWrite(BLYNK_PIN_temp_to_house, temp_to_house);
    Blynk.virtualWrite(BLYNK_PIN_temp_iz_house, temp_iz_house);
    */
    timer_FuncGrafik = millis();
  }
}
bool GetTempD_KETEL() {
  sensor_D_KETEL.requestTemperatures();
  tempC = sensor_D_KETEL.getTempC(Addr_DS_temp_out_d_kotel); // float getTempC(const uint8_t*)
  if(tempC == DEVICE_DISCONNECTED_C) {
    if ( !Err_temp_out_d_kotel) Serial.println(F("датчик подачи от д.котла OFF"));//Serial.println(F("Hello, World!"));
    Err_temp_out_d_kotel++;  // uint8_t
    if (Err_temp_out_d_kotel > 4) temp_out_d_kotel = 0;
  } else {
    temp_out_d_kotel = round(tempC);
    if ( Err_temp_out_d_kotel) Serial.println(F("датчик подачи от д.котла ON"));
    Err_temp_out_d_kotel = 0;
  }
  tempC = sensor_D_KETEL.getTempC(Addr_DS_temp_in_d_kotel); // float getTempC(const uint8_t*)
  if(tempC == DEVICE_DISCONNECTED_C) {
    if ( !Err_temp_in_d_kotel) Serial.println(F("датчик обратки в д.котлёл OFF"));
    Err_temp_in_d_kotel++;   // uint8_t
    if (Err_temp_in_d_kotel > 4) temp_in_d_kotel = 0;
  } else {
    temp_in_d_kotel = round(tempC);
    if ( Err_temp_in_d_kotel) Serial.println(F("датчик обратки в д.котлёл ON"));
    Err_temp_in_d_kotel = 0;   // uint8_t;
  }
  if ( Err_temp_out_d_kotel && Err_temp_in_d_kotel) return false;
  else return true;
}
bool GetTempTO_HOUSE() {
  sensor_TO_HOUSE.requestTemperatures();
  tempC = sensor_TO_HOUSE.getTempC(Addr_DS_temp_to_house); // float getTempC(const uint8_t*)
  if(tempC == DEVICE_DISCONNECTED_C) {
    if ( !Err_temp_to_house) Serial.println(F("датчик подачи в дом OFF"));
    Err_temp_to_house++;   // uint8_t
    if (Err_temp_to_house > 4) temp_to_house = 0;
  } else {
    temp_to_house = round(tempC);
    if ( Err_temp_to_house) Serial.println(F("датчик подачи в дом ON"));
    Err_temp_to_house = 0;   // uint8_t
  }
  tempC = sensor_TO_HOUSE.getTempC(Addr_DS_temp_iz_house); // float getTempC(const uint8_t*)
  if(tempC == DEVICE_DISCONNECTED_C) {
    if ( !Err_temp_iz_house) Serial.println(F("датчик обратка из дома OFF"));
    Err_temp_iz_house++;   // uint8_t
    if (Err_temp_iz_house > 4) temp_iz_house = 0;
  } else {
    temp_iz_house = round(tempC);
    if ( Err_temp_iz_house) Serial.println(F("датчик обратка из дома ON"));
     Err_temp_iz_house = 0;
  } 
  if ( Err_temp_to_house && Err_temp_iz_house) return false;
  else return true;
}
void MyClock() {
  if (millis() - tmrClock > 500) {       // каждые полсекунды
    timeClient.update();
    tmrClock = millis();
    flagClock = !flagClock;
    disp.point(flagClock);   // выкл/выкл точки
    if (flagClock) {
      myHour = timeClient.getHours() ;
      if (myHour < 21 ) myHour+= 3;
      else myHour = myHour - 21;
      myMin = timeClient.getMinutes();
      disp.displayClock(myHour, myMin);    // выводим время
    }
  }
}
void To_int_disp_temp( uint8_t i ) { //   0-оП  1-оО  2-uП  3-uO
  disp.point(1);   // выкл точки
  
  switch (i) { //   0-оП  1-оО  2-uП  3-uO                  0-дровПодача  1-дровОбратка  2-домПодача  3-домОбратка
    case 0:
      //disp.twist( DispData , 50);
      disp.clear();
      disp.point(0);   // выкл точки
      disp.twist(0, 0, 40); 
      disp.clear();
      break;
    case 1:
      disp.displayInt( temp_out_d_kotel );
      disp.displayByte(0, 0x23);     // п 0x54
      disp.point(1);   // выкл точки
      break;
    case 2:
      disp.displayInt( temp_in_d_kotel );
      disp.displayByte(0, 0x63);          // o 0x5C  disp.twistByte(0, 0x5C, 10);  0x5C
      disp.point(1);   // выкл точки
      break;
    case 3: //  klimat 1
      disp.displayInt( temp_to_house );
      disp.displayByte(0, 0x37 );// П
      disp.point(1);   // выкл точки
      break;
    case 4:
      disp.displayInt( temp_iz_house );
      disp.displayByte(0, 0x3F); // O
      disp.point(1);   // выкл точки
    //default:
  }
  if ( Blynk.connected() ) {
    if ( state ) {
        lcd.print(0, 0, "п:"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
        lcd.print(2, 0, temp_out_d_kotel ); // 
        lcd.print(5, 0, "котёл  о:" ); // 5 -10
        //lcd.print(12, 0, "о:" ); //
        lcd.print(14, 0, temp_in_d_kotel ); // 

        lcd.print(0, 1, "П:"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
        lcd.print(2, 1, temp_to_house ); //
        lcd.print(5, 1, "линия  О:" ); // 5 -10 
        //lcd.print(12, 1, "О:" ); //
        lcd.print(14, 1, temp_iz_house ); // 
    } else {
        
    }
  }
}
boolean connect_to_WiFi() {
  WiFi.begin(ssid, password);// Serial.print(F("Connecting to WI-FI... "));
  for (int i = 0; i < 500; i++) {
    //delay(30);
    if ( WiFi.status() == WL_CONNECTED ) {
      return true;
    }
    disp.displayInt( i );
    disp.twist(0, 0, 20); 
  }
	WiFi_Faled_Counter++;
  if (WiFi_Faled_Counter > 10) ESP.restart();
  lastConnectionAttempt = millis();
  return false;
}
void setup(void) {
    pinMode(KNOPA, INPUT_PULLUP);
    //Serial.begin(115200);
    //Serial.println(F(""));
    sensor_D_KETEL.setResolution(9); // установите глобальное разрешение на 9, 10, 11 или 12 бит
    sensor_TO_HOUSE.setResolution(9); // установите глобальное разрешение на 9, 10, 11 или 12 бит
    sensor_D_KETEL.begin();
    sensor_TO_HOUSE.begin();
    disp.clear();
    disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)
    Blynk.config(auth);
    if ( !connect_to_WiFi() ) WiFi_Faled_Counter++;
    timeClient.begin();
}
void loop(void){
  if ( Ota ) {
    Blynk.disconnect();
    server.on("/", []() { server.send(200, "text/plain", "1"); });
    ElegantOTA.begin(&server); server.begin();
    unsigned long now1 = millis();
    disp.point(0);
    uint8_t strOta[] = { _empty, _O, _t, _A };
    disp.scrollByte( strOta , 100); 
    while (millis() - now1 < 200000) {
      server.handleClient();
      //if ( !digitalRead(KNOPA)) break; 
    }
    ESP.restart();
  } else {
    bool btnState = !digitalRead(KNOPA);
    if (btnState && !flag_btn && millis() - btnTimer > 100) {
      flag_btn = true;
      btnTimer = millis();
      //disp.clear();
      btn_pli++;
      if ( btn_pli > 5 ) btn_pli = 0;
      Disp_data_idex = btn_pli;
      To_int_disp_temp( btn_pli );
    }
    if (!btnState && flag_btn && millis() - btnTimer > 100) {
      flag_btn = false;
      btnTimer = millis();
    }
    if ( millis() - timer_get_temp > period_get_temp / 2 ) {
      state = !state;
      if (state) GetTempD_KETEL();
      else GetTempTO_HOUSE();
      timer_get_temp = millis();

      if ( millis() - btnTimer > 300) {
        //if ( Disp_data_idex != 5 ){
          if ( !btn_pli ) {
            Disp_data_idex++;
            if ( Disp_data_idex > 5 ) Disp_data_idex = 1;
          }
          To_int_disp_temp( Disp_data_idex ); //   0-оП  1-оО  2-uП  3-uO
        //}
      }
    }
    if (WiFi.status() != WL_CONNECTED) {  //  "offline"  code
      if (millis() - lastConnectionAttempt > 40000) connect_to_WiFi(); // переподключение через 30 сек
      if ( Disp_data_idex == 5 ) {
        disp.clear();
        disp.point(0);   // выкл точки
        disp.twist(3, 0, 40); 
      }
    } else {
      Blynk.run();  // yield(); now = millis(); if ( Blynk.connected() )
      if ( WiFi_Faled_Counter ) WiFi_Faled_Counter = 0;
      if ( period_Grafika ) FuncGrafik();       //timer.run(); timerMin.run();
      if ( Disp_data_idex == 5 ) MyClock();
    }
  }
}
