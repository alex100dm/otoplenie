#define BLYNK_NO_BUILTIN   // Disable built-in analog & digital pin operations
#define BLYNK_NO_FLOAT  // Disable float operations
#define REQUIRESNEW false     // true, включить для операторов new и delete
#define REQUIRESALARMS false  // true, функции поиска сигналов тревоги

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

//const char* ssid = "CKBEPHA"; const char* password = "6001018p";
const char* ssid = "CBEPX CEKPETHO"; const char* password = "555666777";
char auth[] = "Z1XXG3uwqCyYwmM2yiVBizpFfgXgVJ5t";   //  Test - newbany

#define CLK 5   //  D1         
#define DIO 4   //  D2         
#define RELE 15  //  D8  должен быть подтянут к минусу
#define KNOPA 2 //  D4            
#include "GyverTM1637.h"
GyverTM1637 disp(CLK, DIO);

#define DHTTYPE DHT11
#define DHTPIN 14                 //  D5
#define ONE_WIRE_BUS_ulisa 12     //  D6
#define ONE_WIRE_BUS_parilka 13   //  D7
#define ONE_WIRE_BUS_styachka  0 //  D3 

WidgetBridge bridge_to_MG(V0);
#define BLYNK_PIN_terminal_to_MG V0
#define BLYNK_PIN_klimatTmp_banya V2
#define BLYNK_PIN_Grafik V26
#define BLYNK_PIN_period_get_temp V17
#define BLYNK_PIN_regim_nasos_tPol_banya V30

#define BLYNK_PIN_kalorifer V39
#define BLYNK_PIN_t_banya V31   
#define BLYNK_PIN_t_ulisa V32   // a2
#define BLYNK_PIN_t_parilka V33 // a3 
#define BLYNK_PIN_t_styachka V34   // a1
#define BLYNK_PIN_h_banya V38
#define BLYNK_PIN_Gisterezis_and_on_kalorifer V10
#define BLYNK_PIN_reboot V100

unsigned long timer_FuncGrafik;
unsigned long lastConnectionAttempt;
unsigned long timer_get_temp;
uint32_t timer_1sek;
uint32_t timerTM;

uint32_t btnTimer;
uint8_t Disp_i;
bool flag_btn = false;
bool btnState;

bool flag_DispTM1637;

unsigned int period_Grafika = 20000;
unsigned int period_get_temp = 5000;

int klimatTmp_banya;
int t_banya;
int h_banya;
int t_ulisa;
int t_parilka;
int t_styachka;

int t_temp;
uint8_t t_temp_pli;

uint8_t DHT_error;
uint8_t t_ulisa_error;
uint8_t t_parilka_error; 
uint8_t t_styachka_error; 
uint8_t WiFi_Faled_Counter;
uint8_t ind_GetTemperature = -1;
uint8_t regim_nasos_tPol_banya;
uint8_t kalorifer; 
uint8_t kalorifer_old; 
uint8_t Gisterezis_and_on_kalorifer = 3;
float temp_izm;

bool reboot_sya = false;

byte arBania[] = { 0x7d, _A, _H, _A };
byte arPar[] = { 0x37, _A, _P , _empty};
byte arUlisa[] = { _U, _L, _l, _S };
byte arPol[] = { 0x37, _O, _L , _empty};

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire_styachka(ONE_WIRE_BUS_styachka);
DallasTemperature sensors_styachka(&oneWire_styachka);
OneWire oneWire_parilka(ONE_WIRE_BUS_parilka);
DallasTemperature sensors_parilka(&oneWire_parilka);
OneWire oneWire_ulisa(ONE_WIRE_BUS_ulisa);
DallasTemperature sensors_ulisa(&oneWire_ulisa);
WidgetRTC rtc;

BLYNK_CONNECTED() {
  rtc.begin();
  bridge_to_MG.setAuthToken("Ar_abuKKjGRIYF3ATQQgDE5Bovun61eN"); // 
  Blynk.syncAll();
  if ( millis() < 60000) bridge_to_MG.virtualWrite(BLYNK_PIN_terminal_to_MG, "  Баня start!\n");
  else Printterminal ("Баня reconnect..\n"); 
  //if ( millis() < 60000) Printterminal ("Баня start!\n");
  //else Printterminal ("Баня reconnect..\n"); bridge_to_MG.virtualWrite(BLYNK_PIN_terminal_to_MG, "Баня reconnect..\n");
}
BLYNK_WRITE(BLYNK_PIN_Grafik) { // 0-1 timer_FuncGrafik
  period_Grafika = param.asInt() * 1000;
  DispTM1637_Blynk( param.asInt() );
}
BLYNK_WRITE(BLYNK_PIN_period_get_temp) { // 
  period_get_temp = param.asInt() * 1000;
  DispTM1637_Blynk( param.asInt() );
}
BLYNK_WRITE(BLYNK_PIN_regim_nasos_tPol_banya) { // 0-1 
  regim_nasos_tPol_banya = param.asInt();
  DispTM1637_Blynk( param.asInt() );
}
BLYNK_WRITE(BLYNK_PIN_klimatTmp_banya) { //0-40
  klimatTmp_banya = param.asInt(); // String i = param.asStr(); double d = param.asDouble(); time_bub -=
  DispTM1637_Blynk( param.asInt() );
}
BLYNK_WRITE(BLYNK_PIN_Gisterezis_and_on_kalorifer) { //
  Gisterezis_and_on_kalorifer = param.asInt(); // 
	if ( Gisterezis_and_on_kalorifer < 0) {
		Gisterezis_and_on_kalorifer = 0;
		Blynk.virtualWrite(BLYNK_PIN_Gisterezis_and_on_kalorifer, 0); //
	}
  DispTM1637_Blynk( Gisterezis_and_on_kalorifer );
}
BLYNK_WRITE(BLYNK_PIN_reboot) { //
  reboot_sya = param.asInt();
}

void DispTM1637_Blynk( int n ) {
  disp.displayInt( n );
  flag_DispTM1637 = 1;
  timerTM = millis() - 400;
  t_temp = 67;
}
void Printterminal( String msgt ) {
  if ( Blynk.connected() ) {
    String tempDateTime = "";
    if (day() < 10) tempDateTime += "0" + String(day());
    else tempDateTime += String(day());
    tempDateTime += "/";
    if (month() < 10) tempDateTime += "0" + String(month());
    else tempDateTime += String(month());
    tempDateTime += " ";
    if (hour() < 10) tempDateTime += "0" + String(hour());
    else tempDateTime += String(hour());
    tempDateTime += ":";
    if (minute() < 10) tempDateTime += "0" + String(minute());
    else tempDateTime += String(minute());
    tempDateTime += " ";
    bridge_to_MG.virtualWrite(BLYNK_PIN_terminal_to_MG, tempDateTime );  // "DHT в бане - оff\n"
    bridge_to_MG.virtualWrite(BLYNK_PIN_terminal_to_MG, msgt);
    //if ( ln != default )  bridge_to_MG.virtualWrite(BLYNK_PIN_terminal_to_MG, "\n");
  }
}
void Fun_Grafik () {
  if ( Blynk.connected() ) {
    if ( !DHT_error ) bridge_to_MG.virtualWrite(BLYNK_PIN_t_banya, t_banya); //
    bridge_to_MG.virtualWrite(BLYNK_PIN_h_banya, h_banya); //
    if ( !t_styachka_error ) bridge_to_MG.virtualWrite(BLYNK_PIN_t_styachka, t_styachka); //
    if ( !t_parilka_error ) bridge_to_MG.virtualWrite(BLYNK_PIN_t_parilka, t_parilka); //
    if ( !t_ulisa_error ) bridge_to_MG.virtualWrite(BLYNK_PIN_t_ulisa, t_ulisa); //
    bridge_to_MG.virtualWrite(BLYNK_PIN_kalorifer, kalorifer); //
    timer_FuncGrafik = millis();
  }
}
void DispTM1637( uint8_t i ) { //   0-оП  1-оО  2-uП  3-uO
  disp.point(0);   // выкл точки
    flag_DispTM1637 = 0;
    t_temp_pli = 0; //
    if ( millis() - btnTimer < 10000 ) t_temp_pli = 1;
    else t_temp_pli = 0;  
  switch (i) { //  
		case 0:
      if ( !DHT_error ) {
        if ( t_temp != t_banya ) t_temp_pli = 1; // disp.displayInt( t_banya * 100 );
        t_temp = t_banya;
      } else {
        disp.displayByte(_E, _r, _r, _empty);
        return;
      }
      break;
    case 1:
      if ( !t_ulisa_error ) {
        if ( t_temp != t_parilka ) t_temp_pli = 1; //disp.displayInt( t_parilka * 100 );
        t_temp = t_parilka;
      } else {
        disp.displayByte(_E, _r, _r, _empty);
        return;
      }
      break;
    case 2:  
      if ( !t_parilka_error ) {
        if ( t_temp != t_ulisa ) t_temp_pli = 1; //disp.displayInt( t_ulisa * 100 );
        t_temp = t_ulisa;
      } else {
        disp.displayByte(_E, _r, _r, _empty);
        return;
      }
      break;
    case 3:
      if ( !t_styachka_error ) {
        if ( t_temp != t_styachka ) t_temp_pli = 1; //disp.displayInt( t_styachka * 100 );
        t_temp = t_styachka;
      } else {
        disp.displayByte(_E, _r, _r, _empty);
        return;
      }
      break;
    //default:
  }
  if ( t_temp_pli ) {
    disp.displayInt( t_temp * 100 );
    disp.displayByte( 2, 0x63); // grad
    disp.displayByte( 3, _C);
  }
}
void Fun_button () {
  bool btnState = !digitalRead(KNOPA);
  if (btnState && !flag_btn && millis() - btnTimer > 100) {
    flag_btn = true;
    btnTimer = millis();
    disp.clear();
    Disp_i++;
    if ( Disp_i > 3 ) Disp_i = 0;
   switch (Disp_i) {   
     case 0:
        disp.twistByte( arBania, 20); 
        break;
      case 1:
        disp.twistByte( arPar, 20); 
        break;
      case 2:  
        disp.twistByte( arUlisa, 20); 
        break;
      case 3:
        disp.twistByte( arPol, 20); 
        break;
      //default:
    }
    timerTM = millis();
    //flag_DispTM1637 = 1;
    //DispTM1637( Disp_i );
  }
  if (!btnState && flag_btn && millis() - btnTimer > 100) {
    flag_btn = false;
    btnTimer = millis();
    flag_DispTM1637 = 1;
  }
	
	if ( reboot_sya ) {
    Printterminal( "Котельная Д reboot.." );
    Blynk.virtualWrite(BLYNK_PIN_reboot, 0); 
		Blynk.disconnect();   delay(2000);    ESP.restart();
  }
	
}
void Fun_Kalorifer (){
  if ( regim_nasos_tPol_banya == 2 ) {
      if ( Gisterezis_and_on_kalorifer ) {
        if (t_banya != 99) {
          if ( t_banya <= klimatTmp_banya - Gisterezis_and_on_kalorifer) {
            kalorifer = 1;
          } else {
            if ( Gisterezis_and_on_kalorifer == 1 ) {
              if ( t_banya > klimatTmp_banya ) kalorifer = 0;
            } else {
              if ( t_banya >= klimatTmp_banya - 1 ) kalorifer = 0;
            }
          }
        } else {
          kalorifer = 0;
        }
      } else {
        kalorifer = 0;
      }
      if ( kalorifer_old != kalorifer ) {
        kalorifer_old = kalorifer;
        if ( Blynk.connected()) bridge_to_MG.virtualWrite(BLYNK_PIN_kalorifer, kalorifer);
      }
  } else {
    if ( regim_nasos_tPol_banya == 1 && Gisterezis_and_on_kalorifer ) kalorifer = regim_nasos_tPol_banya;
    else kalorifer = 0;
  }
    digitalWrite (RELE, (kalorifer) ? HIGH : LOW );
    timer_1sek = millis();
}
void GetTemperature() {
  float temp_izm;
  ind_GetTemperature++;
  if (ind_GetTemperature > 3) ind_GetTemperature = 0;
  //if (ind_GetTemperature > 1) ind_GetTemperature = 0;  //   только 2 датчика
  switch (ind_GetTemperature) { //
    case 0:
      temp_izm = dht.readTemperature(); // float 4B с плавающей точкой
      if (isnan(temp_izm) || temp_izm < 0) {
        if ( !DHT_error ) Printterminal ("DHT в бане - оff\n");
        DHT_error++;
      } else {
        t_banya = (int)round(temp_izm - 0.8);
        if ( DHT_error ) Printterminal ("DHT в бане - оn\n");
        DHT_error = 0;
      }
      temp_izm = dht.readHumidity();  // float 4B с плавающей точкой
      if ( !isnan(temp_izm) ) h_banya = (int)round(temp_izm);
      if (DHT_error > 10 ) {
        t_banya = 99;
        h_banya = 99;
      }
      break;
    case 1:
      //temp_izm = sensors_styachka.getTempC(ds_styachka); //   ИЗМЕРЕНИЕ
      //temp_izm = sensorsstyachka.getTempC(ds_ulisa); //   ИЗМЕРЕНИЕ
      sensors_ulisa.requestTemperatures();
      temp_izm = sensors_ulisa.getTempCByIndex(0);
      if (temp_izm == DEVICE_DISCONNECTED_C) {
        if ( !t_ulisa_error ) Printterminal ("t улица - оff\n");
        t_ulisa_error++;
        //if ( t_ulisa_error > 10 ) t_ulisa = 1;
      } else {
        if ( t_ulisa_error ) Printterminal ("t улица - оn\n");
        t_ulisa_error = 0;
        //if ( millis() < 60000) 
        t_ulisa = round(temp_izm);
        //else t_ulisa = round ( (t_ulisa * 0.7) + (temp_izm * 0.3) );
      }
      break;
    case 2:
      //temp_izm = sensorsstyachka.getTempC(ds_parilka); //   ИЗМЕРЕНИЕ
      sensors_parilka.requestTemperatures();
      temp_izm = sensors_parilka.getTempCByIndex(0);
      if (temp_izm == DEVICE_DISCONNECTED_C) {
        if ( !t_parilka_error ) Printterminal ("t парилки - оff\n");
        t_parilka_error++;
        //if ( t_parilka_error > 10 ) t_parilka = 1;
      } else {
        if ( t_parilka_error ) Printterminal ("t парилки оn\n");
        t_parilka_error = 0;
        //if ( millis() < 60000) 
        t_parilka = round(temp_izm);
        //else t_parilka = round ( (t_parilka * 0.7) + (temp_izm * 0.3) );
        break;
      }
      break;
    case 3: //  klimat 1
      sensors_styachka.requestTemperatures();
      temp_izm = sensors_styachka.getTempCByIndex(0);
      if (temp_izm == DEVICE_DISCONNECTED_C ) {
        if ( !t_styachka_error ) Printterminal ("t полов бани - оff\n");
        t_styachka_error++;
        //if ( t_styachka_error > 10 ) t_styachka = 1;
      } else {
        if ( t_styachka_error ) Printterminal ("t полов бани - оn\n");
        t_styachka_error = 0;
        //if ( millis() < 60000) 
        t_styachka = round(temp_izm);
        //else t_styachka = round ( (t_styachka * 0.7) + (temp_izm * 0.3) );
      }
  }
   timer_get_temp = millis();
   if ( !flag_btn ) flag_DispTM1637 = 1;
}
boolean connect_to_WiFi() {
	WiFi.begin(ssid, password);// Serial.print(F("Connecting to WI-FI... "));
	disp.point(0);   // выкл точки
	for (int i = 0; i < 50; i++) {
		if ( WiFi.status() == WL_CONNECTED ) return true;
		disp.displayInt( i );
    disp.twist(0, 0, 40); //delay(1);
    if ( !digitalRead(KNOPA)) return false;
	}
	WiFi_Faled_Counter++;
	if (WiFi_Faled_Counter > 10) ESP.restart();
	lastConnectionAttempt = millis();
	return false;
}
void setup() {
	//pinMode(BUILTIN_LED, OUTPUT); digitalWrite ( RELE_EKOTEL_PIN ,  HIGH); // HIGH;LOW  (rele_eKotel == 1) ? HIGH : LOW;		//sensorsstyachka.setResolution(9); // установите глобальное разрешение на 9, 10, 11 или 12 бит
	pinMode(KNOPA, INPUT_PULLUP);
  pinMode(RELE, OUTPUT);
  digitalWrite (RELE, LOW);
  //attachInterrupt(digitalPinToInterrupt(KNOPA), Knopa_state, FALLING );// CHANGE
	sensors_ulisa.begin();
  sensors_parilka.begin();
  sensors_styachka.begin();
  dht.begin();
  //disp.clear();
  disp.brightness(6);  // яркость, 0 - 7 (минимум - максимум)
	if ( connect_to_WiFi() ) Blynk.begin(auth, ssid, password);
	else Blynk.config(auth);
  disp.displayByte(_empty, _empty, _empty, _empty);
  setSyncInterval(10 * 60); // Интервал синхронизации в секундах (10 minutes) <WidgetRTC.h>
	GetTemperature();
  flag_DispTM1637 = 0;
}
void loop() {
	if ( millis() - timer_get_temp > period_get_temp / 4 ) GetTemperature();
  if ( flag_DispTM1637 && millis() - timerTM > 800 ) DispTM1637( Disp_i );
  if ( millis() - timer_1sek > 1500 ) Fun_Kalorifer ();
  Fun_button ();
	if (WiFi.status() != WL_CONNECTED) {  //  "offline"  code
		if ( millis() - lastConnectionAttempt > 300000) connect_to_WiFi(); // переподключение через 30 сек
	} else {
		Blynk.run();  // yield(); now = millis(); if ( Blynk.connected() )
		Fun_button ();
		WiFi_Faled_Counter = 0;
		if ( period_Grafika && millis() - timer_FuncGrafik > period_Grafika ) Fun_Grafik();
	}
}
