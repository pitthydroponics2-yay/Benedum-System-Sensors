void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  float resistance = analogRead(0);

  float height = (resistance-491.18)/30.905;

  Serial.print(height);
  Serial.print(" in");
  Serial.print('\n');

  delay(3000);
}
