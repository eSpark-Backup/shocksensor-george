#include "deep_sleep.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  init_system();
  measure_battery_and_update_leds();
  init_wifi();
  check_device_registration();
  update_status();

}

void loop() {
  // put your main code here, to run repeatedly:

}
