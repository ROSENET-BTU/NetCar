/*
	Author: Ramazan Burak Korkmaz
*/

// Encoding GPS Module
void getGPS() {
  while (Serial1.available()) {
    gps.encode(Serial1.read());
  }
}//end of getGPS

// 2Hz timer for power consumption measurements
ISR(TIMER1_COMPA_vect) {
  sei();//allow interrupts
  if (ina219.getPower_mW() >= 0) {
    power_mW += ina219.getPower_mW() / 1000.0;

    Serial.print("Power : ");
    Serial.println(power_mW);

  }
}// end of TIMER

// Reading node datas and storing them in a JSON array.
void getNodeData() {

  StaticJsonDocument<cap> bufferJSON;

  // Robot ID Part
  bufferJSON[0]["robot_id"] = robot_id;
  bufferJSON[1]["robot_id"] = robot_id;

  //Node part
  bufferJSON[0]["node"] = ac;
  bufferJSON[1]["node"] = ac;

  // Latitude Part
  bufferJSON[0]["latitude"] = gps.location.lat();
  bufferJSON[1]["latitude"] = gps.location.lat();

  //Longitude Part
  bufferJSON[0]["longitude"] = gps.location.lng();
  bufferJSON[1]["longitude"] = gps.location.lng();

  //Device Part
  bufferJSON[0]["device"] = 0;
  bufferJSON[1]["device"] = 1;

  //Humidity Part
  bufferJSON[0]["humidity"] = 0;

  //Temperature Part
  bufferJSON[1]["temperature"] = 0;

  //Time part
  bufferJSON[0]["time"] = millis() - start;
  bufferJSON[1]["time"] = millis() - start;

  // Energy part
  bufferJSON[0]["energy"] = power_mW / 4.0;
  bufferJSON[1]["energy"] = power_mW / 4.0;

  nodeInfo[ac] = bufferJSON;

}// End of getNodeData

// Gets latest compass values
void getCompass()
{
  // Read compass values
  compass.read();

  // Return Azimuth reading
  headingDegrees = compass.getAzimuth();
  headingDegrees += (5.0 + (19.0 / 60.0));

  Serial.print("Heading degrees: ");
  Serial.print(headingDegrees);
  Serial.println();
}//End of getCompass

// Main algortims for movement of NetCar to the waypoints
void goWayPoint() {

  getGPS();


  // Wait until we get other than 0 value for latitude and longitude
  if (gps.location.lat() != 0 && gps.location.lng() != 0 /* && Number_of_SATS >= 8 */) {

    // Get first time take of time.
    if (firstTime) {
      start = millis();
      Serial.println("Start time: " + String(start));
      firstTime = false;
    }
    Serial.print("Lat: ");
    Serial.println(double(firstInfo[ac]["lat"]), 6);

    Serial.print("Lon: ");
    Serial.println(double(firstInfo[ac]["lon"]), 6);

    getCompass();
    getGPS();
    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      Serial.println("No GPS data: Check wiring");
    }
    // Calculate distance to the Waypoint
    Distance_To_Home = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), firstInfo[ac]["lat"], firstInfo[ac]["lon"]);

    // Calculate course degree between NetCar and Waypoint
    GPS_Course = TinyGPSPlus::courseTo(gps.location.lat(), gps.location.lng(), firstInfo[ac]["lat"], firstInfo[ac]["lon"]);


    // If Distance is zero, that means we arrived to the Waypoint
    if (Distance_To_Home <= 1)
    {
      StopCar();
      getNodeData();
      delay(5000);
      Serial.println("You have arrived!");

      // Controlling the increasement of node number. ac won't exceed numberOfNodes
      if (ac < maxLocNumber) {
        ac++;
      }
      if (ac >= maxLocNumber) {
        state = 3;
        ac = 0;

      }
    }
    else {
      Serial.println("Distance to Home: " + String(Distance_To_Home));
      Serial.println("GPS course: " + String(GPS_Course));
    }

    // If course degree is below a value, NetCar goes straigth. If not, it adjusts its degree by turning.
    if ( abs(GPS_Course - headingDegrees) <= 20)
    {
      Serial.println("Forward");
      Forward(200);
      delay(150);
    }
    else
    {
      // Calculating degree difference
      int x = (GPS_Course - 360);
      int y = (headingDegrees - (x));
      int z = (y - 360);

      Serial.println("Z: " + String(z));

      if ((z <= 180) && (z >= 0))
      {
        Serial.println("Slow Left Turn");
        Serial.println("----------------------------------------------------------------------------");
        SlowLeftTurn(175);
        delay(150);
      }
      else
      {
        Serial.println("Slow Right Turn");
        Serial.println("----------------------------------------------------------------------------");
        SlowRightTurn(175);
        delay(150);
      }
    }
  }
}//End of goToWayPoint

// Prints gps infos for debugging and information
void gpsInfo() {


  getGPS();
  Number_of_SATS = (int)(gps.satellites.value());         //Query Tiny GPS for the number of Satellites Acquired
  Serial.println("Sat number: " + String(Number_of_SATS));
  Distance_To_Home = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), firstInfo[ac]["lat"], firstInfo[ac]["lon"]);
  Serial.println("Distance to Home: " + String(Distance_To_Home));
  Serial.print("LAT: ");
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  Serial.println("");
  Serial.print("LON: ");
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  Serial.println("");
}// gpsInfo

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

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate & d, TinyGPSTime & t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }

  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
  smartDelay(0);
}
