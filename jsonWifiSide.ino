/*
   Author: Ramazan Burak Korkmaz

   Basic explanation: NetCar needs to know location of nodes. This code is responsible from getting and sending
   data by web services. If flag equals to 1, it means car needs location to start test. If flag equals 2, it means the car
   finished its tour and wants to send the data collected.
*/

#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>


// WIFI ID and Password to connect it.
const char* ssid = "";
const char* password =  "";

// Variables
String load, finalString = "";
int flag = 0, lastFlag = 0;

// From web services, we get JSON documents. To catch it, we must set catching JSON doc's size right.
const int capacity = JSON_ARRAY_SIZE(20) + 20 * JSON_OBJECT_SIZE(10);
StaticJsonDocument<capacity> doc;


void setup() {
  Serial.begin(115200);
  Serial2.begin(4800);
  while (!Serial2) continue;

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  // Getting flag variable from arduino side. That will determine which step we want to launch.
  lastFlag = flag;
  while (Serial2.available() > 0) {
    flag = Serial2.readStringUntil('\n').toInt();
  }
  if (lastFlag != flag) {
    Serial.println("Flag: " + String(flag));
  }


  // Switch statement to control algortihm. Flag variable determines steps.
  // case 1 --> Getting node information from web service.
  // case 2 --> Sending node information to web service
  switch (flag) {
    case 1:
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        http.begin("https://mysterious-tor-65968.herokuapp.com/plan");

        // Specify content-type header
        http.addHeader("Content-Type", "application/json");
        // Send HTTP POST request
        int httpResponseCode = http.POST("{\"method\":\"szone\",\"tr\":5,\"car_id\":1,\"mcount\":2,\"json\" : 1,\"c_list\":[{\"lat\":36.991847,\"lon\":30.613351}, {\"lat\":36.991920,\"lon\":30.613340}, {\"lat\":36.991920,\"lon\":30.613306}, {\"lat\":36.991851,\"lon\":30.613354}, {\"lat\":36.991775,\"lon\":30.613231}, {\"lat\":36.991725,\"lon\":30.613178}, {\"lat\":36.991733,\"lon\":30.613103}, {\"lat\":36.991858,\"lon\":30.613100}, {\"lat\":36.991874,\"lon\":30.613132}, {\"lat\":36.991901,\"lon\":30.613063}]}");
        //int httpResponseCode = http.POST("{\"method\":\"szp\",\"tr\":5,\"car_id\":1,\"mcount\":1,\"json\" : 1,\"c_list\":[{\"lat\":36.991847,\"lon\":30.613351}, {\"lat\":36.991920,\"lon\":30.613340}, {\"lat\":36.991920,\"lon\":30.613306}, {\"lat\":36.991851,\"lon\":30.613354}, {\"lat\":36.991775,\"lon\":30.613231}, {\"lat\":36.991725,\"lon\":30.613178}, {\"lat\":36.991733,\"lon\":30.613103}, {\"lat\":36.991858,\"lon\":30.613100}, {\"lat\":36.991874,\"lon\":30.613132}, {\"lat\":36.991901,\"lon\":30.613063}]}");
        //int httpResponseCode = http.POST("{\"method\":\"cc\",\"tr\":5,\"car_id\":1,\"mcount\":1,\"json\" : 1,\"c_list\":[{\"lat\":36.991847,\"lon\":30.613351}, {\"lat\":36.991920,\"lon\":30.613340}, {\"lat\":36.991920,\"lon\":30.613306}, {\"lat\":36.991851,\"lon\":30.613354}, {\"lat\":36.991775,\"lon\":30.613231}, {\"lat\":36.991725,\"lon\":30.613178}, {\"lat\":36.991733,\"lon\":30.613103}, {\"lat\":36.991858,\"lon\":30.613100}, {\"lat\":36.991874,\"lon\":30.613132}, {\"lat\":36.991901,\"lon\":30.613063}]}");
        //6:34:06.783 -> 10   92   36.991733  30.613397   687  07/30/2020 13:34:06 801  292.40 24.51  0.04  WNW   18       186.89 S     35850 138       0        

        Serial.println(httpResponseCode);

        if (httpResponseCode > 0) {
          load = http.getString();
          deserializeJson(doc, load);

          serializeJson(doc, Serial2);
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
        http.end();
      }
      else {
        Serial.println("WiFi Disconnected");
      }
      flag = 0;
      break;
    case 2:
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;


        http.begin("http://rosenet.herokuapp.com/robot/post");

        // Specify content-type header
        http.addHeader("Content-Type", "application/json");

        Serial.println("D1");
        Serial2.println("3");
        Serial2.flush();

        const int kapasite = JSON_ARRAY_SIZE(20) + 20 * JSON_OBJECT_SIZE(10);
        StaticJsonDocument<kapasite> comming;

        Serial.println("Serial2: " + String(Serial2.available()));
        //while(Serial2.available() <= 0 ) continue;

        while (Serial2.available() == 0) {

          DeserializationError err = deserializeJson(comming, Serial2);
          Serial.print("Err: ");
          Serial.println(err.c_str());
          if (err == DeserializationError::Ok)
          {

            serializeJsonPretty(comming, Serial);

            String requestBody;
            serializeJson(comming, requestBody);
            int httpResponseCode = http.POST(requestBody);
            Serial.println(httpResponseCode);

            http.end();

          }
        }
      }
      else {
        Serial.println("WiFi Disconnected");
      }
      flag = 0;
      break;
  }
}
void merge(JsonVariant dst, JsonVariantConst src)
{
  if (src.is<JsonObject>())
  {
    for (auto kvp : src.as<JsonObject>())
    {
      merge(dst.getOrAddMember(kvp.key()), kvp.value());
    }
  }
  else
  {
    dst.set(src);
  }
}
