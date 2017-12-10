/*

 *    API_3  THIS IS WORKING !!!!
 */

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "my_wifi.h"
#include "my_eeprom.h"



static const char* ssid     = "neener";
static const char* password = "neener";

static const char* host = "api.usno.navy.mil";
const char* streamId   = "oneday";
const char* privateKey = "12/6/2017";
const char* value = "Los%20Angeles,%20CA";

String nothing = "Failed";

static uint32_t last_wifi; 
static bool updateMoon();
static bool updateRiseSet();


// ============= Setup =================
String my_wifi() 
{
  delay(10);
  last_wifi = 0;

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);


  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) 
  {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.subnetMask());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.RSSI());
  Serial.println(WiFi.channel());


//====== Now we conencted to the Internet via my router ============

  delay(1);

  Serial.print("connecting to ");
  Serial.println(host);


  Serial.print("test update moon call ");
  updateRiseSet();
  updateMoon();
  

  return nothing; 
  
}
//================================================================
// http://api.usno.navy.mil/rstt/oneday?date=12/11/2017&loc=Los%20Angeles,%20CA
static bool updateRiseSet()
{
  const char* host = "api.usno.navy.mil";
  const char* oneday   = "oneday";
  const char* thedate = "12/11/2017";
  const char* location = "Los%20Angeles,%20CA";

  delay(1);    // why is this needed? 

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient rsst_client;
  const int httpPort = 80;
  if (!rsst_client.connect(host, httpPort)) 
  {
    Serial.println("connection failed");
    return nothing;
  }

  
  // We now create a URI for the request
  String url = "/rstt/";    // the service 
  url += oneday;
  url += "?date=";
  url += thedate;
  url += "&loc=";
  url += location;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  rsst_client.print(String("GET ") + url + " HTTP/1.0\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (rsst_client.available() == 0) 
  {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      rsst_client.stop();
      return nothing;
    }
  }

  
  String line;

  
  // Read all the lines of the reply from server and print them to Serial
  while(rsst_client.available())
  {
    //String line = client.readStringUntil('\r');
    line = rsst_client.readStringUntil('\r');
  }
  //      Serial.print(line);
  char endOfHeaders[] = "\r\n\r\n";
  rsst_client.find(endOfHeaders);

  const size_t BUFFER_SIZE = JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(5) + 8*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(19) + 570;
  DynamicJsonBuffer jsonBuffer(BUFFER_SIZE);


  JsonObject& root = jsonBuffer.parseObject(line);
    if (!root.success()) 
    {
      String nogood = "Failed";
      Serial.println("parseObject() failed");
      Serial.println(jsonBuffer.size());
      return nothing;
    }
 
  JsonArray& sundata = root["sundata"];
  String sundata1_time = sundata[1]["time"]; // "6:49 a.m. ST"
  String sundata3_time = sundata[3]["time"]; // "4:45 p.m. ST"

  JsonArray& moondata = root["moondata"];
  String moondata0_time = moondata[0]["time"]; 
  String moondata2_time = moondata[2]["time"];



  Serial.println(sundata1_time);
  Serial.println(sundata1_time.length());



  my_EE_write(EE_SUNRISE, sundata1_time, sundata1_time.length());
  my_EE_write(EE_SUNSET, sundata3_time, sundata3_time.length());

  my_EE_write(EE_MOONRISE, moondata0_time, moondata0_time.length());
  my_EE_write(EE_MOONSET, moondata2_time, moondata2_time.length());


  
  return true;  

/**********************************************************************
bool error = root["error"]; // false
const char* apiversion = root["apiversion"]; // "2.0.0"
int year = root["year"]; // 2017
int month = root["month"]; // 12
int day = root["day"]; // 11
const char* dayofweek = root["dayofweek"]; // "Monday"
bool datechanged = root["datechanged"]; // false
const char* state = root["state"]; // "CA"
const char* city = root["city"]; // "Los Angeles"
float lon = root["lon"]; // -118.37
float lat = root["lat"]; // 34.08
const char* county = root["county"]; // "Los Angeles"
int tz = root["tz"]; // -8
const char* isdst = root["isdst"]; // "no"

JsonArray& sundata = root["sundata"];

const char* sundata0_phen = sundata[0]["phen"]; // "BC"
const char* sundata0_time = sundata[0]["time"]; // "6:22 a.m. ST"

const char* sundata1_phen = sundata[1]["phen"]; // "R"
const char* sundata1_time = sundata[1]["time"]; // "6:49 a.m. ST"

const char* sundata2_phen = sundata[2]["phen"]; // "U"
const char* sundata2_time = sundata[2]["time"]; // "11:47 a.m. ST"

const char* sundata3_phen = sundata[3]["phen"]; // "S"
const char* sundata3_time = sundata[3]["time"]; // "4:45 p.m. ST"

JsonArray& moondata = root["moondata"];

const char* moondata0_phen = moondata[0]["phen"]; // "R"
const char* moondata0_time = moondata[0]["time"]; // "12:39 a.m. ST"

const char* moondata1_phen = moondata[1]["phen"]; // "U"
const char* moondata1_time = moondata[1]["time"]; // "6:57 a.m. ST"

const char* moondata2_phen = moondata[2]["phen"]; // "S"
const char* moondata2_time = moondata[2]["time"]; // "1:09 p.m. ST"

JsonObject& closestphase = root["closestphase"];
const char* closestphase_phase = closestphase["phase"]; // "Last Quarter"
const char* closestphase_date = closestphase["date"]; // "December 9, 2017"
const char* closestphase_time = closestphase["time"]; // "11:51 p.m. ST"

const char* fracillum = root["fracillum"]; // "35%"
const char* curphase = root["curphase"]; // "Waning Cresc
**********************************************/
  
}


//================================
//http://api.usno.navy.mil/moon/phase?date=12/11/2017&nump=4
static bool updateMoon()
{
  const char* host = "api.usno.navy.mil";
  const char* oneday   = "phase";
  const char* thedate = "12/6/2017";
  //const char* location = "Los%20Angeles,%20CA";
  const char* numbers ="4";

  delay(1);    // why is this needed? 

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient phase_client;
  const int httpPort = 80;
  if (!phase_client.connect(host, httpPort)) 
  {
    Serial.println("connection failed");
    return nothing;
  }

  
  // We now create a URI for the request
  String url = "/moon/phase";    // the service 
  //url += oneday;
  url += "?date=";
  url += thedate;
  url += "&nump=";
  url += numbers;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  phase_client.print(String("GET ") + url + " HTTP/1.0\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (phase_client.available() == 0) 
  {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      phase_client.stop();
      return nothing;
    }
  }

  
  String line;

  
  // Read all the lines of the reply from server and print them to Serial
  while(phase_client.available())
  {
    //String line = client.readStringUntil('\r');
    line = phase_client.readStringUntil('\r');
  }
  //        Serial.print(line);
  char endOfHeaders[] = "\r\n\r\n";
  phase_client.find(endOfHeaders);


  const size_t BUFFER_SIZE = JSON_ARRAY_SIZE(4) + 4*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 310;
  DynamicJsonBuffer jsonBuffer(BUFFER_SIZE);

  JsonObject& root = jsonBuffer.parseObject(line);
    if (!root.success()) 
    {
      String nogood = "Failed";
      Serial.println("parseObject() failed");
      Serial.println(jsonBuffer.size());
      return nothing;
    }

  JsonArray& phasedata = root["phasedata"];

  JsonObject& phasedata0 = phasedata[0];
  String phasedata0_phase = phasedata0["phase"]; // "New Moon"
  String phasedata0_date = phasedata0["date"]; // "2017 Dec 18"
  my_EE_write(EE_MOONNEW, phasedata0_date, phasedata0_date.length());
  String phasedata0_time = phasedata0["time"]; // "06:30"

  JsonObject& phasedata1 = phasedata[1];
  String phasedata1_phase = phasedata1["phase"]; // "First Quarter"
  String phasedata1_date = phasedata1["date"]; // "2017 Dec 26"
  my_EE_write(EE_MOONFIRST, phasedata1_date, phasedata1_date.length());
  String phasedata1_time = phasedata1["time"]; // "09:20"

  JsonObject& phasedata2 = phasedata[2];
  String phasedata2_phase = phasedata2["phase"]; // "Full Moon"
  String phasedata2_date = phasedata2["date"]; // "2018 Jan 02"
  my_EE_write(EE_MOONFULL, phasedata2_date, phasedata2_date.length());
  String phasedata2_time = phasedata2["time"]; // "02:24"

  JsonObject& phasedata3 = phasedata[3];
  String phasedata3_phase = phasedata3["phase"]; // "Last Quarter"
  String phasedata3_date = phasedata3["date"]; // "2018 Jan 08"
  my_EE_write(EE_MOONLAST, phasedata3_date, phasedata3_date.length());
  String phasedata3_time = phasedata3["time"]; // "22:25"

  
  return true;  
/***********************************************************************
  
bool error = root["error"]; // false
const char* apiversion = root["apiversion"]; // "2.0.0"
int year = root["year"]; // 2017
int month = root["month"]; // 12
int day = root["day"]; // 11
int numphases = root["numphases"]; // 4
bool datechanged = root["datechanged"]; // false

JsonArray& phasedata = root["phasedata"];

JsonObject& phasedata0 = phasedata[0];
const char* phasedata0_phase = phasedata0["phase"]; // "New Moon"
const char* phasedata0_date = phasedata0["date"]; // "2017 Dec 18"
const char* phasedata0_time = phasedata0["time"]; // "06:30"

JsonObject& phasedata1 = phasedata[1];
const char* phasedata1_phase = phasedata1["phase"]; // "First Quarter"
const char* phasedata1_date = phasedata1["date"]; // "2017 Dec 26"
const char* phasedata1_time = phasedata1["time"]; // "09:20"

JsonObject& phasedata2 = phasedata[2];
const char* phasedata2_phase = phasedata2["phase"]; // "Full Moon"
const char* phasedata2_date = phasedata2["date"]; // "2018 Jan 02"
const char* phasedata2_time = phasedata2["time"]; // "02:24"

JsonObject& phasedata3 = phasedata[3];
const char* phasedata3_phase = phasedata3["phase"]; // "Last Quarter"
const char* phasedata3_date = phasedata3["date"]; // "2018 Jan 08"
const char* phasedata3_time = phasedata3["time"]; // "22:25"

******************************************************************/


}




