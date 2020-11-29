// Die Javascript-Datei für die index.html

var HeuteTag, HeuteMonat, HeuteJahr;
var JetztMinute, JetztStunde;
var LedValue1 = "<img src=\"/Blank.png\" alt=\"Blank\" width=\"70\" height=\"30\"> <img src=\"/";
var LedValue2 = "\" alt=\"Tick\" width=\"60\" height=\"60\"><img src=\"/Blank.png\" alt=\"Blank\" width=\"30\" height=\"30\"><img src=\"/";
var LedValue3 = "\" alt=\"Tack\" width=\"60\" height=\"60\"> </h2>";

function getData() {
  var xhttp = new XMLHttpRequest();
  var LedValue;
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
		if (this.responseText == "1")
			LedValue = LedValue1 + "LED-R.png" + LedValue2 + "LED-G.png" + LedValue3;
		else
			LedValue = LedValue1 + "LED-G.png" + LedValue2 + "LED-R.png" + LedValue3;
      document.getElementById("BlinkLED").innerHTML = LedValue;
    }
  };
  xhttp.open("GET", "readLED", true);
  xhttp.send();
}

setInterval(function() {
  // Call a function repetatively with 1 Second interval
  getJSON();
}, 1000); //1000ms update rate

setInterval(function() {
  // Call a function repetatively with 1 Second interval
  getData();
}, 1234); //1234ms update rate



function getJSON() {
  var xhttp = new XMLHttpRequest();
  var vorname, nachname, foermlich, geschlecht;
  xhttp.onreadystatechange = function() {
	if (this.readyState == 4 && this.status == 200) {
		var ESPObj = JSON.parse(this.responseText);
		
		nachname = ESPObj.nachname;
		geschlecht = ESPObj.geschlecht;
		foermlich = ESPObj.foermlich;
		if (foermlich == 0) {
			document.getElementById("anrede").innerHTML = "Hallo";
			document.getElementById("name").innerHTML = ESPObj.vorname;
		}
		else {
			if(geschlecht == 1)
				document.getElementById("anrede").innerHTML = "Sehr geehrte<br>Frau";
			else
				document.getElementById("anrede").innerHTML = "Sehr geehrter<br>Herr";
			document.getElementById("name").innerHTML = ESPObj.nachname;
		}
		document.getElementById("wochentag").innerHTML = ESPObj.wochentag;
		document.getElementById("tag").innerHTML = ESPObj.tag;
		document.getElementById("uhrzeit").innerHTML = ESPObj.uhrzeit;
		document.getElementById("compile").innerHTML = ESPObj.compile;
		document.getElementById("rentein").innerHTML = ESPObj.rentein;
		document.getElementById("temperatur").innerHTML = ESPObj.temperatur;
		document.getElementById("feuchte").innerHTML = ESPObj.feuchte;
		document.getElementById("druck").innerHTML = ESPObj.druck;
		document.getElementById("helligkeit").innerHTML = ESPObj.helligkeit;
		document.getElementById("vcc").innerHTML = ESPObj.vcc;
		console.log(nachname, foermlich, geschlecht);
	}
  };
  xhttp.open("GET", "readJSON", true);
  xhttp.send();
}
