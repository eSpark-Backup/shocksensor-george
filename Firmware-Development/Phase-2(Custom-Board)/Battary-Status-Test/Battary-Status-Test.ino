void setup() {
  Serial.begin(9600);

}

void loop() {
  int BATTERY_ADC = analogRead(4);
  Serial.print("BATTERY_ADC : ");
  Serial.println(BATTERY_ADC);
  float voltage = BATTERY_ADC * (3.3 / 4096);
  voltage = voltage * (5/3.3);
  Serial.print("Voltage : ");
  Serial.println(voltage);

}
