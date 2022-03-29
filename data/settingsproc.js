// Die Javascript-Datei für die settings.html

var EpochRente;
var Inzidenz = 0;
var County = "";
var CheckVal = false;
presentHost();


document.addEventListener("DOMContentLoaded", function(event) { 
getJSON(); 
//sendJSON(); 
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

function AGSTest(NewAGS) {
	CheckURL(NewAGS);
	setTimeout(function(){
		if( CheckVal == true) {
			document.getElementById("checkCounty").innerHTML = County;
			console.log(County, Inzidenz);
			}
		else {
			if (NewAGS.length != 5)
				alert("Falsche AGS-Länge.\nBitte genau fünf Ziffern.\nVier ist zu kurz, sechs ist zu lang!\n");
			else
				alert("Unbekannte AGS\nFür AGS-Suche bitte auf \"AGS finden\" klicken");
			}
	}, 500); 
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
		document.getElementById("ags").value = ESPObj.ags;
		document.getElementById("compile").innerHTML = ESPObj.compile;
		document.getElementById("baseFile").innerHTML = ESPObj.baseFile;
		
		if (ESPObj.foermlich == 1)
			document.getElementById("foermlich").checked = true;
		else
			document.getElementById("foermlich").checked = false;
			
		if (ESPObj.showInz == 1) {
			document.getElementById("showInz").checked = true;
		}
		else {
			document.getElementById("showInz").checked = false;
		}
		
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
	var url;
	var xhttp = new XMLHttpRequest();
	url = window.location.hostname;
	var TempRente = 0;
	
	var sendObj = {
		vorname: "",
		nachname: "",
		foermlich: 99,
		geschlecht: 99,
		rente: 0,
		showInz: 99,
		ags: ""
	};
	
	sendObj.vorname = document.getElementById("vorname").value;
	sendObj.nachname = document.getElementById("nachname").value;
	sendObj.bright = document.getElementById("bright").value;
	sendObj.ags = document.getElementById("ags").value;

	if (document.getElementById("foermlich").checked == true)
		sendObj.foermlich = 1;
	else
		sendObj.foermlich = 0;

	if (document.getElementById("showInz").checked == true)
		sendObj.showInz = 1;
	else
		sendObj.showInz = 0;
	
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
	
	AGSTest(sendObj.ags);
	setTimeout(function(){
		if(CheckVal == true) {
			var sendDataJSON = JSON.stringify(sendObj);
			//console.log(sendDataJSON);
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

function CheckURL(ThisAGS) {
	var apiURL1 = "https://services7.arcgis.com/mOBPykOjAyBO2ZKk/arcgis/rest/services/RKI_Landkreisdaten/FeatureServer/0/query?where=AGS %3D%27";
	var apiURL2 = "%27&outFields=county,last_update,cases7_per_100k&returnGeometry=false&outSR=4326&f=json";
	var RetVal;
	//var AGS = "14612";
	var AGS = "04012";
	console.log(ThisAGS);
	var apiURL = apiURL1 + ThisAGS + apiURL2;
	console.log(apiURL);

	//const url = apiURL;
	if(ThisAGS.length == 5)
		fetch(apiURL)
		.then(data => data.json())
		.then((json) => {
			try {
			Inzidenz = json["features"][0]["attributes"]["cases7_per_100k"];
			County = json["features"][0]["attributes"]["county"];
			CheckVal = true;
			}
			catch {
			Inzidenz = 0;
			County = "Nix"
			CheckVal = false;
			}
		});
	else
		CheckVal = false;
}

 
 