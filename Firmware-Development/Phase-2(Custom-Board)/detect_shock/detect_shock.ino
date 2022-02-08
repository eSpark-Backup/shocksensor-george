#define SHOCK_SENSOR 11

void setup() {
  // put your setup code here, to run once:
  pinMode(SHOCK_SENSOR, INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("Shock response");
  Serial.println(digitalRead(SHOCK_SENSOR));

  //delay(1000);
}
