#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid     = "StormFiber-2.4G";
const char* password = "47779884";
String macId = "";
String url = "http://35.184.38.177:8000/";

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  15        /* Time ESP32 will go to sleep (in seconds) */

boolean gpio_interr = false;
boolean timer_interr = false;

RTC_DATA_ATTR

const int ESP_LED_PIN = 2;
const int BUZZER = 12;
int BUZZER_ON_TIME = 10000;
int BUZZER_START_TIME = 0;

const int BATTERY_PIN = 34;
String BATTERY_STATUS;



//*******************************************************************************
void measureBattery()
{
  int BATTERY_ADC = analogRead(BATTERY_PIN);    //takes ADC value (from 3.3v pin)

  float SCALING_FACTOR = 4.5 / 1024;            // Scaling ADC value for 4.5v (AAA battery)

  float BATTERY_VOLTAGE = BATTERY_ADC * SCALING_FACTOR;     //(Latest voltage of AAA battery)

  if(BATTERY_VOLTAGE > 3.9)
  {
    BATTERY_STATUS = "good";
  }

  else if((BATTERY_VOLTAGE >= 3.3) && (BATTERY_VOLTAGE <= 3.9))
  {
    BATTERY_STATUS = "medium";
  }

  else if(BATTERY_VOLTAGE < 3.3)
  {
    BATTERY_STATUS = "low";
  }
  
}


/*
  Method to print the reason by which ESP32
  has been awaken from sleep
*/
void print_wakeup_reason() {    //*****************************************************************************
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 :
      Serial.println("Wakeup caused by external signal using RTC_IO");
      gpio_interr = true;
      pinMode(BUZZER, OUTPUT);
      digitalWrite(BUZZER, HIGH);
  //    BUZZER_START_TIME = millis();
      break;

    case ESP_SLEEP_WAKEUP_EXT1 :
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      break;

    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer");
      timer_interr = true;
      break;

    case ESP_SLEEP_WAKEUP_TOUCHPAD :
      Serial.println("Wakeup caused by touchpad");
      break;

    case ESP_SLEEP_WAKEUP_ULP :
      Serial.println("Wakeup caused by ULP program");
      break;

    default :
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
      break;
  }
}

//********************************************************************************
boolean checkMacId(String macId)
{
  HTTPClient http;
  boolean already_added = false;

  Serial.println("Sending request");
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Inside request");
    http.begin(url + "sensor_info/get_sensors_by_id/5b09nPavNJHKx8leuYLK");

    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);

        if (payload.indexOf(macId) > 0)
        {
          already_added = true;
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

    }

    http.end();
    return already_added;
  }
}

//**********************************************************************************
void createSensor(String macId)
{

  HTTPClient http;
  String requestData = "{\"mac_address\":\"" + macId + "\" ,\"rack_block_id\": \"5b09nPavNJHKx8leuYLK\"}";

  Serial.print("[HTTP] begin...\n");
  http.begin(url + "/sensor_info/create_sensor"); //HTTP

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(requestData.length()));
  int httpCode = http.POST(requestData);

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    Serial.println("Sensor created" + macId);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}


//************************************************************************************
void Update_Power_Up(String macId)
{
  HTTPClient http;
  measureBattery();
  String requestData = "{\"mac_address\":\"" + macId + "\" , \"location_id\":\"PabBqnXhbmASUP9WPHO4\" , \"severity\":\"NULL\" , \"part_damaged\":\"NULL\" , \"notes\":\"NULL\" , \"status\":\"ok\" , \"battery_status\":\"" + BATTERY_STATUS + "\"}";

  Serial.print("[HTTP] begin...\n");
  http.begin(url + "/sensor_info/create_incident"); //HTTP

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(requestData.length()));
  int httpCode = http.POST(requestData);

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    Serial.println("Incident happened" + macId);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}



//************************************************************************************
void Update_Timer(String macId)
{
  HTTPClient http;
  measureBattery();
  String requestData = "{\"mac_address\":\"" + macId + "\" , \"location_id\":\"PabBqnXhbmASUP9WPHO4\" , \"severity\":\"NULL\" , \"part_damaged\":\"NULL\" , \"notes\":\"NULL\" , \"status\":\"NULL\" , \"battery_status\":\"" + BATTERY_STATUS + "\"}";

  Serial.print("[HTTP] begin...\n");
  http.begin(url + "/sensor_info/create_incident"); //HTTP

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(requestData.length()));
  int httpCode = http.POST(requestData);

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    Serial.println("Incident happened" + macId);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}



//************************************************************************************
void Update_Shock(String macId)
{
  HTTPClient http;
  String requestData = "{\"mac_address\":\"" + macId + "\" , \"location_id\":\"PabBqnXhbmASUP9WPHO4\" , \"severity\":\"NULL\" , \"part_damaged\":\"NULL\" , \"notes\":\"NULL\" , \"status\":\"New Impact\" , \"battery_status\":\"NULL\"}";

  Serial.print("[HTTP] begin...\n");
  http.begin(url + "/sensor_info/create_incident"); //HTTP

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(requestData.length()));
  int httpCode = http.POST(requestData);

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    Serial.println("Incident happened" + macId);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}




void setup() {  //********************************************************************************************************************************************

  pinMode(ESP_LED_PIN, OUTPUT);
  digitalWrite(ESP_LED_PIN, HIGH);

 // BUZZER_START_TIME = 0;

  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor

  macId = WiFi.macAddress();
  //macId = "abracadabra";

  //Increment boot number and print it every reboot
  // ++bootCount;
  // Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
    First we configure the wake up source
    We set our ESP32 to wake up every 15 seconds
    and when GPIO33 is Active High
  */
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1); //1 = High, 0 = Low
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                 " Seconds");

  /*
    Next we decide what all peripherals to shut down/keep on
    By default, ESP32 will automatically power down the peripherals
    not needed by the wakeup source, but if you want to be a poweruser
    this is for you. Read in detail at the API docs
    http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
    Left the line commented as an example of how to configure peripherals.
    The line below turns off all RTC peripherals in deep sleep.
  */
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //Serial.println("Configured all RTC Peripherals to be powered down in sleep");

  /*
    Now that we have setup a wake cause and if needed setup the
    peripherals state in deep sleep, we can now start going to
    deep sleep.
    In the case that no wake up sources were provided but deep
    sleep was started, it will sleep forever unless hardware
    reset occurs.
  */

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  

  if (!checkMacId(macId))     //   && gpio_interr (Seems like it's Not Need in power up)
  {
                                   // gpio_interr = false;
    createSensor(macId);
    Serial.println("First time Power up Update");
    Update_Power_Up(macId);
  }

  else if (timer_interr)
  {
    timer_interr = false;
    Serial.println("Battery Update");
    Update_Timer(macId);
  }

  else if(gpio_interr)
  {
    gpio_interr = false;
    Serial.println("Shock Update");
    Update_Shock(macId);
    }
  else
  {
    Serial.println("Something is going wrong");
    }


    

/*  while ((millis() - BUZZER_START_TIME) <= (BUZZER_ON_TIME))
  {
    //wait for 10 seconds
  }*/

  delay(10000);
  digitalWrite(BUZZER, LOW);
  delay(5000);
  Serial.println("Going to sleep now");
  Serial.flush();
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop() {
  //This is not going to be called
}
