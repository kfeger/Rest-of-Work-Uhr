// Die Javascript-Datei für die index.html

var HeuteTag, HeuteMonat, HeuteJahr;
var JetztMinute, JetztStunde;
var LedValue1 = "<img src=\"/Blank.png\" width=\"70\" height=\"30\"> <img src=\"";
var LedValue2 = "\" width=\"60\" height=\"60\"><img src=\"/Blank.png\" width=\"30\" height=\"30\"><img src=\"";
var LedValue3 = "\" width=\"60\" height=\"60\"> </h2>";

var imgR = new Image();
var imgG = new Image();
var imgBlank = new Image();
var InitialLoad = true;

var serlialNumber;

function getData() {
  var xhttp = new XMLHttpRequest();
  var LedValue;
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
		if (this.responseText == "1") {
			LedValue = LedValue1 + "/LED-R.png" + LedValue2 + "/LED-G.png" + LedValue3;
		}
		else {
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
			document.getElementById("renteText").innerHTML = "<h2>Strecke bis zum Ziel...<br>...noch <font color = \"red\">" + ESPObj.rentein + "</font> Tage.</h2>"
		else
			document.getElementById("renteText").innerHTML = "<h2>Im Ziel angekommen...<br>...seit <font color = \"red\">" + (ESPObj.rentein * -1) + "</font> Tage.</h2>"
		document.getElementById("temperatur").innerHTML = ESPObj.temperatur;
		document.getElementById("feuchte").innerHTML = ESPObj.feuchte;
        if(ESPObj.withBME == 1) {
            document.getElementById("druck").innerHTML = ESPObj.druck;
            document.getElementById("druckUnit").innerHTML = "hPa";
        }
        else {
            document.getElementById("druck").innerHTML = "kein";
            document.getElementById("druckUnit").innerHTML = "Sensor";
        }
		document.getElementById("helligkeit").innerHTML = ESPObj.helligkeit;
		document.getElementById("vcc").innerHTML = ESPObj.vcc;
		document.getElementById("baseFile").innerHTML = ESPObj.baseFile;
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

function copySN() {
    var Url = document.getElementById("paste-box");
    Url.value = "Test";
    Url.select();
    document.execCommand("Copy");
    /* Alert the copied text */
    alert("Copied the text: " + Url.value);
}


