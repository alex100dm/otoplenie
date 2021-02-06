//#define BLYNK_PRINT Serial
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
#include <ElegantOTA.h>
//const char* ssid = "CKBEPHA"; const char* password = "6001018p";
const char* ssid = "CBEPX CEKPETHO"; const char* password = "555666777";
char auth[] = "Ar_abuKKjGRIYF3ATQQgDE5Bovun61eN";  // MG kotel

#define BLYNK_RED       "#D3435C"
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_YELLOW    "#ED9D00"
//#define BLYNK_serui   "#e7e7e7"

#define BUILTIN_LED 2                 // d4 
#define DHTTYPE DHT11
#define DHTPIN 5 // d1   ESP8266 примечание: используйте контакты 3, 4, 5, 12, 13 или 14. 
#define ONE_WIRE_BUS 4 // d2 шина 1-WIRE с датчиками ds18b20
#define ONE_WIRE_BUS_D7 13        // d7 

#define RELE_EKOTEL_PIN 14            // d5 
#define RELE_NASOS_EKOTEL_PIN 12      // d6 
#define RELE_NASOS_TPOL_PIN 16        // d0 
#define RELE_NASOS_TPOL_BAYUA_PIN 15  // d8
#define RELE_2_stupen_PIN 0           // d3 управление второй ступенью

WidgetLED LED_BLYNK_eKotel(V10);
WidgetLED LED_BLYNK_nasos_eKotel(V11);
WidgetLED LED_BLYNK_2_stupen(V12); // d3 управление второй ступенью
WidgetLED LED_BLYNK_nasos_tPol(V13);
WidgetLED LED_BLYNK_nasos_tPol_banya(V14);
WidgetLED LED_BLYNK_drovyanoi_kotel(V41);

#define BLYNK_PIN_teminal V0
#define BLYNK_PIN_klimatTmp V1
#define BLYNK_PIN_klimatTmp_banya V2

#define BLYNK_PIN_t_banya V31   // a1
#define BLYNK_PIN_h_banya V38
#define BLYNK_PIN_t_ulisa V32   // a2
#define BLYNK_PIN_t_parilka V33   // a3 запасной датчик
#define BLYNK_PIN_t_styachka V34   // a1
#define BLYNK_PIN_t_dom V5
#define BLYNK_PIN_h_dom V6
#define BLYNK_PIN_tempOutEKotla V7
#define BLYNK_PIN_Obratka V8

#define BLYNK_PIN_regim_eKotel V3         // 1 2 3
#define BLYNK_PIN_regim_2_stupen V4 // d3 управление второй ступенью
#define BLYNK_PIN_regim_nasos_tPol V9
#define BLYNK_PIN_regim_nasos_tPol_banya V30

#define BLYNK_PIN_rele_eKotel V20
#define BLYNK_PIN_rele_nasos_eKotel V21
#define BLYNK_PIN_rele_2_stupen V22 // d3 управление второй ступенью
#define BLYNK_PIN_rele_nasos_tPol V23
#define BLYNK_PIN_rele_nasos_tPol_banya V24

#define BLYNK_PIN_klimatGisterezis V15
#define BLYNK_PIN_tempObratka_valueOtkl V16
#define BLYNK_PIN_nasos_eKotel_timeout V18
#define BLYNK_PIN_t_out_to_off V19
#define BLYNK_PIN_Tmp_banya_ot_zamerpaniya V25
#define BLYNK_PIN_Grafik V26
#define BLYNK_PIN_period_oprosa_dat4ikov V17
#define BLYNK_PIN_Gisterezis_2_stupen V27
#define BLYNK_PIN_Gisterezis_temp_Otkl V28
#define BLYNK_PIN_Gisterezis_Max_tempOutEKotla_off_2_stupen V35
#define BLYNK_PIN_Max_tempOutEKotla_off_2_stupen V36
WidgetLCD BLYNK_lcd(V37);
#define BLYNK_PIN_kalorifer V39
WidgetLED LED_BLYNK_kalorifer(V40);
#define BLYNK_PIN_t_grebenka V43
//#define BLYNK_PIN_ V43

#define BLYNK_PIN_temp_out_d_kotel V50
#define BLYNK_PIN_temp_in_d_kotel V51
#define BLYNK_PIN_temp_to_house V52
#define BLYNK_PIN_temp_iz_house V53


unsigned long now1; //
unsigned long timer_1sek = 30000;//
unsigned long lastConnectionAttempt;
unsigned long timer_FuncGrafik;
unsigned long timer_rele_nasos_eKotel;
unsigned long timer_oprosa_dat4ikov;
uint32_t timer_t_banya;
int h_dom;
int h_dom_old;
int t_dom;
int t_dom_old = 5;
int t_banya = 5;
int h_banya;
int t_ulisa;
int t_parilka; //
int t_styachka; 
int t_grebenka; // 
int t_grebenka_old; // 

int tempOutEKotla;
int tempOutEKotla_old;
int tempObratka;
int tempObratka_old;

int DHT_error;     //
int tempObratka_error;     //
int tempOutEKotla_error;     //
int t_grebenka_error;     //
int temp_out_d_kotel;
int temp_in_d_kotel;
int temp_to_house;
int temp_iz_house;
//int t_banya_error = 0;     //
//int t_ulisa_error = 0;     //
//int t_parilka_error = 0;     //
unsigned int period_Grafika = 20000;     //
int rele_eKotel = 0;
int rele_eKotel_old = 0;
int regim_eKotel = 2;     //  3 1 2
int regim_eKotel_old = 2;
int rele_nasos_eKotel = 0;
int rele_nasos_eKotel_old = 0;
int regim_nasos_eKotel = 2;    // 1 2
int rele_2_stupen = 0;
int rele_2_stupen_old = 0;
int var_rele_2_stupen = 0;
int regim_2_stupen = 2;   // 0 1
//int regim_nasos_boiler_old = 2;// 0 1
int rele_nasos_tPol = 0;
int rele_nasos_tPol_old = 0;
int regim_nasos_tPol = 3;     // 0 1 2
int rele_nasos_tPol_banya = 0;
int rele_nasos_tPol_banya_old = 0;
int regim_nasos_tPol_banya = 3;     // 0 1 2
int min_max_2_stupen = 0;
int klimatTmp = 10;      // -10 +50
int klimatTmp_banya = 10;      // -10 +50

int klimatGisterezis = 1; // в градусах выключение Т дома + гистерезис
int Gisterezis_2_stupen = 2; // в градусах выключение Т дома + гистерезис
int tempObratka_valueOtkl = 30;      // 30 град
int tempPodacha_Kotel_off = 85;      // 85 град
int Tmp_banya_ot_zamerpaniya;     //
int period_oprosa_dat4ikov = 20000;
int Gisterezis_temp_Otkl = 5;
int Gisterezis_Max_tempOutEKotla_off_2_stupen = 10;
int Max_tempOutEKotla_off_2_stupen = tempPodacha_Kotel_off;
int WiFi_Faled_Counter;
int ind_GetTemperature = -1;
unsigned int nasos_eKotel_timeout = 300000;     // 40 sek
boolean BUILTIN_LED_State = 1;
boolean Ota_kotel = false;
boolean rabota_ot_elektr = true;
//boolean rabota_ot_elektr = true;
boolean zach4ita_ot_zamerz = false;
boolean zach4ita_ot_peregreva_el_kotla = false;
uint8_t lcd_i = 1;
uint8_t kalorifer;

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
OneWire oneWire_D7(ONE_WIRE_BUS_D7);
DallasTemperature sensors2(&oneWire_D7);

DeviceAddress dsKotel = {  0x28, 0xE6, 0x19, 0x79, 0xA2, 0x19, 0x03, 0x34  };  // dds18b20 #5
DeviceAddress dsObratka = {  0x28, 0x75, 0x22, 0x79, 0xA2, 0x16, 0x03, 0x30  }; // dds18b20 #6
//DeviceAddress ds_banya = {  0x28, 0xF6, 0xDB, 0xB8, 0x1E, 0x19, 0x01, 0xC3  };  // a1
//DeviceAddress ds_banya  = {  0x28, 0x33, 0xD0, 0xD8, 0x1E, 0x19, 0x01, 0x57  }; // a4
//DeviceAddress ds_parilka = {  0x28, 0x26, 0x5E, 0xE1, 0x1E, 0x19, 0x01, 0x55  }; // a2
//DeviceAddress ds_ulisa = {  0x28, 0x3B, 0x36, 0xC0, 0x1E, 0x19, 0x01, 0x94  }; // a3

WidgetTerminal terminal(BLYNK_PIN_teminal);
WidgetRTC rtc;
ESP8266WebServer server(80);

BLYNK_CONNECTED() {
  rtc.begin(); //
  if ( millis() < 60000) {
    Blynk.syncAll();
		/*Blynk.syncVirtual(
		BLYNK_PIN_klimatTmp,
		BLYNK_PIN_klimatTmp_banya,
		BLYNK_PIN_regim_eKotel,
		BLYNK_PIN_regim_nasos_eKotel,
		BLYNK_PIN_regim_nasos_tPol,
		BLYNK_PIN_regim_nasos_tPol_banya,
		BLYNK_PIN_regim_2_stupen,
		BLYNK_PIN_tempObratka_valueOtkl,
		BLYNK_PIN_Tmp_banya_ot_zamerpaniya,
		BLYNK_PIN_Gisterezis_2_stupen,
		BLYNK_PIN_Gisterezis_temp_Otkl,
		BLYNK_PIN_Gisterezis_Max_tempOutEKotla_off_2_stupen,
		BLYNK_PIN_Max_tempOutEKotla_off_2_stupen,
		BLYNK_PIN_t_banya, BLYNK_PIN_t_ulisa, BLYNK_PIN_t_parilka
		);*/
		terminal.println( "	Dom start!" ); //terminal.clear();	Printterminal( "reconnect.." );Printterminal( "start" );
    terminal.flush();
  } else {
		Printterminal( "Dom reconnect..\n" );
  }
}
BLYNK_APP_CONNECTED() {
  Printterminal("app on\n");
}
BLYNK_APP_DISCONNECTED() {
	Printterminal("app off\n");
}
BLYNK_WRITE(BLYNK_PIN_teminal) {
  String str = param.asStr();
  if ( String("reboot") == str ) {
    Printterminal("Перезагрузка...\n");
    ESP.restart();
  } else if ( String("ota") == str ) {
    Ota_kotel = true;
    Printterminal("Вкл OTA\n");
  }
}
BLYNK_WRITE(BLYNK_PIN_klimatTmp) { //0-40
  klimatTmp = param.asInt(); // String i = param.asStr(); double d = param.asDouble(); time_bub -=
}
BLYNK_WRITE(BLYNK_PIN_klimatTmp_banya) { //0-40
  klimatTmp_banya = param.asInt(); // String i = param.asStr(); double d = param.asDouble(); time_bub -=
}
BLYNK_WRITE(BLYNK_PIN_regim_eKotel) { // 3  1  2
  regim_eKotel = param.asInt(); // assigning incoming value from pin V1 to a variable
  if ( regim_eKotel == 2 ) regim_nasos_tPol = 2;
}
BLYNK_WRITE(BLYNK_PIN_regim_nasos_eKotel) { // 1 2
  regim_nasos_eKotel = param.asInt(); // assigning incoming value from pin V1 to a variable
}
BLYNK_WRITE(BLYNK_PIN_regim_2_stupen) { // 0  1
  regim_2_stupen = param.asInt(); // assigning incoming value from pin V1 to a variable
}
BLYNK_WRITE(BLYNK_PIN_regim_nasos_tPol) { // 3  1  2
  regim_nasos_tPol = param.asInt(); // assigning incoming value from pin V1 to a variable
  //if ( regim_nasos_tPol == 1 ) regim_eKotel = 3;
}
BLYNK_WRITE(BLYNK_PIN_regim_nasos_tPol_banya) { // 3  1  2
  regim_nasos_tPol_banya = param.asInt(); // assigning incoming value from pin V1 to a variable
}
BLYNK_WRITE(BLYNK_PIN_klimatGisterezis) { // 1-5
  klimatGisterezis = param.asInt(); // assigning incoming value from pin V1 to a variable
}
BLYNK_WRITE(BLYNK_PIN_tempObratka_valueOtkl) { // 0-90
  tempObratka_valueOtkl = param.asInt(); // assigning incoming value from pin V1 to a variable
}
BLYNK_WRITE(BLYNK_PIN_nasos_eKotel_timeout) { // 0-180
  nasos_eKotel_timeout = param.asInt() * 1000; //
}
BLYNK_WRITE(BLYNK_PIN_t_out_to_off) { // 0-1
  tempPodacha_Kotel_off = param.asInt(); //
}
BLYNK_WRITE(BLYNK_PIN_Grafik) { // 0-1 timer_FuncGrafik
  period_Grafika = param.asInt() * 1000;
}
BLYNK_WRITE(BLYNK_PIN_Tmp_banya_ot_zamerpaniya) { //3
  Tmp_banya_ot_zamerpaniya = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_Gisterezis_2_stupen) { //
  Gisterezis_2_stupen = param.asInt();
	if ( Gisterezis_2_stupen < 0) {
		Gisterezis_2_stupen = 0;
		Blynk.virtualWrite(BLYNK_PIN_Gisterezis_2_stupen, 0); //
	}
}
BLYNK_WRITE(BLYNK_PIN_period_oprosa_dat4ikov) { //
  period_oprosa_dat4ikov = param.asInt() * 1000;
}
BLYNK_WRITE(BLYNK_PIN_Gisterezis_temp_Otkl) { //
  Gisterezis_temp_Otkl = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_Gisterezis_Max_tempOutEKotla_off_2_stupen) { //
  Gisterezis_Max_tempOutEKotla_off_2_stupen = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_Max_tempOutEKotla_off_2_stupen) { //
  Max_tempOutEKotla_off_2_stupen = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_t_banya) { //  V31   // a1
  t_banya = param.asInt();
	timer_t_banya = millis();
}
BLYNK_WRITE(BLYNK_PIN_t_ulisa) { //  V32   // a2
  t_ulisa = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_t_parilka) { // V33   // a3
  t_parilka = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_t_styachka) { // V33   // a3
  t_styachka = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_temp_out_d_kotel) { // выход дровяного котла
  temp_out_d_kotel = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_temp_in_d_kotel) { //  вход дровяного
  temp_in_d_kotel = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_temp_to_house) { //  от дровяного в дом
  temp_to_house = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_temp_iz_house) { //  обратка из дома
  temp_iz_house = param.asInt();
}
BLYNK_WRITE(BLYNK_PIN_kalorifer) { // 
  kalorifer = param.asInt();
}
//BLYNK_READ(V37) {  // LCD температура BLYNK_lcd }
void Fun_BLYNK_lcd( uint8_t n, uint8_t data) {
	switch (data) { //BLYNK_lcd.clear(); BLYNK_lcd.print(4,0,""); // 0-15 0-1
    case 1:
			BLYNK_lcd.print(0, n ,"п   "); // 0-15 0-1
			BLYNK_lcd.print(1, n , temp_out_d_kotel ); // 0-15 0-1
			BLYNK_lcd.print(3, n ," о  "); // 0-15 0-1
      BLYNK_lcd.print(5, n , temp_in_d_kotel ); // 0-15 0-1
			BLYNK_lcd.print(7, n ," П   "); // 0-15 0-1
      BLYNK_lcd.print(9, n , temp_to_house ); // 0-15 0-1
      BLYNK_lcd.print(11, n ," О   "); // 0-15 0-1
      BLYNK_lcd.print(13, n , temp_iz_house ); // 0-15 0-1
			break;
		case 2:
			BLYNK_lcd.print(0, n ,"Б:   "); // 
			BLYNK_lcd.print(2, n , t_banya ); //
			BLYNK_lcd.print(5, n ,"пар:   "); // 
			BLYNK_lcd.print(9, n , t_parilka ); //
			BLYNK_lcd.print(12, n ,"У:  "); // 
			BLYNK_lcd.print(14, n , t_ulisa ); //
			break;
		case 3:
			BLYNK_lcd.print(0, n ,"Д:   "); // 
			BLYNK_lcd.print(2, n , t_dom ); //
			BLYNK_lcd.print(5, n ,"Пэ:   "); // 
			BLYNK_lcd.print(8, n , tempOutEKotla ); //
			BLYNK_lcd.print(11, n ,"Оэ:  "); // 
			BLYNK_lcd.print(14, n , tempObratka ); //
			break;
		case 4:
			BLYNK_lcd.print(0, n ,"Гребён:   "); // 
			BLYNK_lcd.print(7, n , t_grebenka ); //
			BLYNK_lcd.print(9, n ,"Бпол:  "); // 
			BLYNK_lcd.print(14, n , t_styachka ); //
			break;
	}
}
void Printterminal( String msgt) {
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
    terminal.print(tempDateTime); //terminal.clear();
    terminal.print(msgt); //terminal.clear(); terminal.println(msgt);
    terminal.flush();
  }
}
void GetTemperature() {
  float temp_izm;
  ind_GetTemperature++;
  if (ind_GetTemperature > 2) ind_GetTemperature = 0;
  switch (ind_GetTemperature) { //
    case 0:
      temp_izm = dht.readTemperature(); // float 4B с плавающей точкой
      if (isnan(temp_izm) || temp_izm < 0) {
        if ( !DHT_error ) Printterminal ("DHT Дом - оff\n");
				DHT_error++;
      } else {
        if ( DHT_error ) {
					Printterminal ("DHT Дом - оn\n");
					DHT_error = 0;
				}
				t_dom = (int)round(temp_izm - 0.9);
      }
      temp_izm = dht.readHumidity();  // float 4B с плавающей точкой
      if ( !isnan(temp_izm)) h_dom = (int)round(temp_izm);
      break;
			
    case 1:
      sensors.requestTemperatures();
			temp_izm = sensors.getTempC(dsKotel); //  ИЗМЕРЕНИ ПОДАЧИ
      if (temp_izm == DEVICE_DISCONNECTED_C) {
        if ( !tempOutEKotla_error ) Printterminal("dt подачи - оff\n");
        tempOutEKotla_error++;
      } else {
        if ( tempOutEKotla_error ) {
					Printterminal("dt подачи - оn\n");
					tempOutEKotla_error = 0;
				}
				tempOutEKotla = round ( temp_izm );
				//tempOutEKotla = round ( (tempOutEKotla * 0.7) + (temp_izm * 0.3) );
      }

      temp_izm = sensors.getTempC(dsObratka); //   ИЗМЕРЕНИЕ ОБРАТКИ
      if (temp_izm == DEVICE_DISCONNECTED_C) {
        if ( !tempObratka_error  ) Printterminal("dt обратки - оff\n");
        tempObratka_error++;
      } else {
        if ( tempObratka_error  ) Printterminal("dt обратки - оn\n");
        tempObratka_error = 0;
        tempObratka = round ( temp_izm );
      }
      break;

    case 2: //  
      sensors2.requestTemperatures();
			temp_izm = sensors2.getTempCByIndex(0);
      if (temp_izm == DEVICE_DISCONNECTED_C ) {
        if ( !t_grebenka_error ) Printterminal("dt гребенки - оff\n");
        t_grebenka_error++;
      } else {
        if ( t_grebenka_error ) Printterminal("dt гребенки - оn\n");
        t_grebenka_error = 0;
        t_grebenka = round(temp_izm);
      }
      break;
  }
   timer_oprosa_dat4ikov = millis();
}
void FuncGrafik() {
	
		if ( !DHT_error ){
		  Blynk.virtualWrite(BLYNK_PIN_h_dom, h_dom); //
      Blynk.virtualWrite(BLYNK_PIN_t_dom, t_dom);
		}
		if ( !tempOutEKotla_error ) Blynk.virtualWrite(BLYNK_PIN_tempOutEKotla, tempOutEKotla);
		if ( !tempObratka_error ) Blynk.virtualWrite(BLYNK_PIN_Obratka, tempObratka);
		if ( !t_grebenka_error ) Blynk.virtualWrite(BLYNK_PIN_t_grebenka, t_grebenka); //
		
		Blynk.virtualWrite(BLYNK_PIN_rele_eKotel, rele_eKotel); //
		Blynk.virtualWrite(BLYNK_PIN_rele_nasos_eKotel, rele_nasos_eKotel); //
		Blynk.virtualWrite(BLYNK_PIN_rele_2_stupen, rele_2_stupen); //
		Blynk.virtualWrite(BLYNK_PIN_rele_nasos_tPol, rele_nasos_tPol); //
		Blynk.virtualWrite(BLYNK_PIN_rele_nasos_tPol_banya, rele_nasos_tPol_banya); //
		
		/*
		if ( rabota_ot_elektr ) {
			Blynk.setProperty( BLYNK_PIN_regim_eKotel , "color", BLYNK_YELLOW);//"label", "My New Widget Label"
			Blynk.setProperty( BLYNK_PIN_t_dom , "color", BLYNK_YELLOW);//"label", "My New Widget Label"
			Blynk.setProperty( BLYNK_PIN_klimatTmp , "color", BLYNK_YELLOW);//"label", "My New Widget Label"
			Blynk.setProperty( BLYNK_PIN_regim_nasos_tPol_banya , "color", BLYNK_YELLOW); // BLYNK_GREEN
			Blynk.setProperty( BLYNK_PIN_t_banya , "color", BLYNK_YELLOW);
			Blynk.setProperty( BLYNK_PIN_klimatTmp_banya , "color", BLYNK_YELLOW); // BLYNK_serui
		} else {
			Blynk.setProperty( BLYNK_PIN_regim_eKotel , "color", BLYNK_RED);//"label", "My New Widget Label"
			Blynk.setProperty( BLYNK_PIN_t_dom , "color", BLYNK_RED);//"label", "My New Widget Label"
			Blynk.setProperty( BLYNK_PIN_klimatTmp , "color", BLYNK_RED);//"label", "My New Widget Label"
			Blynk.setProperty( BLYNK_PIN_regim_nasos_tPol_banya , "color", BLYNK_RED); // BLYNK_GREEN
			Blynk.setProperty( BLYNK_PIN_t_banya , "color", BLYNK_RED);
			Blynk.setProperty( BLYNK_PIN_klimatTmp_banya , "color", BLYNK_RED); // BLYNK_serui
		}*/
		timer_FuncGrafik = millis();

}
void Fun_obnov_to_Blynk () {
	//======================== ОТПРАВКА ИМЕНЕНИЙ НА СЕРВЕР  ===================================
	if (h_dom != h_dom_old) {
		h_dom_old = h_dom;
		Blynk.virtualWrite(BLYNK_PIN_h_dom, h_dom); //
	}
	if (t_dom != t_dom_old) {
		t_dom_old = t_dom;
		Blynk.virtualWrite(BLYNK_PIN_t_dom, t_dom);
	}
	if (tempOutEKotla != tempOutEKotla_old) {
		tempOutEKotla_old = tempOutEKotla;
		Blynk.virtualWrite(BLYNK_PIN_tempOutEKotla, tempOutEKotla);
	}
	if (tempObratka != tempObratka_old) {
		tempObratka_old = tempObratka;
		Blynk.virtualWrite(BLYNK_PIN_Obratka, tempObratka);
	}
	if (rele_eKotel_old != rele_eKotel) {
		rele_eKotel_old = rele_eKotel;
		Blynk.virtualWrite(BLYNK_PIN_rele_eKotel, rele_eKotel); //
	}
	if (rele_nasos_tPol_banya_old != rele_nasos_tPol_banya) {
		rele_nasos_tPol_banya_old = rele_nasos_tPol_banya;
		Blynk.virtualWrite(BLYNK_PIN_rele_nasos_tPol_banya, rele_nasos_tPol_banya); //
	}
	if (rele_nasos_eKotel_old != rele_nasos_eKotel) {
		rele_nasos_eKotel_old = rele_nasos_eKotel;
		Blynk.virtualWrite(BLYNK_PIN_rele_nasos_eKotel, rele_nasos_eKotel); //
	}
	if (rele_2_stupen_old != rele_2_stupen) {
		rele_2_stupen_old = rele_2_stupen;
		Blynk.virtualWrite(BLYNK_PIN_rele_2_stupen, rele_2_stupen); //
	}
	if (rele_nasos_tPol_old != rele_nasos_tPol) {
		rele_nasos_tPol_old = rele_nasos_tPol;
		Blynk.virtualWrite(BLYNK_PIN_rele_nasos_tPol, rele_nasos_tPol); //
	}
  if (t_grebenka_old != t_grebenka) {
    t_grebenka_old = t_grebenka;
    Blynk.virtualWrite(BLYNK_PIN_t_grebenka, t_grebenka); //
  }
  
	if ( rele_nasos_tPol_banya ) LED_BLYNK_nasos_tPol_banya.on();
	else LED_BLYNK_nasos_tPol_banya.off();
	if ( rele_nasos_eKotel ) LED_BLYNK_nasos_eKotel.on();
	else LED_BLYNK_nasos_eKotel.off();
	if ( rele_2_stupen ) LED_BLYNK_2_stupen.on();
	else LED_BLYNK_2_stupen.off();
	if ( rele_nasos_tPol ) LED_BLYNK_nasos_tPol.on();
	else LED_BLYNK_nasos_tPol.off();
	
	if ( rele_eKotel ) {
    if ( rele_2_stupen ) Blynk.setProperty( V10 , "color", BLYNK_YELLOW);//"label", "My New Widget Label"  BLYNK_YELLOW   BLYNK_RED
    else Blynk.setProperty( V10 , "color", BLYNK_GREEN);
	  LED_BLYNK_eKotel.on();
	}	else {
	  LED_BLYNK_eKotel.off();
	}
  
	if ( kalorifer ) LED_BLYNK_kalorifer.on();// WidgetLED LED_BLYNK_kalorifer(V40);
	else LED_BLYNK_kalorifer.off();

	if ( rabota_ot_elektr ) {
    LED_BLYNK_drovyanoi_kotel.off();
	}	else {
    if ( temp_out_d_kotel > 60 ) Blynk.setProperty( V41 , "color", BLYNK_YELLOW);//WidgetLED LED_BLYNK_drovyanoi_kotel(V41);
    else Blynk.setProperty( V41 , "color", BLYNK_GREEN);
    LED_BLYNK_drovyanoi_kotel.on();
	}
}
void Func1sek() {
	//======================== КЛИМАТ КОНТРОЛЬ ===================================
	BUILTIN_LED_State = !BUILTIN_LED_State;

	if ( tempObratka < tempObratka_valueOtkl ) {
		rabota_ot_elektr = true;
	} else {
		if ( tempObratka > tempObratka_valueOtkl + Gisterezis_temp_Otkl ) rabota_ot_elektr = false;
	}

	if ( rabota_ot_elektr ) { //   rabota_ot_elektr
		switch (regim_eKotel) { //   3 1 2
			case 3:
				digitalWrite ( BUILTIN_LED , HIGH );
				if (rele_eKotel) timer_rele_nasos_eKotel = millis();
				rele_eKotel = 0;
				break;
			case 1:
				digitalWrite ( BUILTIN_LED , LOW );
				rele_eKotel = 1;
				break;
			case 2: //  klimat 1
				if (BUILTIN_LED_State) digitalWrite ( BUILTIN_LED , HIGH );
				else digitalWrite ( BUILTIN_LED , LOW );
				if ( t_dom < klimatTmp ) { // || rele_nasos_tPol_banya
					rele_eKotel = 1;
					rele_nasos_eKotel = 1;
				} else {
					if ( t_dom >= (klimatTmp + klimatGisterezis) ) {
						if (rele_eKotel) timer_rele_nasos_eKotel = millis();
						rele_eKotel = 0;
					}
				}
		}
		if (regim_nasos_eKotel == 1) {  //  1 2
			rele_nasos_eKotel = regim_nasos_eKotel;
		} else {
			if ( rele_eKotel == 1 ) {
				rele_nasos_eKotel = 1;
			} else {
				if (rele_nasos_eKotel) {
					if ( millis() - timer_rele_nasos_eKotel > nasos_eKotel_timeout ) rele_nasos_eKotel = 0;
				}
			}
		}
	} else {   //   rabota_ot drova
		if (rele_eKotel) timer_rele_nasos_eKotel = millis();
		rele_eKotel = 0;
		if (millis() - timer_rele_nasos_eKotel > nasos_eKotel_timeout ) rele_nasos_eKotel = 0;
	}
	
	switch (regim_nasos_tPol) {    // 1 2 3
		case 3:
			rele_nasos_tPol = 0;
			break;
		case 1:
			rele_nasos_tPol = 1;
			break;
		case 2:
			if ( regim_eKotel == 2 ) {
				if ( t_dom < klimatTmp ) {
					rele_nasos_tPol = 1;
				} else {
					if ( t_dom >= (klimatTmp + klimatGisterezis) ) rele_nasos_tPol = 0;
				}
			} else {
				rele_nasos_tPol = 0;
			}
	}
	
	switch (regim_nasos_tPol_banya) {    // 1 2 3
		case 3:
			rele_nasos_tPol_banya = 0;
			break;
		case 1:
			rele_nasos_tPol_banya = 1;
			break;
		case 2:
			if ( t_banya < klimatTmp_banya ) {
				rele_nasos_tPol_banya = 1;
				if ( rabota_ot_elektr ) {
					rele_eKotel = 1;
					rele_nasos_eKotel = 1;
				}
			} else {
				if ( t_banya >= (klimatTmp_banya + klimatGisterezis) ) rele_nasos_tPol_banya = 0;
			}
	}
	
	if ( rabota_ot_elektr && Gisterezis_2_stupen) {   // управление второй ступенью 
			switch (regim_2_stupen) {    // 
			case 3:
				rele_2_stupen = 0;
				break;
			case 1:
				rele_2_stupen = 1;
				break;
			case 2:
				if ( tempOutEKotla >= Max_tempOutEKotla_off_2_stupen ) {
					min_max_2_stupen = 0;
				} else {
					if ( tempOutEKotla <= Max_tempOutEKotla_off_2_stupen - Gisterezis_Max_tempOutEKotla_off_2_stupen ) {
						min_max_2_stupen = 1;
					}
				}

				if ( klimatTmp - t_dom >= Gisterezis_2_stupen || klimatTmp_banya - t_banya >= Gisterezis_2_stupen ) {
					var_rele_2_stupen = 1;
				} else {
					if ( Gisterezis_2_stupen == 1 ) {
						if ( klimatTmp <= t_dom + Gisterezis_2_stupen ) var_rele_2_stupen = 0;
					} else {
						if ( klimatTmp <= t_dom ) var_rele_2_stupen = 0;
					}
				}

				if (rele_eKotel) {
					if ( min_max_2_stupen ) rele_2_stupen = var_rele_2_stupen;
					else rele_2_stupen = min_max_2_stupen;
				} else {
					rele_2_stupen = 0;
				}
			}
	}
	
	if ( t_banya <= Tmp_banya_ot_zamerpaniya && millis() > 40000) { // защита от замерзания
		rele_nasos_tPol_banya = 1;
		if ( rabota_ot_elektr ) { //
			rele_eKotel = 1;
			rele_nasos_eKotel = 1;
		}
		if ( !zach4ita_ot_zamerz ) Printterminal( "Эл.котёл ON принудительно - защита от замерзания бани\n" );
		zach4ita_ot_zamerz = 1;
	} else {
		if ( zach4ita_ot_zamerz ) Printterminal( "защита от замерзания бани сделала свое дело\n" );
		zach4ita_ot_zamerz = 0;
	}
	
	if ( tempOutEKotla > tempPodacha_Kotel_off ) { // защита от перегрева котла
		if (rele_eKotel) timer_rele_nasos_eKotel = millis();
		rele_nasos_eKotel = 1;
		rele_eKotel = 0;
		if ( !zach4ita_ot_peregreva_el_kotla ) Printterminal( "Эл.котёл OFF - защита от перегрева\n" ); // if ( 
		zach4ita_ot_peregreva_el_kotla = 1;
	} else {
		if ( zach4ita_ot_peregreva_el_kotla ) {
			Printterminal( (String)"перегрев котла устранён! длилось " + round((millis()-timer_rele_nasos_eKotel)/1000) + " сек\n" );
			zach4ita_ot_peregreva_el_kotla = 0;
		}
	}
	
	//======================== ОБНОВЛЕНИЕ РЕЛЕ ===================================
  if (rele_eKotel) digitalWrite ( RELE_EKOTEL_PIN ,  LOW);
  else digitalWrite ( RELE_EKOTEL_PIN , HIGH);
  if (rele_nasos_eKotel) digitalWrite ( RELE_NASOS_EKOTEL_PIN ,  LOW);
  else digitalWrite ( RELE_NASOS_EKOTEL_PIN , HIGH);
  if (rele_nasos_tPol) digitalWrite ( RELE_NASOS_TPOL_PIN ,  LOW);
  else digitalWrite ( RELE_NASOS_TPOL_PIN , HIGH);
  if (rele_2_stupen) digitalWrite ( RELE_2_stupen_PIN ,  LOW);
  else digitalWrite ( RELE_2_stupen_PIN , HIGH);
  if (rele_nasos_tPol_banya) digitalWrite ( RELE_NASOS_TPOL_BAYUA_PIN ,  LOW);
  else digitalWrite ( RELE_NASOS_TPOL_BAYUA_PIN , HIGH);
  
	if ( BUILTIN_LED_State && Blynk.connected() ) {
		Fun_obnov_to_Blynk ();
		lcd_i++;
		if (lcd_i > 2) lcd_i = 0; 
		switch (lcd_i) { //
			case 0:
					Fun_BLYNK_lcd( 0, 3 );
					Fun_BLYNK_lcd( 1, 2 );
				break;
			case 1:
					Fun_BLYNK_lcd( 0, 2 );
					Fun_BLYNK_lcd( 1, 1 );
				break;
			case 2:
					Fun_BLYNK_lcd( 0, 1 );
					Fun_BLYNK_lcd( 1, 3 );
				break;
			case 3:
					Fun_BLYNK_lcd( 0, 4 );
					Fun_BLYNK_lcd( 1, 3 );
				break;
		}
	}
	timer_1sek = millis();
}
boolean connect_to_WiFi() {
	WiFi.begin(ssid, password);// Serial.print(F("Connecting to WI-FI... "));
	for (int i = 0; i < 600; i++) {
		delay(50);
		if ( WiFi.status() == WL_CONNECTED ) return true;
		BUILTIN_LED_State = !BUILTIN_LED_State;
		if (BUILTIN_LED_State) digitalWrite ( BUILTIN_LED , HIGH );
		else digitalWrite ( BUILTIN_LED , LOW );
	}
	WiFi_Faled_Counter++;
	if (WiFi_Faled_Counter > 10) ESP.restart();
	lastConnectionAttempt = millis();
	return false;
}
void setup() {
	pinMode(BUILTIN_LED, OUTPUT);     //
	pinMode(RELE_NASOS_TPOL_BAYUA_PIN, OUTPUT);     //
	pinMode(RELE_2_stupen_PIN, OUTPUT);     //
	pinMode(RELE_NASOS_EKOTEL_PIN, OUTPUT);     //
	pinMode(RELE_NASOS_TPOL_PIN, OUTPUT);     //
	pinMode(RELE_EKOTEL_PIN, OUTPUT);     //
	digitalWrite ( RELE_NASOS_TPOL_BAYUA_PIN , HIGH );
	digitalWrite ( RELE_EKOTEL_PIN ,  HIGH); // HIGH;LOW  (rele_eKotel == 1) ? HIGH : LOW;
	digitalWrite ( RELE_NASOS_EKOTEL_PIN , HIGH );
	digitalWrite ( RELE_NASOS_TPOL_PIN , HIGH );
	digitalWrite ( RELE_2_stupen_PIN , HIGH );
	digitalWrite ( BUILTIN_LED , HIGH );
	// Serial.begin(115200); delay(10);  // Serial.println(); Serial.print(ssid); Serial.print("/"); Serial.print(password);  // Serial.println("Connecting to WI-FI");
	if ( connect_to_WiFi() ) Blynk.begin(auth, ssid, password);
	else Blynk.config(auth);
	dht.begin();
	sensors.begin();
	sensors2.setResolution(9); // установите глобальное разрешение на 9, 10, 11 или 12 бит
	sensors2.begin();
	setSyncInterval(10 * 60); // Интервал синхронизации в секундах (10 minutes) <WidgetRTC.h>
	for (int i = 0; i > 2; i++) {
	  GetTemperature();
	}
  timer_1sek = millis() + 5000;
}
void loop() {
	if ( Ota_kotel ) {
		server.on("/", []() {	server.send(200, "text/plain", "Elektro Kotel");	});
		ElegantOTA.begin(&server); server.begin(); now1 = millis();
		while (millis() - now1 < 300000) { server.handleClient(); }
		ESP.restart();
	} else {
		if ( millis() - timer_1sek > 1500 ) Func1sek();
		if ( millis() - timer_oprosa_dat4ikov > period_oprosa_dat4ikov / 3 ) GetTemperature();
		if (WiFi.status() != WL_CONNECTED) {  //  "offline"  code
			if ( millis() - lastConnectionAttempt > 60000) {
				if ( connect_to_WiFi()) WiFi_Faled_Counter = 0;
			}
		} else {
			Blynk.run();  // yield(); now = millis(); if ( Blynk.connected() )
			if ( period_Grafika ) {
				if ( millis() - timer_FuncGrafik > period_Grafika ) {
				  FuncGrafik();//
  				if ( millis() - timer_t_banya > period_Grafika * 10 ) {
  					Printterminal( "температура бани нет данных\n" );
            timer_t_banya = millis();
  				}
				}
			}
		}
	}
}
