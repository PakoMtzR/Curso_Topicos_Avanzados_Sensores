void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print(1);
  Serial.print(",");
  Serial.print(2);
  Serial.print(",");
  Serial.print(3);
  Serial.print(",");
  Serial.print(4);
  Serial.print(",");
  Serial.println(5);
  delay(100);
}
