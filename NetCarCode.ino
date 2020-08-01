/*
  Author: Ramazan Burak Korkmaz

  Basic explanation: NetCar is a autonomous mobil robot that can collect sensor datas and measure its own energy consumption
  while moving between nodes. The program is coded to work under 4 step. At first step, the car gets its needed information via
  wifi and web service we wrote. After getting information from web service, the car goes to locations that are writen in JSON
  file. At each location, the car collects sensor datas, GPS location etc. After all location, the car stops and sends all data
  collected via another web service. With that web service, we graph the datas and analyse them.

*/
#define ARDUINOJSON_USE_DOUBLE 1
// LIBRARY PART
#include <TinyGPS++.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <ArduinoJson.h>
#include <QMC5883LCompass.h>

// OBJECT CREATING PART
TinyGPSPlus gps;
Adafruit_INA219 ina219;
QMC5883LCompass compass;

// Determining JSON file size
const int capacity = JSON_ARRAY_SIZE(20) + 20 * JSON_OBJECT_SIZE(10);
const int cap = JSON_ARRAY_SIZE(10) + 10 * JSON_OBJECT_SIZE(10);

// Creating a JSON file and an array
StaticJsonDocument<capacity> nodeInfo;
StaticJsonDocument<1500> firstInfo;
//JsonArray infos = nodeInfo.createNestedArray();



// VARIABLES

int state = 0;  // State that determines step of program
bool firstTime = false; // To check first time take of time.
unsigned long start;  // Start time variable.
const int robot_id = 1; // Robot id. Changes for each robot.

int maxLocNumber = 0;

float power_mW = 0;     //INA219 POWER VARIABLE

int ac = 0;

int Number_of_SATS;
unsigned long Distance_To_Home;
int GPS_Course;


float heading = 0;
float headingDegrees = 0;
int desired_heading;

void setup() {

  // Serial com. begin
  Serial.begin(115200);
  Serial1.begin(9600);
  Serial2.begin(4800);
  Serial3.begin(4800);


  while (!Serial3) continue;
  state = 1;

  // TIMER PART
  cli();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 2hz increments
  OCR1A = 3906;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();

  compass.init();
  compass.setCalibration(-1157, 1803, -3422, 0, -4736, 846);

  // INA219 PART
  ina219.begin();

  // FIRST STOP
  stop_Robot();

  Serial.println("Starting....");

  // Set firstTime bool to get first take of time.
  firstTime = true;



}

void loop() {

  // Printing state value to debug
  Serial.println("State: " + String(state));

  // Switch case for part of algoritm.
  // case 0 --> Last part. Car stops and waits.
  // case 1 --> First part. We get node informations from wifi side
  // case 2 --> Second part. Car goes to selected node location and collects data.
  // case 3 --> Third part. After all nodes, car sends the data it collected.
  switch (state) {
    case 0: // After cycle, car will stuck right here.
      StopCar();
      serializeJsonPretty(nodeInfo, Serial);
      //Serial.println("The cycle ends here");
      delay(500);
      break;
    case 1: { // First step is to take all path information.
        //Serial.println("Waiting for JSON DOC");
        Serial3.println("1");
        StaticJsonDocument<1000> coming;
        DeserializationError err;
        while (Serial3.available() > 0)
        {
          err = deserializeJson(coming, Serial3);
          if (err == DeserializationError::Ok)
          {
            serializeJsonPretty(coming, Serial);
            Serial.println("");
          }
        }
        if (!coming.isNull() && err == DeserializationError::Ok) {
          firstInfo = coming;

          maxLocNumber = coming.size();
          Serial.println("MaxLocNumber: " + String(maxLocNumber));
          Serial.println("ac: " + String(ac));
          state = 2;
        }
      }
      break;
    case 2: // Second step is to go nodes to get sensor values. After all nodes, state will be assigned as 3.
      gpsInfo();
      goWayPoint();
      //delay(150);
      break;
    case 3: // Last part is to send all sensor values that harvested from nodes by WIFI ESP32
      // Gönderme işlemi

      for (int i = 0; i < 3; i++) {
        delay(500);
        Serial3.println("2");
        Serial3.flush();
        //serializeJson(nodeInfo, Serial3);
        delay(100);

        //while (Serial3.available() <= 0) continue;

        while (Serial3.available() > 0) {
          state = Serial3.readStringUntil('\n').toInt();
          Serial.println("State: " + String(state));
          if (state == 3) {
            Serial.println("Deneme");
            serializeJsonPretty(nodeInfo, Serial);
            serializeJson(nodeInfo, Serial3);
            //Serial3.flush();
          }
        }

      }
      state = 0;
      break;
  }

}
