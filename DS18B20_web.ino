#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"
//#include <ESP8266WiFi.h>
#include <Hash.h>
#include "ESPAsyncTCP.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is connected to GPIO 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
int buzzerPin=2;
#define ledpin 16
float tempC = sensors.getTempCByIndex(0);
// Variables to store temperature values
String temperatureF = "";
String temperatureC = "";

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;

// Replace with your network credentials
const char* ssid     = "LİMANLAR";
const char* password = "16150680";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
WiFiServer server1(80);

String readDSTemperatureC() {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempCByIndex(0);

  if(tempC == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    Serial.print("Temperature Celsius: ");
    Serial.println(tempC); 
  }
  return String(tempC);
}

String readDSTemperatureF() {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  float tempF = sensors.getTempFByIndex(0);

  if(int(tempF) == -196){
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    Serial.print("Temperature Fahrenheit: ");
    Serial.println(tempF);
  }
  return String(tempF);
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .ds-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>SICAKLIK TAKIP SISTEMI</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="ds-labels">SICAKLIK</span> 
    <span id="temperaturec">%TemperatureC%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>

  

   
  </p> 
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturec").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturec", true);
  xhttp.send();
}, 10000) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturef").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturef", true);
  xhttp.send();
}, 10000) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DS18B20 values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATUREC"){
    return temperatureC;
  }
  else if(var == "TEMPERATUREF"){
    return temperatureF;
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledpin,OUTPUT);
  // Start up the DS18B20 library
  sensors.begin();

  temperatureC = readDSTemperatureC();
  temperatureF = readDSTemperatureF();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperaturec", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureC.c_str());
  });
  /*server.on("/temperaturef", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureF.c_str());
  });
  */
  // Start server
  server.begin();
}
 
void loop(){

  if ((millis() - lastTime) > timerDelay) {
    temperatureC = readDSTemperatureC();
    temperatureF = readDSTemperatureF();
    lastTime = millis();
  }  
if(tempC > 25){
  
  //melodi(tempC*100);
  tone(buzzerPin, 1000); // Send 1KHz sound signal...
  delay(5000);        // ...for 1 sec
  noTone(buzzerPin);     // Stop sound...
  delay(500); 
  digitalWrite(ledpin,HIGH);
  delay(50);
  digitalWrite(ledpin,LOW);
  delay(50);
  }
else{
   noTone(buzzerPin);
   digitalWrite(ledpin,LOW);
  
  }
}
   /* int melodi(int dly)
{
tone(buzzerPin, 440);
delay(dly);
noTone(buzzerPin);
delay(dly);
}









*/
}
