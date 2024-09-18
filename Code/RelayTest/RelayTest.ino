void setup() {
  // Set pins 6 to 11 as OUTPUT
  for (int pin = 6; pin <= 11; ++pin) {
    pinMode(pin, OUTPUT);
  }
  Serial.begin(9600);  // Initialize serial communication
}

void loop() {
  for (int pin = 6; pin <= 11; ++pin) {
    delay(500);
    digitalWrite(pin, HIGH);
    Serial.println("Pin " + String(pin) + " activated");
    delay(1000);  // Wait for 1 second
    digitalWrite(pin, LOW);
  }
}
