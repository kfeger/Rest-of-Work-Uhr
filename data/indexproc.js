// Die Javascript-Datei für die index.html

var HeuteTag, HeuteMonat, HeuteJahr;
var JetztMinute, JetztStunde;
var LedValue1 = "<img src=\"/Blank.png\" width=\"70\" height=\"30\"> <img src=\"";
var LedValue2 = "\" width=\"60\" height=\"60\"><img src=\"/Blank.png\" width=\"30\" height=\"30\"><img src=\"";
var LedValue3 = "\" width=\"60\" height=\"60\"> </h2>";

var imgR = new Image();
var imgG = new Image();
var imgBlank = new Image();

function getData() {
  var xhttp = new XMLHttpRequest();
  var LedValue;
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
		if (this.responseText == "1") {
			//document.getElementById("led1").innerHTML = '<img src = "/LED-R.png">';
			//document.getElementById("led2").innerHTML = '<img src = "/LED-G.png">';
			LedValue = LedValue1 + "/LED-R.png" + LedValue2 + "/LED-G.png" + LedValue3;
		}
		else {
			//document.getElementById("led1").innerHTML = '<img src = "/LED-G.png">';
			//document.getElementById("led2").innerHTML = '<img src = "/LED-R.png">';
			LedValue = LedValue1 + "/LED-G.png" + LedValue2 + "/LED-R.png" + LedValue3;
		}
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
		if (ESPObj.rentein >= 0)
			//document.getElementById("rentein").innerHTML = ESPObj.rentein;
			document.getElementById("renteText").innerHTML = "<h2>Strecke bis zum Ziel...<br>...noch <font color = \"red\">" + ESPObj.rentein + "</font> Tage.</h2>"
		else
			document.getElementById("renteText").innerHTML = "<h2>Im Ziel angekommen...<br>...seit <font color = \"red\">" + (ESPObj.rentein * -1) + "</font> Tage.</h2>"
		document.getElementById("temperatur").innerHTML = ESPObj.temperatur;
		document.getElementById("feuchte").innerHTML = ESPObj.feuchte;
        
        if(ESPObj.druck == 0)
            document.getElementById("druck").innerHTML = "---";
        else
            document.getElementById("druck").innerHTML = ESPObj.druck;
        
		document.getElementById("helligkeit").innerHTML = ESPObj.helligkeit;
		document.getElementById("vcc").innerHTML = ESPObj.vcc;
		document.getElementById("county").innerHTML = ESPObj.county;
		//document.getElementById("ags").innerHTML = ESPObj.ags;
		var Inzidenz = ESPObj.inzidenz;
		document.getElementById("inzidenz").innerHTML = Inzidenz.toFixed(1);
		document.getElementById("baseFile").innerHTML = ESPObj.baseFile;
		document.getElementById("serialnumber").innerHTML = ESPObj.serialnumber;
	}
  };
  xhttp.open("GET", "readJSON", true);
  xhttp.send();
}

function PreLoadImages() {
    imgR.src = "/LED-R.png";
    imgG.src = "/LED-G.png";
    imgBlank.src = "/LED-Blank.png";
}

