/*
   Hier wird die etwas komplexere Struktur vom
   RKI geholt.
   Die Verbindung ist https://
   RKI-URL für örtliche (AGS 17612, Dresden) Inzidenz
  https://services7.arcgis.com/mOBPykOjAyBO2ZKk/arcgis/rest/services/RKI_Landkreisdaten/FeatureServer/0/query?where=AGS%20%3D%20%2714612%27&outFields=cases7_per_100k&outSR=4326&f=json
  AGS-Karte
  https://npgeo-corona-npgeo-de.hub.arcgis.com/datasets/917fc37a709542548cc3be077a786c17_0/explore?location=51.061877%2C8.718827%2C6.67
  Erfordert
  #include <ESP8266HTTPClient.h>
  #include <WiFiClientSecureBearSSL.h>
  #include <ArduinoJson.h>
  ********* Globale Variable
  const char apiURL1[] PROGMEM = "https://services7.arcgis.com/mOBPykOjAyBO2ZKk/arcgis/rest/services/RKI_Landkreisdaten/FeatureServer/0/query?where=AGS %3D%27";
  const char apiURL2[] PROGMEM = "%27&outFields=county,last_update,cases7_per_100k&returnGeometry=false&outSR=4326&f=json";
  float Inzidenz = 0.0;
  int InzidenzInt = 0;
  char LastUpdate[32] = "21.03.1954, 12:12 Uhr";
  char AGS[8] = "14612";
  char County[128] = "Test-Ort";
  uint32_t NextRKI = 0;
  String payload;
  char apiURL[300] = "";
*/

bool GetJSON (void) {
  bool ReturnCode = false;
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  HTTPClient https;
  //strcpy(apiURL, "");
  strcpy_P(apiURL, apiURL1);
  strcat(apiURL, Daten.InzAGS);
  strcat_P(apiURL, apiURL2);
  //Serial.println(apiURL);
  if (https.begin(*client, apiURL)) {
    //http.addHeader("Accept", "application/json");
    int code = https.GET();
    if (code == HTTP_CODE_OK || code == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = https.getString();
      if (payload.length() > 128) {
        deserializeJson(doc, payload);
        strcpy(LastUpdate, doc["features"][0]["attributes"]["last_update"]);
        Inzidenz = doc["features"][0]["attributes"]["cases7_per_100k"];
        strcpy(County, doc["features"][0]["attributes"]["county"]);
        InzidenzInt = round(Inzidenz * 10);
        ReturnCode = true;
      }
      else {
        Serial.println("Fehlermeldung vom RKI");
        ReturnCode = false;
      }
    }
    else {
      Serial.printf("[HTTPS] GET... failed, Fehler ist: %d, %s\n", code, https.errorToString(code).c_str());
      ReturnCode = false;
    }
  }
  else {
    Serial.printf("HTTPS-Begin hat nicht geklappt\n");
    ReturnCode = false;
  }
  https.end();
  return (ReturnCode);
}
