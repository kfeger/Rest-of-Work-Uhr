// Die Javascript-Datei für die settings.html

var EpochRente;
var Inzidenz = 0;
var County = "";
var CheckVal = false;
presentHost();


document.addEventListener("DOMContentLoaded", function(event) { 
getJSON(); 
});

function presentHost() {
	var host = window.location.hostname;
	//console.log(host);
}

function sendReboot() {
	var xhttp = new XMLHttpRequest();
    xhttp.open("POST", "/rebootHost", true);
    xhttp.setRequestHeader("Content-type", "application/text");
    xhttp.send("boot!");
}

function sendInit() {
	var xhttp = new XMLHttpRequest();
    if (confirm('Sind Sie sicher, dass Ihre Einstellungen gelöscht werden sollen?')) {
        xhttp.open("POST", "/initEEPROM", true);
        xhttp.setRequestHeader("Content-type", "application/text");
        xhttp.send("init!");
    }
    }

function getJSON() {	// Variablen aus EEPROM laden
  var xhttp = new XMLHttpRequest();

  xhttp.onreadystatechange = function() {
	if (this.readyState == 4 && this.status == 200) {
		console.log(this.responseText);
		var ESPObj = JSON.parse(this.responseText);
		document.getElementById("vorname").value = ESPObj.vorname;
		document.getElementById("nachname").value = ESPObj.nachname;
		document.getElementById("bright").value = ESPObj.bright;
		document.getElementById("compile").innerHTML = ESPObj.compile;
		document.getElementById("baseFile").innerHTML = ESPObj.baseFile;
		
		if (ESPObj.foermlich == 1)
			document.getElementById("foermlich").checked = true;
		else
			document.getElementById("foermlich").checked = false;
		if (ESPObj.blink == 1)
			document.getElementById("blink").checked = true;
		else
			document.getElementById("blink").checked = false;			
			
		if (ESPObj.showIP == 1)
			document.getElementById("ipaddress").checked = true;
		else
			document.getElementById("ipaddress").checked = false;

		if (ESPObj.geschlecht == 1) {	// 1 = weiblich, kein it
			document.getElementById("female").checked = true;
			document.getElementById("male").checked = false;
		}
		else {
			document.getElementById("female").checked = false;
			document.getElementById("male").checked = true;
		}
		
		EpochRente = ESPObj.rente;
		MakeRenteDatum(ESPObj.rente);
	}
  };
  xhttp.open("GET", "readJSON", true);
  xhttp.send();
}

function sendJSON() {
    console.log("Daten speichern");
	var url;
	var xhttp = new XMLHttpRequest();
	url = window.location.hostname;
	var TempRente = 0;
	
	var sendObj = {
		vorname: "",
		nachname: "",
		foermlich: 99,
		geschlecht: 99,
		rente: 0
	};
	
	sendObj.vorname = document.getElementById("vorname").value;
	sendObj.nachname = document.getElementById("nachname").value;
	sendObj.bright = document.getElementById("bright").value;

	if (document.getElementById("foermlich").checked == true)
		sendObj.foermlich = 1;
	else
		sendObj.foermlich = 0;
	
	if (document.getElementById("blink").checked == true)
		sendObj.blink = 1;
	else
		sendObj.blink = 0;
	
	if (document.getElementById("ipaddress").checked == true)
		sendObj.showIP = 1;
	else
		sendObj.showIP = 0;
	
	if (document.getElementById("female").checked == true)
		sendObj.geschlecht = 1;
	else
		sendObj.geschlecht = 0;
	
	TempRente = new Date((document.getElementById("renteInput").value)).getTime();
	sendObj.rente = TempRente/1000;
	
	setTimeout(function(){
		//if(CheckVal == true) {
			var sendDataJSON = JSON.stringify(sendObj);
			console.log(sendDataJSON);
			xhttp.onreadystatechange = function() {
			if (this.readyState == 4 && this.status == 200) {
				console.log(this.responseText);
				}
			else {
				console.log("Respone Error: " + this.responseText);
				}
			};
			xhttp.open("POST", "/getJSON", true);
			xhttp.setRequestHeader("Content-type", "application/text");
			xhttp.send(sendDataJSON);
		//}
	}, 250); 
}

function MakeRenteDatum(TimeIn){
	var jsEpoch, MonthEpoch;
	var RenteString;
	jsEpoch = new Date(TimeIn * 1000);
	console.log(jsEpoch);
	RenteString = jsEpoch.getFullYear()+ "-";
	console.log("Jahr" + RenteString);
	MonthEpoch = jsEpoch.getMonth() + 1;	//Korrektur wegen Javascript Monat (0..11)
	if ( MonthEpoch < 10) {
		RenteString += "0";
		}
	RenteString += MonthEpoch + "-";
	console.log("Monat " + RenteString);
	if (jsEpoch.getDate() < 10) {
		RenteString += "0";
		}
	RenteString += jsEpoch.getDate();
	console.log(RenteString);
	document.getElementById("renteInput").value = RenteString;
   }

 
 