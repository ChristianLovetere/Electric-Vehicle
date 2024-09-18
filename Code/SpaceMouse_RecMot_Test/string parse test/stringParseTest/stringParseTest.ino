

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  String test = String("857");
  for (int i = 0; i < test.length(); i++) {
    Serial.println((int)test[i]-48);
  }
}
