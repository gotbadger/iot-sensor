#include <ESP8266HTTPClient.h>

/*
  Access temprature and humidity via json request
*/

#include <DHT.h>
#include <DHT_U.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>  

//for LED status
#include <Ticker.h>
Ticker ticker;


void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server ( 80 );
DHT dht(D2, DHT22);

////-------SETUP ROUTES
void handleDHT() {
  Serial.print("sending environmental data");  
  float h = dht.readHumidity();
  float t = dht.readTemperature();  
  // Check if any reads failed for this call
  bool err =  (isnan(h) || isnan(t));
  if(err){
    server.send ( 200, "application/json", "{\"error\":true}");
  }else{
    String humidity_string = String(h, 2);
    String temp_string =  String(t, 2);
    String res = String("{\"error\": false,\"temp\": "+temp_string+",\"humidity\":"+humidity_string+"}");
    server.send ( 200, "application/json", res);
  }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );
}

//------END ROUTES

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...");
  ticker.detach();
  // Print the IP address
  Serial.println(WiFi.localIP());

  char deviceName[20];
  sprintf(deviceName,"sensor-%x",ESP.getChipId());
  Serial.print("MDNS Name: ");
  Serial.println(deviceName);
  
  if ( MDNS.begin ( deviceName ) ) {
    Serial.println ( "MDNS responder started" );
  }

  //start http server
  server.on ( "/dht", handleDHT );
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );

  // Add service to MDNS-SD
  MDNS.addService("sensor", "tcp", 80);
  //LED off when config is ok
  digitalWrite(BUILTIN_LED, HIGH);
}
//void setup() {
//  Serial.begin(115200);
//  delay(10);
//  //prep dht
//  dht.begin();
//
//  // prepare GPIO2
//  pinMode(LED_BUILTIN, OUTPUT);
//  digitalWrite(LED_BUILTIN, LOW);
//  
//  // Connect to WiFi network
//  Serial.println();
//  Serial.println();
//  Serial.print("Connecting to ");
//  Serial.println(ssid);
//  
//  WiFi.begin(ssid, password);
//  
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//  Serial.println("");
//  Serial.println("WiFi connected");
//
//  // Print the IP address
//  Serial.println(WiFi.localIP());
//
//  char deviceName[20];
//  sprintf(deviceName,"sensor-%x",ESP.getChipId());
//  Serial.print("MDNS Name: ");
//  Serial.println(deviceName);
//  
//  if ( MDNS.begin ( deviceName ) ) {
//    Serial.println ( "MDNS responder started" );
//  }
//
//  //start http server
//  server.on ( "/dht", handleDHT );
//  server.onNotFound ( handleNotFound );
//  server.begin();
//  Serial.println ( "HTTP server started" );
//
//  // Add service to MDNS-SD
//  MDNS.addService("sensor", "tcp", 80);
//
//  //indicate that startup ended
//  digitalWrite(LED_BUILTIN, HIGH);
//}

void loop ( void ) {
  server.handleClient();
}


