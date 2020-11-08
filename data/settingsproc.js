// Die Javascript-Datei für die settings.html

var EpochRente;
presentHost();


document.addEventListener("DOMContentLoaded", function(event) { 
getJSON(); 
//sendJSON(); 
});

function presentHost() {
	var host = window.location.hostname;
	console.log(host);
}


function getJSON() {	// Variablen aus EEPROM laden
  var xhttp = new XMLHttpRequest();

  xhttp.onreadystatechange = function() {
	if (this.readyState == 4 && this.status == 200) {
		var ESPObj = JSON.parse(this.responseText);
		document.getElementById("vorname").value = ESPObj.vorname;
		document.getElementById("nachname").value = ESPObj.nachname;
		document.getElementById("bright").value = ESPObj.bright;
		if (ESPObj.foermlich == 1)
			document.getElementById("foermlich").checked = true;
		else
			document.getElementById("foermlich").checked = false;
			
		if (ESPObj.blink == 1)
			document.getElementById("blink").checked = true;
		else
			document.getElementById("blink").checked = false;
			
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
	
	if (document.getElementById("female").checked == true)
		sendObj.geschlecht = 1;
	else
		sendObj.geschlecht = 0;
	
	TempRente = new Date((document.getElementById("renteInput").value)).getTime();
	console.log(TempRente);
	sendObj.rente = TempRente/1000;
	console.log(sendObj.rente);

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
 //style="font-size:20px">
