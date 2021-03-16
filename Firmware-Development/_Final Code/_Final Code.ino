#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char *ssid     = "Devsol Pakistan";
const char *password = "47356880";


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 18000);

const char* serverName = "https://firestore12.herokuapp.com/sensor_info/create"; //Endpoint for data updation




void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();

}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClientSecure client;

    client.setInsecure();
    client.connect(serverName, 34441);

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(JSONPayload());
    String response = http.getString();

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    Serial.print("Endpoint Response: ");
    Serial.println(response);

    http.end();
  } else {
    Serial.println ("Wifi Connection Failed");
  }

}


String printLocaltime() {
  
  timeClient.update();

  unsigned long epochTime = timeClient.getEpochTime();
  byte pak_hour = timeClient.getHours();
  byte pak_min  = timeClient.getMinutes();
  byte pak_sec  = timeClient.getSeconds();

  struct tm *ptm = gmtime ((time_t *)&epochTime);

  byte pak_day  = ptm->tm_mday;
  byte pak_mon  = ptm->tm_mon+1;
  byte pak_year = ptm->tm_year+1900;

  String timestamp = String(pak_day)+"/"+String(pak_mon)+"/"+String(pak_year)+","+String(pak_hour)+":"+String(pak_min)+":"+String(pak_sec);
  return timestamp;
}

String JSONPayload() {
  String payload;
  DynamicJsonBuffer jBuffer;
  JsonObject& root = jBuffer.createObject();

  root["mac_id"] = String(ESP.getChipId());
  root["timestamp"] = printLocaltime();
  root["sensor_status"] = "Sensor is Triggered";

  root.printTo(payload);
  return payload;
}
