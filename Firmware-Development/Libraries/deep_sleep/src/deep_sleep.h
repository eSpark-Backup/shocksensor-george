#pragma once

#include "Arduino.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "WiFiMulti.h"
#include "driver/rtc_io.h"
#include "ArduinoJson.h"

// macros and constant variables

#define BLUE_LED 14
#define GREEN_LED GPIO_NUM_21
#define ORANGE_LED GPIO_NUM_20
#define RED_LED GPIO_NUM_19
#define BATTERY_PIN 1

#define URL "http://35.184.38.177:8000/"

#define FAILED 0
#define SUCCESS 1
#define TRUE 1
#define FALSE 0

#define SSID "eSpark Consulting"
#define PSK "47356880"

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  15        /* Time ESP32 will go to sleep (in seconds) */

extern "C"
{
	// Public functions
	uint8_t init_system();
	uint8_t init_wifi();
	uint8_t check_device_registration();
	uint8_t measure_battery_and_update_leds();
	uint8_t update_status();
	
	// Private functions
	static uint8_t GetWakeupReason();
	static uint8_t CreateSensor(String mac_id);
	static uint8_t CheckMacId(String mac_id);
	static uint8_t UpdateTimer(String mac_id);
	static uint8_t UpdateShock(String mac_id);
}
