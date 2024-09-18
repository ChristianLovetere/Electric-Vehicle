/* Receiver code for the Arduino Radio control with PWM output
 * Connect a Reyax RYLR896 module to it:
 
    Module // Arduino Mega
    
    GND    ->   GND
    VDD    ->   3.3V
    TX     ->   RX
    RX     ->   TX
    NRST   ->   D22

This code receive 3 channels and create a PWM output for each one on D3, D4, D5 */

#include <Servo.h>

Servo escR;
Servo escL;
Servo myServo;

int ch1_value = 0;
int ch2_value = 0;
int ch3_value = 0;
unsigned long lastRecvTime = 0;

 int unpacked_data[3];

void setup(void) {

  while (!Serial);

  myServo.attach(3); //pin3 sends steering PWM to Servo
  escL.attach(4); //pin4 sends motor PWM to left motor
  escR.attach(5); //pin5 sends motor PWM to right motor
  
    for (int pin = 6; pin <= 11; ++pin) { //set all the relay control pins as outputs
    pinMode(pin, OUTPUT);
  }
  delay(1000);
  Serial.begin(115200);

  String listOfCommandsToSend[7] = {"AT\r\n", "AT+PARAMETER=7,9,4,7\r\n", "AT+NETWORKID=6\r\n",
                                    "AT+ADDRESS=1\r\n", "AT+PARAMETER?\r\n", "AT+NETWORKID?\r\n",
                                    "AT+ADDRESS?\r\n"};

  for (int i = 0; i < 7; i++){
    Serial.print(listOfCommandsToSend[i]);
    delay(250);
  }
}

void loop(void) {
  
  // Read and print the response
  String data = "";
  while (Serial.available()) {
    char c = Serial.read();
    data += c;
  }
  Serial.print(data);

  int index0 = data.indexOf("@");
  int index1 = data.indexOf("#");
  int index2 = data.indexOf("$");
  int index3 = data.indexOf("%");
  String data0 = (data.substring(index0+1, index1));
  String data1 = (data.substring(index1+1, index2));
  String data2 = (data.substring(index2+1, index3));
  unpacked_data[0] = data0.toInt();
  unpacked_data[1] = data1.toInt();
  unpacked_data[2] = data2.toInt();
  ch1_value = map(unpacked_data[0],-350,350,800,2000);
  ch2_value = map(unpacked_data[1], -350,350,1000,2000);
  ch3_value = map(unpacked_data[2],-350,350,1000,2000);
  lastRecvTime = millis(); //Here we receive the data

  if (ch2_value == 2000 && ch3_value == 2000){
    for (int pin = 6; pin <= 11; ++pin) {
    digitalWrite(pin, HIGH);
    }
  }
  else if (ch2_value == 1000 && ch3_value == 1000){
    for (int pin = 6; pin <= 11; ++pin) {
    digitalWrite(pin, LOW);
    }
  }
  
  Serial.println(data0+" "+data1+" "+data2);
  //print: ch1val ch2val ch3val
  Serial.print(String(ch1_value) + " " + String(ch2_value) + " " + String(ch3_value) + "\n");
 
  myServo.write(ch1_value);
  escL.write(ch2_value);
  escR.write(ch3_value);

  // Reset the module (if needed)
  digitalWrite(22, LOW);
  delay(100);
  digitalWrite(22, HIGH);

  
}
