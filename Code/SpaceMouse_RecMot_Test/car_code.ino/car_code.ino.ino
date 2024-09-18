/* Receiver code for the Arduino Radio control with PWM output
  * Connect a NRF24 module to it:

    Module // Arduino Mega

    GND    ->   GND
    Vcc    ->   3.3V
    CE     ->   D48
    CSN    ->   D49
    SCK    ->   D52
    MOSI   ->   D51
    MISO   ->   D50

This code receive 3 channels and create a PWM output for each one on D3, D4, D5
*/
# include<SPI.h>                   // spi library for connecting nrf
# include<RF24.h>                  // nrf library
# include "printf.h"

# include <RC_Receiver.h>
# include <Servo.h>

Servo escR;
Servo escL;
Servo myServo;
int f = 0;

RF24 radio(46, 53); // ce, csn pins
const uint64_t pipe = 0xE0E0F1F1E0LL ;    // pipe address same as sender i.e. raspberry pi
int ch1_value = 0;
int ch2_value = 0;
int ch3_value = 0;
unsigned long lastRecvTime = 0;

 int16_t unpacked_data[3]; // Using int16_t for 2-byte signed integers
  int8_t chan = 118;
void reset_the_Data()
{
  // 'safe' values to use when NO radio input is detected
  ch1_value = 1500+f;
  ch2_value = 1500+f;
  ch3_value = 1500+f;
}

void setup(void) {

  while (!Serial);

  printf_begin();
  radio.begin();        // start radio at ce csn pin 9 and 10
  Serial.begin(9600);     // start serial monitor baud rate
  radio.setChannel(chan);            // set chanel at 76

  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MIN) ;   // set power level
  radio.openReadingPipe(0, pipe) ;        // start reading pipe
  radio.setPayloadSize(32);
  radio.startListening() ;        // start listening forever
  Serial.println("Starting.. Setting Up.. Radio on..") ; // debug message
  radio.enableDynamicPayloads() ;
  radio.powerUp() ;
  
  radio.printDetails();
 Serial.println("Ready to Receive") ;

 reset_the_Data();

  myServo.attach(3); //pin3 sends steering PWM to Servo
  escL.attach(4);    //pin4 is sends motor PWM to left motor
  escR.attach(5);   //pin5 is sends motor PWM to right motor


  delay(2000);
}


void loop(void) {
  f=0;
//Motor and Servo Test:
  // if (f <= 500 && f >= 0){
  //   f++;
  // } else {
  //   f=-150;
  // }


 while (radio.available()) {
    // Read the received data
    //int16_t unpacked_data[3]; // Using int16_t for 2-byte signed integers
    radio.read(&unpacked_data, sizeof(unpacked_data));

    // Print the unpacked data
    for (int i = 0; i < 3; i++) {
      Serial.print(unpacked_data[i]);
      Serial.println(" ");
    }
    ch1_value = map(unpacked_data[0],-350,350,1000,2000);
    ch2_value = map(unpacked_data[1],-350,350,1000,2000);
    ch3_value = map(unpacked_data[2],-350,350,1000,2000);
    lastRecvTime = millis(); //Here we receive the data
 }

  unsigned long now = millis();
  if ( now - lastRecvTime > 1000 ) {
    // signal lost?
    reset_the_Data();
    //Go up and change the initial values if you want depending on
    //your aplications. Put 0 for throttle in case of drones so it won't
    //fly away
  }

  Serial.print(ch1_value);
  Serial.print(" ");
  Serial.print(ch2_value);
  Serial.print(" ");
  Serial.print(ch3_value);
  Serial.println(" ");

  myServo.write(ch1_value);
  escL.write(ch2_value);
  escR.write(ch3_value);



}
