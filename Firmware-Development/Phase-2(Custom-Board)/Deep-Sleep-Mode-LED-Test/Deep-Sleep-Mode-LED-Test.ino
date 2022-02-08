#include <driver/rtc_io.h>


#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR bool FLAG = 0;

void setup() {
  Serial.begin(9600);
  rtc_gpio_init(GPIO_NUM_19);
  if (rtc_gpio_is_valid_gpio(GPIO_NUM_19))
    Serial.println("Valid");

  rtc_gpio_set_direction(GPIO_NUM_19, RTC_GPIO_MODE_OUTPUT_ONLY);



  Serial.println("Wake up");

  if (FLAG == 0)
  {
    rtc_gpio_hold_dis(GPIO_NUM_19);
    rtc_gpio_set_level(GPIO_NUM_19, LOW);
    rtc_gpio_hold_en(GPIO_NUM_19);
    Serial.println("LED ON");
    FLAG = 1;
  }
  else if (FLAG == 1)
  {
    rtc_gpio_hold_dis(GPIO_NUM_19);
    rtc_gpio_set_level(GPIO_NUM_19, HIGH);
    rtc_gpio_hold_en(GPIO_NUM_19);
    Serial.println("LED OFF");
    FLAG = 0;
  }

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  delay(1000);
  Serial.println("Going to sleep");
  esp_deep_sleep_start();
}

void loop() {
  // put your main code here, to run repeatedly:

}
