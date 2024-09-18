#include <Servo.h>

Servo sv;

void setup() {
  // put your setup code here, to run once:
  sv.attach(3);  
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 1500; i <= 2500; i+=10) {
    sv.write(i);
    delay(10);
  }
    for (int i = 2500; i >= 1500 ; i-=10) {
    sv.write(i);
    delay(10);
  }  
}
