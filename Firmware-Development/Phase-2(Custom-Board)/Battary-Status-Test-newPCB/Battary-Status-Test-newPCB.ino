void setup() {
  Serial.begin(9600);
  pinMode(10, OUTPUT);

  digitalWrite(10, HIGH);

}

void loop() {
  int BATTERY_ADC = analogRead(1);
  Serial.print("BATTERY_ADC : ");
  Serial.println(BATTERY_ADC);
  float voltage = BATTERY_ADC * (2.3 / 7200);
  voltage = voltage * (4.5/2.3);
  Serial.print("Voltage : ");
  Serial.println(voltage);

}
