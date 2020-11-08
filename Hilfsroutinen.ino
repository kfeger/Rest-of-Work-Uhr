String MakeDateString(time_t DateTime) {
  String DateString = "";
  DateString = day(DateTime);
  DateString += ".";
  DateString += month(DateTime);
  DateString += ".";
  DateString += year(DateTime);
  return DateString;
}

String MakeTimeString(time_t DateTime) {
  String TimeString = "";
  TimeString += hour(DateTime);
  TimeString += ":";
  if(minute(DateTime) < 10)
    TimeString += "0";
  TimeString += minute(DateTime);
  return TimeString;
}

time_t SyncTimeToNTP(void) {
  Serial.println("******Time-Sync******");
  timeClient.update();
  return(LTZ.toLocal(timeClient.getEpochTime(), &tcr));
}

time_t TimeCalc (int Td, int Tm, int Ty) {
  tmElements_t Tin;
  Tin.Second = 0;
  Tin.Minute = 0;
  Tin.Hour = 0;
  Tin.Day = Td;
  Tin.Month = Tm;
  Tin.Year = Ty;
  return (makeTime(Tin));
}
