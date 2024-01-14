#include <Arduino.h>
#include <SPI.h>
#include <ESP32Time.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "IEEE_5G";
const char* password = "Ilovesolder";
String serverName = "https://ideahacks-server-2024-2debc52e533c.herokuapp.com/get_diagnosis";
const int AUDIO_IN = 35;
const int THRESHOLD = 2000;
ESP32Time rtc(3600);
// #ifndef getBit
// #define getBit(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
// #endif
bool is_recording, is_done_recording;

bool testADCsimple(void)
{
    int i = analogRead(AUDIO_IN);
    if (i > THRESHOLD)
        is_recording = true;
    if (is_recording)
        Serial.println(i);
    return i > THRESHOLD;
}

void setup()
{
    Serial.begin(9600);

    WiFi.begin(ssid, password);
    Serial.println("Connecting");

    while (WiFi.status() != WL_Connected) {
        delay(500);
        Serial.print(".");
    }  
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    analogRead(AUDIO_IN);
    rtc.setTime(1, 0, 0, 1, 1, 2023);
    is_recording = false;
    is_done_recording = false;
}

void loop()
{
    if (is_done_recording)
        return;
    if (testADCsimple())
    {
        rtc.setTime(0, 0, 0, 1, 1, 2023);
        return;
    }

    // if !above_threshold
    if (rtc.getSecond() == 0)
    {
        // Set to 1 second
        rtc.setTime(1, 0, 0, 1, 1, 2023);
        return;
    }
    if (rtc.getSecond() >= 4)
    {
        // Send request
        Serial.println("SEND HHTP REQUEST TO SERVER HERE");
        is_done_recording = true;
        
        if (WiFi.status() == WL_CONNECTED) {
            response = httpGETRequest(serverName);
            Serial.println(response);
            JSONVar resObject = JSON.parse(response);
            if (JSON.typeof(resObject) == "undefined") {
                Serial.println("Parsing input failed!");
                return;
            }

            Serial.print("JSON object = ");
            Serial.println(myObject);

            JSONVar keys = myObject.keys();
            Serial.print(keys[0])
            Serial.print(": " );
            Serial.println(resObject[keys[0]])
            Serial.print(keys[1])
            Serial.print(": " );
            Serial.println(resObject[keys[1]])

        } else {
            Serial.println("WiFi Disconnected");
        }
    }
}

String httpGETRequest(const char* serverName) {
    WiFiClient client;
    HTTPClient http;
        
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // If you need Node-RED/server authentication, insert user and password below
    //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
    
    // Send HTTP POST request
    int httpResponseCode = http.GET();
    
    String payload = "{}"; 
    
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    return payload;
}
