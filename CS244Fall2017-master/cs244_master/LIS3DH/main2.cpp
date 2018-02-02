#include "SparkFunLIS3DH.h"
#include "Wire.h"
#include "SPI.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

LIS3DH myIMU; //Default constructor is I2C, addr 0x19.
const char *ssid = "UCInet Mobile Access";

void connectWifi()
{
    WiFi.begin(ssid);//, wifipasswd);
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

void postServer(String postMesg)
{
    if (WiFi.status() != WL_CONNECTED){
        Serial.println("WIFI Error");
        return;
    }
        HTTPClient http;
        Serial.print("[HTTP] begin...\n");
        http.begin("http://13.57.112.90/LIS3DHCSV.php/"); //HTTP
        
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("[HTTP] POST...\n");

        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        Serial.println(postMesg);        
        int httpCode = http.POST(postMesg); 

        if(httpCode > 0) {
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);
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

void read_acc(float * x, float * y, float * z) {
  *x = myIMU.readFloatAccelX();
  *y = myIMU.readFloatAccelY();
  *z = myIMU.readFloatAccelZ();
  
}

void print_acc(float & x, float & y, float & z) {
  Serial.print("\nAccelerometer:\n");
  Serial.print(" X = ");
  Serial.println(x, 4);
  Serial.print(" Y = ");
  Serial.println(y, 4);
  Serial.print(" Z = ");
  Serial.println(z, 4);
}

String to_JSON_data(String data[], int len) {
  String s = "data={\"acceleration\":[";
  for(int i = 0; i < len-1; i++){
    s += data[i] + ",";
  }
  s += data[len-1];
  s += "]}";
  return s;
}

String to_JSON_array(float & x, float & y, float & z){
  String s = "[" + String(x) + "," +String(y) + "," + String(z) + "]";
  //Serial.println(s);
  return s;
  //[1,2,3]}";
}
  //data={\"acceleration\"=[1,2,3]}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000); //relax...
  Serial.println("Processor came out of reset.\n");
  digitalWrite(LED_BUILTIN, HIGH);
  //Call .begin() to configure the IMU
  
  myIMU.begin();
  connectWifi();
  digitalWrite(LED_BUILTIN, LOW);
}


void loop()
{
  
 //Get all parameters
int CHUNK_SIZE = 10;
//float x[CHUNK_SIZE],y[CHUNK_SIZE],z[CHUNK_SIZE];
float x, y, z;
String data[CHUNK_SIZE];
//read_acc(&x,&y,&z);
//print_acc(x,y,z);
//delay(1000);

for(int i = 0; i < CHUNK_SIZE; i++){
  read_acc(&x,&y,&z);
  data[i] = to_JSON_array(x, y, z);
}
String s = to_JSON_data(data, CHUNK_SIZE);
postServer(s);
delay(1000);


}