#include "deep_sleep.h"

RTC_DATA_ATTR uint8_t registered;
uint8_t gpio_interr = false;
uint8_t timer_interr = false;
String macId;
float BATTERY_VOLTAGE;
String BATTERY_STATUS;
WiFiMulti wifiMulti;

extern "C"
{

	uint8_t init_system()
	{

	  setCpuFrequencyMhz(80);
		
	  pinMode(BLUE_LED, OUTPUT);
	  
	  rtc_gpio_init(RED_LED);
	  rtc_gpio_init(GREEN_LED);
	  rtc_gpio_init(ORANGE_LED);

	  rtc_gpio_set_direction(RED_LED, RTC_GPIO_MODE_OUTPUT_ONLY);
	  rtc_gpio_set_direction(GREEN_LED, RTC_GPIO_MODE_OUTPUT_ONLY);
	  rtc_gpio_set_direction(ORANGE_LED, RTC_GPIO_MODE_OUTPUT_ONLY);
	  
	  esp_sleep_enable_ext0_wakeup(GPIO_NUM_11, 0); //1 = High, 0 = Low
	  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
		
	  return SUCCESS;
	}

	uint8_t init_wifi()
	{
	wifiMulti.addAP(SSID, PSK); // SSID and Pass here
	
	  uint8_t count = 0;

	  while(wifiMulti.run() != WL_CONNECTED)
	  {
		Serial.print(".");
		
		count++;
		delay(100);

		if(count >= 100)
		{
		  ESP.restart();
		}
	  }
	
	return SUCCESS;
	}

	uint8_t check_device_registration()
	{
		macId =  WiFi.macAddress();
		
		if(!registered)
		{
		  if (!CheckMacId(macId))
		  {
			Serial.println("******************************************************************************");
			Serial.println("First time Power up Update");
			CreateSensor(macId);
			Serial.println("******************************************************************************");
		  }
		}

		return SUCCESS;
	}
	
	uint8_t measure_battery_and_update_leds()
	{
	  int BATTERY_ADC = analogRead(BATTERY_PIN);    //takes ADC value (from 3.3v pin)
	  float SCALING_FACTOR = 2.3 / 7200;            // Scaling ADC value for 4.5v (AAA battery)
	  float BATTERY_ADC_VOLTAGE = BATTERY_ADC * SCALING_FACTOR;     //(Latest voltage of AAA battery)
	  float VOLTAGE_CONVERSION_FACTOR = 4.5 / 2.3 ;

	  BATTERY_VOLTAGE = BATTERY_ADC_VOLTAGE * VOLTAGE_CONVERSION_FACTOR + 0.5;

	  Serial.println("ADC battery voltage");
	  Serial.println(BATTERY_VOLTAGE);

	  if (BATTERY_VOLTAGE > 3.9)
	  {
		BATTERY_STATUS = "good";


		rtc_gpio_set_level(GREEN_LED, LOW);
		rtc_gpio_set_level(RED_LED, HIGH);
		rtc_gpio_set_level(ORANGE_LED, HIGH);

	  }

	  else if ((BATTERY_VOLTAGE >= 3.3) && (BATTERY_VOLTAGE <= 3.9))
	  {
		BATTERY_STATUS = "medium";


		rtc_gpio_set_level(GREEN_LED, HIGH);
		rtc_gpio_set_level(RED_LED, HIGH);
		rtc_gpio_set_level(ORANGE_LED, LOW);
	  }

	  else if (BATTERY_VOLTAGE < 3.3)
	  {
		BATTERY_STATUS = "low";


		rtc_gpio_set_level(GREEN_LED, HIGH);
		rtc_gpio_set_level(RED_LED, LOW);
		rtc_gpio_set_level(ORANGE_LED, HIGH);
	  }

	  return SUCCESS;
	}
	
	uint8_t update_status()
	{
		GetWakeupReason();
		
		if (timer_interr)
		{
		  Serial.println("******************************************************************************");
		  timer_interr = false;
		  Serial.println("Battery Update (Timer)");
		  UpdateTimer(macId);
		  Serial.println("******************************************************************************");
		}

		else if (gpio_interr)
		{
		  Serial.println("******************************************************************************");
		  gpio_interr = false;
		  Serial.println("Shock Update");
		  UpdateShock(macId);
		  Serial.println("******************************************************************************");
		}
		
		digitalWrite(BLUE_LED, HIGH);

		esp_deep_sleep_start();
		
		return SUCCESS;
	}

	static uint8_t GetWakeupReason()
	{

	  esp_sleep_wakeup_cause_t wakeup_reason;

	  wakeup_reason = esp_sleep_get_wakeup_cause();

	  switch (wakeup_reason)
	  {
		case ESP_SLEEP_WAKEUP_EXT0 :
		  Serial.println("Wakeup caused by external signal using RTC_IO");
		  gpio_interr = true;
		  break;

		case ESP_SLEEP_WAKEUP_EXT1 :
		  Serial.println("Wakeup caused by external signal using RTC_CNTL");
		  break;

		case ESP_SLEEP_WAKEUP_TIMER :
		  Serial.println("Wakeup caused by timer");
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
	
	  return SUCCESS;
	}

	static uint8_t CheckMacId(String macId)
	{
	  HTTPClient http;
	  uint8_t already_added = false;

	  Serial.println("Sending request");

	  Serial.println("Inside request");
	  http.begin(String(URL) + "sensor_info/get_sensors_by_id/5b09nPavNJHKx8leuYLK");

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
			registered = true;
		  }
		} else {
		  Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}

	  }

	  http.end();
	  return (already_added ? SUCCESS : FAILED);
	}
	
	static uint8_t UpdateShock(String macId)
	{
		HTTPClient http;
		//  measureBattery();

		String requestData;
		requestData = "";

		StaticJsonDocument<500> doc;

		doc["mac_address_id"] = macId;
		doc["location_id"] = "PabBqnXhbmASUP9WPHO4";
		doc["severity"] = "NULL";
		doc["part_damaged"] = "NULL";
		doc["notes"] = "NULL";
		doc["status"] = "New Impact";
		doc["battery_status"] = "NULL";

		serializeJson(doc, requestData);


		Serial.print("[HTTP] begin...\n");
		Serial.println(requestData);
		Serial.println(requestData.length());
		http.begin(String(URL) + "sensor_info/create_incident"); //HTTP

		http.addHeader("Content-Type", "application/json");
		http.addHeader("Content-Length", String(requestData.length()));
		int httpCode = http.POST(requestData);

		// httpCode will be negative on error
		if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		Serial.printf("[HTTP] POST... code: %d\n", httpCode);
		//  Serial.println("Incident happened" + macId);

		// file found at server
		if (httpCode == HTTP_CODE_OK) {
		  String payload = http.getString();
		  Serial.println(payload);
		}
		} else {
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}

		http.end();

		return SUCCESS;
	}
	
	//************************************************************************************
	static uint8_t UpdateTimer(String macId)
	{
		HTTPClient http;
		//  measureBattery();
		String requestData;
		requestData = "";

		StaticJsonDocument<500> doc;

		doc["mac_address_id"] = macId;
		doc["location_id"] = "PabBqnXhbmASUP9WPHO4";
		doc["severity"] = "NULL";
		doc["part_damaged"] = "NULL";
		doc["notes"] = "NULL";
		doc["status"] = "New Impact";
		doc["battery_status"] = "NULL";

		serializeJson(doc, requestData);

		Serial.print("[HTTP] begin...\n");
		Serial.println(requestData);
		Serial.println(requestData.length());
		http.begin(String(URL) + "sensor_info/create_incident"); //HTTP

		http.addHeader("Content-Type", "application/json");
		http.addHeader("Content-Length", String(requestData.length()));
		int httpCode = http.POST(requestData);

		// httpCode will be negative on error
		if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		Serial.printf("[HTTP] POST... code: %d\n", httpCode);
		//  Serial.println("Incident happened" + macId);

		// file found at server
		if (httpCode == HTTP_CODE_OK) {
		  String payload = http.getString();
		  Serial.println(payload);
		}
		} else {
		Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}

		http.end();

		return SUCCESS;
	}
	
	static uint8_t CreateSensor(String macId)
	{

		HTTPClient http;
		String requestData = "";

		requestData = "";

		StaticJsonDocument<200> doc;

		doc["mac_address"] = macId;
		doc["rack_block_id"] = "5b09nPavNJHKx8leuYLK";

		serializeJson(doc, requestData);

		Serial.print("[HTTP] begin...\n");
		Serial.println(requestData);
		Serial.println(requestData.length());
		http.begin(String(URL) + "sensor_info/create_sensor"); //HTTP

		http.addHeader("Content-Type", "application/json");
		http.addHeader("Content-Length", String(requestData.length()));
		int httpCode = http.POST(requestData);

		// httpCode will be negative on error
		if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		Serial.printf("[HTTP] POST... code: %d\n", httpCode);

		// file found at server
		if (httpCode == HTTP_CODE_OK) {
		  String payload = http.getString();
		  Serial.println(payload);
		}
		} else {
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
		}

		http.end();

		return SUCCESS;
	}

}