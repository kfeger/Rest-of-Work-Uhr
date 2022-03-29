String MakeDateString(void) {
  String DateString = "";
  DateString += tm.tm_mday;
  //Serial.println(""); Serial.print("tm.tm_mday "), Serial.println(tm.tm_mday);
  DateString += ". ";
  DateString += Monate[tm.tm_mon];
  DateString += " ";
  DateString += tm.tm_year + 1900;
  return DateString;
}

String MakeTimeString(void) {
  String TimeString = "";
  TimeString += tm.tm_hour;
  TimeString += ":";
  if (tm.tm_min < 10)
    TimeString += "0";
  TimeString += tm.tm_min;
  return TimeString;
}

int MakeDateDisplay (void) {
  int Day, Month;
  Day = tm.tm_mday;
  Month = tm.tm_mon + 1;
  return (Day * 100 + Month);
}

void wmPrintAP (WiFiManager *myWiFiManager) {
  SAA.printAP();
}

void MakeTimeDisplay(void) {
  if (Daten.LED_Blink) {
    if (tm.tm_sec & 1) {
      SAA.printTime(tm.tm_hour * 100 + tm.tm_min, true);
    }
    else {
      SAA.printTime(tm.tm_hour * 100 + tm.tm_min, false);
    }
  }
  else {
    SAA.printTime(tm.tm_hour * 100 + tm.tm_min, true);
  }
}

void GetSerialnumber (void) {
  int i = 0;
  for(i = 0; i < 4; i++) {
    Serialnumber[i] = random(0x41, 0x5b);
  }
  for(i ; i < 9; i++) {
    Serialnumber[i] = random(0x30, 0x3a);
    Serialnumber[i+1] = 0;
  }
}
