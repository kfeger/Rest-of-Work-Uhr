void DefServerFunctions(void) {
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/readLED", handleLED);
  server.on("/readJSON", handleJSON);
  server.on("/getJSON", readJSON);
  //Initialize Webserver
  server.on("/", handleRoot);
  server.onNotFound(handleWebRequests); //Set setver all paths are not found so we can handle as per URI
}
