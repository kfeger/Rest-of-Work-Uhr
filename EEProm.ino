/*
   Hier werden die persistenten Daten
   der Rest-of-Work-Uhr im EEPROM
   behandelt.
*/
void InitEE (void) {
  EEPROM.get(0, Daten);
  if (Daten.check != 0x55aa) {
    Daten.check = 0x55aa;
    strcpy(Daten.vorname, "John");
    strcpy(Daten.nachname, "Doe");
    Daten.rente = 2026512000; // 21.03.2034, 00:00
    Daten.foermlich = 0;
    Daten.geschlecht = 0;
    Daten.ShowMore = 1;
    Daten.LED_Bright = 5;
    Daten.LED_Blink = 0;
    Daten.ShowIP = 1;
    strcpy(Daten.CrashLog, "kein Eintrag, EE init.");
    EEPROM.put(0, Daten);
    EEPROM.commit();
    Serial.println("EEPROM initialisiert");
  }
  else {
    Serial.println("EEPROM ok");
  }
  //Serial.printf("+++++++\ncheck: 0x%x\n Vorname: %s\nNachname: %s\nRente am %ul\n", Daten.check, Daten.vorname, Daten.nachname, Daten.rente);
  //Serial.printf("Förmlich: %d, Geschlecht (m=0): %d, LED : %d, Blink = %d\n", Daten.foermlich, Daten.geschlecht, Daten.LED_Bright, Daten.LED_Blink);
  //Serial.printf("AGS: %s, ShowInz: %d\n+++++\n", Daten.InzAGS, Daten.ShowInz);
}

void GetEE (void) {
  EEPROM.get(0, Daten);  
  //Serial.printf("+++++++\ncheck: 0x%x\n Vorname: %s\nNachname: %s\nRente am %ul\n", Daten.check, Daten.vorname, Daten.nachname, Daten.rente);
  //Serial.printf("Förmlich: %d, Geschlecht (m=0): %d, LED : %d, Blink = %d\n+++++\n", Daten.foermlich, Daten.geschlecht, Daten.LED_Bright, Daten.LED_Blink);
  //Serial.printf("AGS: %s, ShowInz: %d\n+++++\n", Daten.InzAGS, Daten.ShowInz);
  //Serial.println(time(&now));
}

void PutEE (void) {
  EEPROM.put(0, Daten);
  EEPROM.commit();
}
