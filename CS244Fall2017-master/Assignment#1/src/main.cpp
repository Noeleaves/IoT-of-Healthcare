#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

String deviceName = "CS244";

// WiFi settings
const char *ssid = "UCInet Mobile Access";

void printMacAddress()
{
    byte mac[6];
    WiFi.macAddress(mac);

    char MAC_char[18]="";
    for (int i = 0; i < sizeof(mac); ++i)
    {
        sprintf(MAC_char, "%s%02x:", MAC_char, mac[i]);
    }

    //Serial.println("print mac address");
    Serial.println(MAC_char);
}   
void connectWifi()
{
    WiFi.begin(ssid); //, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    // Print the IP address
    Serial.println(WiFi.localIP());
    
}

void connectServer()
{       
    if (WiFi.status() != WL_CONNECTED){
        Serial.println("WIFI Error");
        return;
    }
    
        HTTPClient http;

        Serial.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin("http://13.57.112.90/post.php/"); //HTTP
        
        //Serial.println(http.connected());
        
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("[HTTP] POST...\n");
        // start connection and send HTTP header

        //http.header()
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");        
        int httpCode = http.POST("name=Hello from ESP8266!"); 

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
        digitalWrite(LED_BUILTIN, HIGH);
}

void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
    
  Serial.begin(115200);
  Serial.println("Program started");
  connectWifi();
}

void loop()
{
  // You can write your logic here

    connectServer();
    
  // Below are the LED samples from the hands-on

  // turn the LED on (HIGH is the voltage level)
  //digitalWrite(LED_BUILTIN, HIGH);
  // wait for a second
  //delay(2000);
  // turn the LED off by making the voltage LOW

  //digitalWrite(LED_BUILTIN, LOW);
   // wait for a second
  delay(10000);
}

