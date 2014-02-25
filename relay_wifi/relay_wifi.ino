/* 
*  Simple remote relay control with Arduino & the CC3000 chip
*  Part of the code is based on the work done by Adafruit on the CC3000 chip
*  Writtent by Marco Schwartz for Open Home Automation
*/

// Include required libraries
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <CC3000_MDNS.h>

// Define CC3000 chip pins
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
 
const int relay_pin = 8; // Relay pin
String result;

const unsigned long
  dhcpTimeout     = 60L * 1000L, // Max time to wait for address from DHCP
  connectTimeout  = 15L * 1000L, // Max time to wait for server connection
  responseTimeout = 15L * 1000L; // Max time to wait for data from server
uint32_t t;

// WiFi network (change with your settings !)
#define WLAN_SSID       "yourNetwork"       // cannot be longer than 32 characters!
#define WLAN_PASS       "yourPassword"
#define WLAN_SECURITY   WLAN_SEC_WPA2 // This can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2

// What TCP port to listen on for connections.
#define LISTEN_PORT           8888    

// Create CC3000 instances
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2);                                                                         

// Create server
Adafruit_CC3000_Server relayServer(LISTEN_PORT);

MDNSResponder mdns;

void setup() {
   
  pinMode(relay_pin,OUTPUT);
  Serial.begin(115200);
  
  // Initialise the CC3000 module
  if (!cc3000.begin())
  {
    while(1);
  }

  // Connect to  WiFi network
  cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
  Serial.println("Connected to WiFi network!");
    
  // Check DHCP
  Serial.print(F("Requesting address from DHCP server..."));
  for(t=millis(); !cc3000.checkDHCP() && ((millis() - t) < dhcpTimeout); delay(1000));
  if(cc3000.checkDHCP()) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("failed"));
    return;
  }
  
  // Start multicast DNS responder
  if (!mdns.begin("relay", cc3000)) {
    Serial.println(F("Error setting up MDNS responder!"));
    while(1); 
  }
  
   // Start listening for connections
  relayServer.begin();
  
  Serial.println(F("Listening for connections..."));

}

void loop() {
  
  // Handle any multicast DNS requests
  mdns.update();
  
  // Try to get a client which is connected.
  Adafruit_CC3000_ClientRef client = relayServer.available();
  if (client) {
    
   boolean currentLineIsBlank = true;
     // Check if there is data available to read.
     while (client.available()) {
     
       char c = client.read();
       result = result + c;
       Serial.write(c);
       
       // Delete HTTP headers
      if(result.endsWith("Content-Type: text/html"))
      {
        result="";
      }
       
       if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();          
       }
       if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
       
     }
     // give the web browser time to receive the data
    delay(5);
    // close the connection:
    client.close();
    Serial.println("client disconnected"); 
    
    result.trim();
  
   // Change relay state accordingly
   if(result.endsWith("Off"))
   {
     digitalWrite(relay_pin,LOW);
   }
      
   if(result.endsWith("On"))
   {
      digitalWrite(relay_pin,HIGH);
   }
 }

}
