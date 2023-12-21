/*
   D1_RoW_Base_ClockNewNTO_RKI.ino
   RoW-Uhr mit Anzeige der Inzidenz
   für eine beliebige AGS des RKI
   15.11.2023:
   V2.1 nun ohne Inzidenz. Das RKI
   hat sein Corona-Portal abgeschaltet.
*/
#include <Wire.h>  // This library is required to be included as well

// die Platinenversionen
// Was wollen wir bauen?
#include "RoW_pcb_version.h"  //siehe C:\Users\Besitzer\Documents\Arduino18-xeon\libraries\SAA1064RoW

#if defined(PCB_VERSION1) && defined(PCB_VERSION2)
  #error "Platine doppelt definiert!"
#endif

#if !defined(PCB_VERSION1) && !defined(PCB_VERSION2)
  #error "Keine Platine definiert!"
#endif

#include <SAA1064RoW.h>

#if defined(PCB_VERSION1) // mit BME280, mittlerweile zu teuer
  #pragma message ("Für Platine Version 1")
  #include <Adafruit_Sensor.h>
  #include <Adafruit_BME280.h>
#else
  #pragma message ("Für Platine Version 2") // mit HDC1080, leider ohne Luftdruck
  #include "ClosedCube_HDC1080.h"
#endif

#include <BH1750.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h> //für http-Update
#include <ESP8266Ping.h>
#include <DNSServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESP8266Ping.h>
#include <FS.h>   //Include File System Headers
#include <EEPROM.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include "time.h"


#define sdaPIN D2
#define sclPIN D1
#define startAP D5
#define SYNC_INTERVAL 1800  //alle 30min Zeit synchronisieren
#define TIME_TO_PING 423    //alle rund 7 min Ping und WLAN prüfen
#define TIME_TO_RKI 3600  //alle Stunde RKI abfragen

ADC_MODE(ADC_VCC);

// ***** UDP, NTP und Timezone *****

#define MY_NTP_SERVER "pool.ntp.org"

//#define CHILE
//#define UK
#if defined CHILE
#pragma message("Compile für Santiago de Chile")
// Chile Standard Time für Santiago
#define MY_TZ "CLST-4CLT-3,M4.1.0/00:00:00,M9.5.0/00:00:00"
#elif defined UK
#pragma message ("Compile für UK")
#define MY_TZ "GMT+0BST-1,M3.5.0/01:00:00,M10.5.0/02:00:00"
#else
#pragma message("Compile für mitteleuropäische Zeit")
// Central European Time (Frankfurt, Paris) from Timezone/examples/WorldClock/WorldClock.pde
#define MY_TZ "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"
#endif


ESP8266WebServer server(80); //Server on port 80
ESP8266HTTPUpdateServer serverUpdater;
String BaseFile;
char Serialnumber[22];


const char* Wochentag[8] = {
  "Sonntag",
  "Montag",
  "Dienstag",
  "Mittwoch",
  "Donnerstag",
  "Freitag",
  "Samstag",
  "Fehler"
};
const char* Monate[12] = {
  "Januar",
  "Februar",
  "März",
  "April",
  "Mai",
  "Juni",
  "Juli",
  "August",
  "September",
  "Oktober",
  "November",
  "Dezember"
};

time_t now;
tm tm;

IPAddress ip;

float Temperatur, Feuchte, Druck, Helligkeit ;
int Signal = 0;
int Bright = 0;
bool ShowMore = true, ShowTick = false;
long startAPDown = 0;
bool InitClock = true;
StaticJsonDocument<2048> doc;


// RKI-Daten
#define RKI_INTERVAL 60000
const char apiURL1[] PROGMEM = "https://services7.arcgis.com/mOBPykOjAyBO2ZKk/arcgis/rest/services/RKI_Landkreisdaten/FeatureServer/0/query?where=AGS %3D%27";
const char apiURL2[] PROGMEM = "%27&outFields=county,last_update,cases7_per_100k&returnGeometry=false&outSR=4326&f=json";
float Inzidenz = 0.0;
int InzidenzInt = 0;
char LastUpdate[32] = "21.03.1954, 12:12 Uhr";
char AGS[8] = "14612";
char County[128] = "SK Dresden";
uint32_t NextRKI = 0;
#define NEXT_RKI 1800000 // alle 30 min
String payload;
char apiURL[300] = "";

#ifdef PCB_VERSION1
  Adafruit_BME280 Klima; // Temp-Feuchte-Druck
  int withBME = 1;
#else
  ClosedCube_HDC1080 Klima; //Temp & Feuchte
  int withBME = 0;
#endif

BH1750 Licht;
SAA1064RoW SAA = SAA1064RoW(0);
time_t LastShow = 0, LastSync = 0, LastPing = 0;
time_t DaysLeft = 0;
String StartTime;

struct EEData_t {
  int check;
  char vorname[64];
  char nachname[64];
  long long rente;
  int foermlich;
  int geschlecht;
  int ShowMore;
  int LED_Bright;
  int LED_Blink;
  int ShowIP;
  char CrashLog[64];
};
EEData_t Daten;

struct Sensor_t {
  float Temperatur;
  float Druck;
  float Feuchte;
  float Helligkeit;
  bool bmeOK;
  bool bhOK;
};
Sensor_t Sensoren;

void setup() {
  Serial.begin(115200);
  randomSeed(ESP.getVcc());

  pinMode(startAP, INPUT);
  WiFiManager wifiManager;
  if (!digitalRead(startAP))
    wifiManager.resetSettings();

  GetSerialnumber();
  // Dateinamen holen und parsen
  BaseFile = String(__BASE_FILE__);
  int LastSlash = BaseFile.lastIndexOf('\\');
  String BaseFileSub = BaseFile.substring(LastSlash + 1);
  BaseFile = BaseFileSub.substring(0, BaseFileSub.length() - 8);
#if defined(PCB_VERSION1)
  BaseFile += "(Platine V1)";
#else
  BaseFile += "(Platine V2)";
#endif
  // fertig
  Serial.println("");
  Serial.println(BaseFile);
  pinMode(startAP, INPUT_PULLUP);
  SPIFFS.begin();
  EEPROM.begin(512);
  Wire.begin(sdaPIN, sclPIN);
  SAA.begin();
  SAA.printLongFun();
  delay(250);
  Sensoren.bhOK = Licht.begin();

#ifdef PCB_VERSION1
  Sensoren.bmeOK = Klima.begin(0x76); // BME280
#else
  Klima.begin(0x40);  // HDC1080
  Klima.setResolution(HDC1080_RESOLUTION_8BIT, HDC1080_RESOLUTION_11BIT);
  Sensoren.bmeOK = 1;
#endif

  delay(250);
  Temperatur = Klima.readTemperature(); // °C
  if (Temperatur > 120.0) // Sensorfehler melden
    Sensoren.bmeOK = 0;
  else
    Sensoren.bmeOK = 1;
  SAA.brightness(2);
  SAA.print4Dot();  // 4 Dots
  Serial.println("");
  Serial.println("Geht los!");
  Serial.println("");
  uint8_t macAddr[6];
  char Hostname[15];
  WiFi.macAddress(macAddr);
  sprintf(Hostname, "ottO-%02x-%02x-%02x", macAddr[3], macAddr[4], macAddr[5]);
  WiFi.hostname(Hostname);

  wifiManager.setDebugOutput(false);
  wifiManager.setAPCallback(wmPrintAP);
  if (!wifiManager.autoConnect("RoW-Uhr-AP")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.restart();
    delay(5000);
  }

  /*
    WiFi.begin("orange", "w!r messen 1000 Werte");
    int TryCount = 0;
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    TryCount++;
    if (TryCount > 20) {
      Serial.println("WLAN nicht gefunden!");
      delay (2000);
      ESP.restart();
      delay(100);
    }
    }
    Serial.println("");
  */

  ip = WiFi.localIP();
  //Serial.print("IP address: ");
  //Serial.println(ip);  //IP address assigned to your ESP
  Serial.printf("IP-Adresse ist %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
  for (int i = 0; i < 4; i++) {
    SAA.print((int)ip[i], 1);
    delay(1500);
  }
  delay(100);
  configTzTime(MY_TZ, MY_NTP_SERVER);
  while (time(&now) < 1000) { //ungültige Zeit
    delay(1000);  //Schamfrist für NTP
    configTzTime(MY_TZ, MY_NTP_SERVER);
    Serial.println("Warte auf NTP");
  }
  localtime_r(&now, &tm);
  StartTime = MakeDateString() + ", " + MakeTimeString();
  LastSync = now;
  LastShow = now;
  SAA.brightness(CalcBrightness());
  delay(1000);
  InitEE();
  GetEE();
  if (Sensoren.bmeOK) {
    Feuchte = Klima.readHumidity();
    Temperatur = Klima.readTemperature();
    Sensoren.Feuchte = Klima.readHumidity();
    Sensoren.Temperatur = Klima.readTemperature();
#ifdef PCB_VERSION1
    Sensoren.Druck = Klima.readPressure();
#endif
#ifdef PCB_VERSION2
    Sensoren.Druck = 0;
#endif

  }
  if (Sensoren.bhOK) {
    Sensoren.Helligkeit = Licht.readLightLevel();
  }
  Signal = WiFi.RSSI();
  DefServerFunctions();
  serverUpdater.setup(&server);
  server.begin();
  MDNS.addService("http", "tcp", 80);
}

void loop() {
  server.handleClient();
  time(&now);
  localtime_r(&now, &tm);
  if (now > LastPing) {
    if (WiFi.status() != WL_CONNECTED) {
      // Nicht im WLAN
      strcpy(Daten.CrashLog, "WLAN-Fehler");
      PutEE();
      Serial.println("WLAN-Fehler");
      ESP.restart();
    }
    else {
      Serial.println("+++++WLAN o.k.++++++");
      LastPing = now + TIME_TO_PING;
      }
  }
  if (LastShow != time(&now)) {
    DaysLeft = (Daten.rente - now) / 86400;
    DisplayData();
    SAA.brightness(CalcBrightness());
    LastShow = time(&now);
  }
  if (digitalRead(startAP)) {
    startAPDown = millis();
    ShowTick = false;
  }
  else {
    if (((millis() - startAPDown) > 2000) && !ShowTick) {
      if (Daten.ShowMore == 1) {
        Daten.ShowMore = 0;
        SAA.printDotDot();
      }
      else {
        Daten.ShowMore = 1;
        SAA.printDashDash(0);
      }
      PutEE();
      ShowTick = true;
      Serial.printf("Daten.ShowMore ist jetzt %d\n", Daten.ShowMore);
      while (!digitalRead(startAP));
    }
  }
}

void DisplayData(void) {
  time(&now);
  localtime_r(&now, &tm); // update the structure tm with the current time
  yield();
  if (Daten.ShowMore) {
    switch (tm.tm_sec) {
      case 0:
        Signal = WiFi.RSSI();
        if (Sensoren.bmeOK) {
          Feuchte = Klima.readHumidity();
          Temperatur = Klima.readTemperature();
          Sensoren.Feuchte = Klima.readHumidity();
          Sensoren.Temperatur = Klima.readTemperature();
          #ifdef PCB_VERSION1
            Sensoren.Druck = Klima.readPressure();
          #else
            Sensoren.Druck = 0;
          #endif
        }
        if (Sensoren.bhOK) {
          Sensoren.Helligkeit = Licht.readLightLevel();
        }
        MakeTimeDisplay();
        break;
      case 1:
      case 2:
        SAA.print(MakeDateDisplay(), 5);
        break;
      case 30:
      case 31:
      case 32:
        int DaysLeftShort;
        if (DaysLeft >= 0) {
          if (DaysLeft <= 9999) { // positiv, Rente kommt
            DaysLeftShort = (int)DaysLeft;
            SAA.print(DaysLeftShort);  // Tage bis Ruhestand
          }
          else {
            DaysLeftShort = (int)(DaysLeft / 365);
            SAA.printYear(DaysLeftShort);  // Jahre bis Ruhestand
          }
        }
        else {  // negativ, in Rente
          if (DaysLeft > -1000) {
            DaysLeftShort = (int)DaysLeft;
            SAA.print(DaysLeftShort);
          }
          else {  // ab 1000 Tage Rente werden die Jahre angezeigt
            DaysLeftShort = (int)(DaysLeft / 365);
            SAA.printYear(DaysLeftShort);
          }
        }
        break;
      case 43:
      case 44:
        if (Sensoren.bmeOK)
          SAA.printTemp((int)Temperatur);
        else
          SAA.printDashDash(1);
        break;
      case 45:
      case 46:
        if (Sensoren.bmeOK)
          SAA.printHum((int)Feuchte);
        else
          SAA.printDashDash(2);
        break;
      case 55:
        if (Daten.ShowIP)
          SAA.print((int)ip[0], 1);
        else
          MakeTimeDisplay();
        break;
      case 56:
        if (Daten.ShowIP)
          SAA.print((int)ip[1], 1);
        else
          MakeTimeDisplay();
        break;
      case 57:
        if (Daten.ShowIP)
          SAA.print((int)ip[2], 1);
        else
          MakeTimeDisplay();
        break;
      case 58:
        if (Daten.ShowIP)
          SAA.print((int)ip[3], 1);
        else
          MakeTimeDisplay();
        break;
      default:
        MakeTimeDisplay();
        break;
    }
  }
  else
    SAA.printTime(tm.tm_hour * 100 + tm.tm_min, true);
}

int CalcBrightness (void) {
#define MAXBRIGHT 10
#define MINBRIGHT 1
#define BRIGHT_NO_SENSOR 4
#define BRIGHTMAX 7
  //float Helligkeit = 0;
  if (!Sensoren.bhOK) {
    Bright = Daten.LED_Bright;
    return (Bright);
  }
  Helligkeit = Licht.readLightLevel();
  if ((int)Helligkeit >= MAXBRIGHT)
    Bright = Daten.LED_Bright;
  else if ((int)Helligkeit < MINBRIGHT)
    Bright = 1;
  else
    Bright = (map((int)Helligkeit, MINBRIGHT, MAXBRIGHT, 1, Daten.LED_Bright + 1));
  //Serial.printf("Helligkeit ist %6.1f lux, Bright ist %d\n", Helligkeit, Bright);
  return (Bright);
}
