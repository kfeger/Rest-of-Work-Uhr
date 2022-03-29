void handleRoot() {
  server.sendHeader("Location", "/index.html", true);  //Redirect to our html web page
  server.send(302, "text/plane", "");
}

bool loadFromSPIFFS(String path) {
  Serial.print("path ist ");
  String dataType = "text/plain";
  if (path.endsWith("/"))
    path += "index.htm";
  if (path.endsWith(".src"))
    path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  Serial.println(path);
  return true;
}

void handleWebRequests() {
  if (loadFromSPIFFS(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

void handleLED() {
  static int Counter = 0;
  Counter++;
  server.send(200, "text/plane", String(Counter & 1)); //Sende 0/1 only to client ajax request
}

void handleJSON(void) {
  static int CallCounter = 0;
  time_t renteFix = 1919635200;
  const size_t capacity = JSON_OBJECT_SIZE(64);
  DynamicJsonDocument doc(capacity);
  char JSONBuffer[2048];
  String strOutput;
  //String strVorname, strNachname, strCrash;
  GetEE();
  time(&now);
  localtime_r(&now, &tm);
  //strVorname = Daten.vorname;
  //strNachname = Daten.nachname;
  //strCrash = Daten.CrashLog;

  doc["compile"] = __DATE__ " " __TIME__;
  strOutput = Daten.vorname;
  doc["vorname"] = strOutput;
  strOutput = Daten.nachname;
  doc["nachname"] = strOutput;
  doc["foermlich"] = Daten.foermlich;
  doc["geschlecht"] = Daten.geschlecht;
  doc["vcc"] = ESP.getVcc();
  doc["wochentag"] = Wochentag[tm.tm_wday];
  doc["tag"] = MakeDateString();
  doc["uhrzeit"] = MakeTimeString();
  doc["rentein"] = DaysLeft;
  doc["rente"] = Daten.rente;
  if (Sensoren.bhOK)
    doc["helligkeit"] = (int)Sensoren.Helligkeit;
  else
    doc["helligkeit"] = "--";
  doc["bright"] = Daten.LED_Bright;
  doc["blink"] = Daten.LED_Blink;
  doc["showIP"] = Daten.ShowIP;
  if (Sensoren.bmeOK) {
    doc["temperatur"] = (int)Sensoren.Temperatur;
    doc["feuchte"] = (int)Sensoren.Feuchte;
    #ifdef PCB_VERSION1
      doc["druck"] = (int)(Sensoren.Druck / 100);
    #else
      doc["druck"] = 0;
    #endif
  }
  else {
    doc["temperatur"] = "--";
    doc["feuchte"] = "--";
    doc["druck"] = "--";
  }
  doc["signal"] = Signal;
  doc["county"] = County;
  doc["inzidenz"] = Inzidenz;
  doc["ags"] = Daten.InzAGS;
  doc["showInz"] = Daten.ShowInz;
  doc["baseFile"] = BaseFile;
  doc["serialnumber"] = Serialnumber;
  doc["resetReason"] = ESP.getResetReason();
  doc["startTime"] = StartTime;
  strOutput = Daten.CrashLog;
  doc["crash"] = strOutput;
  serializeJson(doc, JSONBuffer);
  Serial.print("\nsendJSON...\n");
  serializeJsonPretty(doc, Serial);
  server.send(200, "text/plane", JSONBuffer); //Send JSON Data to client ajax request
}

void readJSON() {
  DynamicJsonDocument doc(2048);

  if (server.hasArg("plain") == false) { //Check if body received
    server.send(200, "text/plain", "JSON not received");
    Serial.println("Nix empfangen");
    return;
  }
  server.send(200, "text/plain", "JSON received");
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  Serial.print("readJSON...");
  serializeJsonPretty(doc, Serial);
  GetEE();
  strcpy(Daten.vorname, doc["vorname"]);
  strcpy(Daten.nachname, doc["nachname"]);
  strcpy(Daten.InzAGS, doc["ags"]);
  Daten.foermlich = doc["foermlich"];
  Daten.geschlecht = doc["geschlecht"];
  Daten.rente = doc["rente"];
  Daten.LED_Bright = doc["bright"];
  Daten.LED_Blink = doc["blink"];
  Daten.ShowIP = doc["showIP"];
  Daten.ShowInz = doc["showInz"];
  if (!GetJSON()) {  // RKI holen
    Inzidenz = 9999.9;  // Fehlermeldung
    InzidenzInt = 9999;
    Serial.printf("Fehler vom RKI: Inzidenz auf %f, Anzeige Inz. auf %d gesetzt\n", Inzidenz, InzidenzInt);
    NextRKI = millis() + 10000;
  }
  else {
    Serial.printf("Für AGS %s (%s) -> Inzidenz: %f (int: %d)\n", AGS, County, Inzidenz, InzidenzInt);
    NextRKI = millis() + NEXT_RKI;
  }
  Serial.println("Daten gespeichert");
  PutEE();
}

void rebootHost () {
  Serial.println("Reboot ping");
  Serial.println("ESP Reboot");
  delay(1000);
  ESP.restart();
  delay(1000);
}

void initEEPROM () {
  Serial.println("EEPROM initialisieren");
  Serial.println("ESP Reboot");
  Daten.check = 0x0;
  PutEE();
  InitEE();
  delay(1000);
  ESP.restart();
  delay(1000);
}
