// Counter.ino - Example from the SAA1064 library for Arduino
// http://photonicsguy.ca/projects/electronics/7segdisplay

#include <Wire.h>	// This library is required to be included as well
#include <SAA1064.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
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
//#include <LittleFS.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUDP.h>
#include <TimeLib.h>
#include <Timezone.h>     // from https://github.com/JChristensen/Timezone
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>


#define sdaPIN D2
#define sclPIN D1
#define startAP D5
#define SYNC_INTERVAL 1800  //alle 30min Zeit synchronisieren
#define TIME_TO_PING 423    //alle 5 min Ping

ADC_MODE(ADC_VCC);

// ***** UDP, NTP und Timezone *****

//#define CHILE

// local time zone definition
#if defined CHILE
  #pragma message("Compile für Santiago de Chile")
  // Chile Standard Time für Santiago
  TimeChangeRule CLT = {"CLT", First, Sun, Sep, 0, -180};     //Chile Standard Time
  TimeChangeRule CLST = {"CLST", Last, Sun, Apr, 0, -240};       //Chile Summer Time
  Timezone LTZ(CLST, CLT);    // this is the Timezone object that will be used to calculate local time
#else
  #pragma message("Compile für mitteleuropäische Zeit")
  // Central European Time (Frankfurt, Paris) from Timezone/examples/WorldClock/WorldClock.pde
  TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
  TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
  Timezone LTZ(CEST, CET);    // this is the Timezone object that will be used to calculate local time
#endif

TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev


unsigned int localPort = 2390;      // local port to listen for UDP packets
/* Don't hardwire the IP address or we won't get the benefits of the pool.
    Lookup the IP address for the host name instead */
IPAddress timeServerIP; // NTP pool server address
const char* ntpServerName = "pool.ntp.org";

// A UDP instance to let us send and receive packets over UDP
const long utcOffsetInSeconds = 0;
unsigned long UpdateIntervall = 1800;
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", utcOffsetInSeconds, UpdateIntervall);
ESP8266WebServer server(80); //Server on port 80
ESP8266HTTPUpdateServer serverUpdater;
tmElements_t TP;
const char* Wochentag[8] = {
  "Fehler",
  "Sonntag",
  "Montag",
  "Dienstag",
  "Mittwoch",
  "Donnerstag",
  "Freitag",
  "Samstag"
};

IPAddress ip;

float Temperatur, Feuchte, Druck, Helligkeit ;
int Bright = 0;
bool ShowMore = true, ShowTick = false;
long startAPDown = 0;

Adafruit_BME280 bme; // Temp-Feuchte-Druck
BH1750 Licht;
SAA1064 SAA = SAA1064(0);
time_t LastShow = 0, LastSync = 0, LastPing = 0;

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
};

time_t DaysLeft = 0;

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
  pinMode(startAP, INPUT_PULLUP);
  WiFiManager wifiManager;
  SPIFFS.begin();
  EEPROM.begin(200);
  Wire.begin(sdaPIN, sclPIN);
  SAA.begin();
  Serial.begin(115200);
  Sensoren.bhOK = Licht.begin();
  Sensoren.bmeOK = bme.begin(0x76);
  delay(100);
  SAA.brightness(2);
  SAA.dash();  // Display dashes
  Serial.println("");
  Serial.println("Geht los!");
  Serial.println("");
  uint8_t macAddr[6];
  char Hostname[15];
  WiFi.macAddress(macAddr);
  sprintf(Hostname, "ottO-%02x-%02x-%02x", macAddr[3], macAddr[4], macAddr[5]);
  WiFi.hostname(Hostname);

  wifiManager.setDebugOutput(false);
  if (!digitalRead(startAP))
    wifiManager.resetSettings();
  if (!wifiManager.autoConnect("RoW-Uhr-AP")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  ip = WiFi.localIP();
  //Serial.print("IP address: ");
  //Serial.println(ip);  //IP address assigned to your ESP
  Serial.printf("IP-Adresse ist %d.%d.%d.%d\n",ip[0],ip[1],ip[2],ip[3]);
  for (int i = 0; i < 4; i++) {
    SAA.print((int)ip[i], 1);
    delay(1500);
  }
  //WiFi.printDiag(Serial);
  //udp.begin(localPort);
  delay(100);
  setSyncProvider(SyncTimeToNTP);
  setSyncInterval(1800);
  LastSync = now();

  LastShow = now();
  //SAA.brightness(CalcBrightness());
  delay(1000);
  InitEE();
  GetEE();
  DefServerFunctions();
  serverUpdater.setup(&server);
  server.begin();
  MDNS.addService("http", "tcp", 80);
}

void loop() {
  server.handleClient();
  MDNS.update();
  if (now() >= (LastPing + TIME_TO_PING)) {
    if (!Ping.ping(WiFi.gatewayIP(), 1)) {
      // Nicht im Netz
      Serial.println("Ping-Fehler");
      ESP.restart();
    }
    Serial.println("+++++Ping o.k.++++++");
    LastPing = now();
  }
  if (LastShow != now()) {
    if (WiFi.status() != WL_CONNECTED) {
      ESP.reset();
    }
    if (Sensoren.bmeOK) {
      Feuchte = bme.readHumidity();
      Temperatur = bme.readTemperature();
      Sensoren.Feuchte = bme.readHumidity();
      Sensoren.Temperatur = bme.readTemperature();
      Sensoren.Druck = bme.readPressure();
    }
    if (Sensoren.bhOK) {
      Sensoren.Helligkeit = Licht.readLightLevel();
    }
    DaysLeft = (Daten.rente - now()) / 86400;
    DisplayData();

    SAA.brightness(CalcBrightness());
    LastShow = now();
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
  if (Daten.ShowMore) {
    switch (second(now())) {
      case 14:
      case 15:
      case 16:
        SAA.print(MakeDateDisplay(), 5);
        break;
      case 29:
      case 30:
      case 31:
        int DaysLeftShort;
        if (DaysLeft <= 9999) {
          DaysLeftShort = (int)DaysLeft;
          SAA.print(DaysLeftShort);  // Tage bis Ruhestand
        }
        else {
          DaysLeftShort = (int)(DaysLeft / 365);
          SAA.printYear(DaysLeftShort);  // Jahre bis Ruhestand
        }
        break;
      case 44:
      case 45:
        if (Sensoren.bmeOK)
          SAA.printTemp((int)Temperatur);
        else
          SAA.printDashDash(1);
        break;
      case 46:
      case 47:
        if (Sensoren.bmeOK)
          SAA.printHum((int)Feuchte);
        else
          SAA.printDashDash(2);
        break;
      case 56:
        if (Daten.ShowIP)
          SAA.print((int)ip[0], 1);
        else {
          if (Daten.LED_Blink) {
            if (second(now()) & 1)
              SAA.printTime(hour(now()) * 100 + minute(now()), true);
            else
              SAA.printTime(hour(now()) * 100 + minute(now()), false);
          }
          else {
            SAA.printTime(hour(now()) * 100 + minute(now()), true);
          }
        }
        break;
      case 57:
        if (Daten.ShowIP)
          SAA.print((int)ip[1], 1);
        else {
          if (Daten.LED_Blink) {
            if (second(now()) & 1)
              SAA.printTime(hour(now()) * 100 + minute(now()), true);
            else
              SAA.printTime(hour(now()) * 100 + minute(now()), false);
          }
          else {
            SAA.printTime(hour(now()) * 100 + minute(now()), true);
          }
        }
        break;
      case 58:
        if (Daten.ShowIP)
          SAA.print((int)ip[2], 1);
        else {
          if (Daten.LED_Blink) {
            if (second(now()) & 1)
              SAA.printTime(hour(now()) * 100 + minute(now()), true);
            else
              SAA.printTime(hour(now()) * 100 + minute(now()), false);
          }
          else {
            SAA.printTime(hour(now()) * 100 + minute(now()), true);
          }
        }
        break;
      case 59:
        if (Daten.ShowIP)
          SAA.print((int)ip[3], 1);
        else {
          if (Daten.LED_Blink) {
            if (second(now()) & 1)
              SAA.printTime(hour(now()) * 100 + minute(now()), true);
            else
              SAA.printTime(hour(now()) * 100 + minute(now()), false);
          }
          else {
            SAA.printTime(hour(now()) * 100 + minute(now()), true);
          }
        }
        break;
      default:
        if (Daten.LED_Blink) {
          if (second(now()) & 1)
            SAA.printTime(hour(now()) * 100 + minute(now()), true);
          else
            SAA.printTime(hour(now()) * 100 + minute(now()), false);
        }
        else {
          SAA.printTime(hour(now()) * 100 + minute(now()), true);
        }
        break;
    }
  }
  else
    SAA.printTime(hour(now()) * 100 + minute(now()), true);
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
