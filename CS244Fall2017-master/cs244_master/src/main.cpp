#include "SparkFunLIS3DH.h"
#include "Secrets.h"
#include "MAX30105.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

LIS3DH myIMU; // constructed with parameters for SPI and cs pin number
MAX30105 particleSensor;
int counter = 0;
unsigned long startTime;
const int CHUNK_SIZE = 50; //50 samples / sec * 60 sec = 3000 samples = 3kb
String data[CHUNK_SIZE];

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
    //Serial.print("[HTTP] begin...\n");
    http.begin("http://13.57.112.90/sensor.php/"); //HTTP
    //http.begin("http://192.168.43.224/~ryansun1/embed/sensor.php/"); //HTTP

    
    
    digitalWrite(LED_BUILTIN, LOW);
    //Serial.print("[HTTP] POST...\n");

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    //Serial.println(postMesg);        
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

void read_sensor(float * x, float * y, float * z, int * IR, int * RED) {
    *x = myIMU.readFloatAccelX();
    *y = myIMU.readFloatAccelY();
    *z = myIMU.readFloatAccelZ();
    *IR = particleSensor.getIR();
    *RED = particleSensor.getRed();
}

void print_sensor(float & x, float & y, float & z, int & IR, int & RED) {
    Serial.print("\nAccelerometer:\n");
    Serial.print(" X = ");
    Serial.println(x, 4);
    Serial.print(" Y = ");
    Serial.println(y, 4);
    Serial.print(" Z = ");
    Serial.println(z, 4);
    Serial.print("\nPPG:\n");
    Serial.print(" IR = ");
    Serial.println(IR, 4);
    Serial.print(" RED = ");
    Serial.println(RED, 4);
}

void startPPGSensor()
{
    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
        Serial.println("MAX30105 was not found. Please check wiring/power. ");
        while (1);
    }
    
    particleSensor.setup(); //Configure sensor. Use 6.4mA for LED drive
    particleSensor.setSampleRate(50);  //set freq = 50
    particleSensor.setFIFOAverage(0x10);
}

String to_JSON_data(String data[], int len) {
    String s;
    s = "data={\"sensor\":[";
    for(int i = 0; i < len-1; i++){
        s += data[i] + ",";
    }
    s += data[len-1] + "]}";
    return s;
}

String to_JSON_array(float & x, float & y, float & z, int & IR, int & RED){
    String s = "[" + String(x,'\005') + "," +String(y, '\005') + "," + String(z, '\005') + "," + String(IR) + "," + String(RED) + "]";   //somehow, it won't include the digit under 0.01 when converting float to string
    //Serial.println(s);
    return s;
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    delay(1000); //relax...
    Serial.println("Processor came out of reset.\n");
    digitalWrite(LED_BUILTIN, LOW);
    //Call .begin() to configure the IMU

    connectWifi();
    myIMU.begin();
    startPPGSensor();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    startTime = millis();
}


void loop()
{
     //Get all parameters
    float x, y, z;
    int IR, RED;
    for(int i = 0; i < CHUNK_SIZE; i++){
        delay(10);
        read_sensor(&x,&y,&z,&IR,&RED);
        //print_sensor(x,y,z,IR,RED);
        data[i] = to_JSON_array(x, y, z, IR, RED);
        Serial.println(counter);
        counter++;
    }
    String s = to_JSON_data(data, CHUNK_SIZE);
    postServer(s);
    //delay(500);
    if((millis() - startTime) > 10*60*1000)   //set 10 minutes
    {
        digitalWrite(LED_BUILTIN, LOW);
        //uint32_t free = system_get_free_heap_size();      //see the remaining size of heap
        delay(500000000);
    }
        
}
