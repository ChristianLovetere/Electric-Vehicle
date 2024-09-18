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
#include<SPI.h>                   // spi library for connecting nrf
#include<RF24.h>                  // nrf library
#include "printf.h"

#include <RC_Receiver.h>
#include <Servo.h>

Servo escR;
Servo escL;
Servo myServo;

RF24 radio(48, 49);  // ce, csn pins    
const uint64_t pipe = 0xE0E0F1F1E0LL ;    // pipe address same as sender i.e. raspberry pi
int ch1_value = 0;
int ch2_value = 0;
int ch3_value = 0;
unsigned long lastRecvTime = 0;

 int16_t unpacked_data[3]; // Using int16_t for 2-byte signed integers

void reset_the_Data() 
{
  // 'safe' values to use when NO radio input is detected
  ch1_value = 1500;
  ch2_value = 1500;
  ch3_value = 1500;
}

void setup(void) {

  printf_begin();
  Serial.begin(9600);     // start serial monitor baud rate


 reset_the_Data();

  myServo.attach(3); //pin3 sends steering PWM to Servo
  escL.attach(4);    //pin4 is sends motor PWM to left motor
  escR.attach(5);   //pin5 is sends motor PWM to right motor


  delay(2000);
}

int testnum = 0;
void loop(void) {


  if (testnum < 6){
    testnum++;
  }  else {testnum = 0;}


  //test Values
  switch(testnum) {
    case 1:
      ch1_value = 800;
      ch2_value = 800;
      ch3_value = 800;
    break;

    case 2:
      ch1_value = 1000;
      ch2_value = 1000;
      ch3_value = 1000;
    break;

    case 3:
      ch1_value = 1200;
      ch2_value = 1200;
      ch3_value = 1200;
    break;

    case 4:
      ch1_value = 1400;
      ch2_value = 1400;
      ch3_value = 1400;
    break;

    case 5:
      ch1_value = 1600;
      ch2_value = 1600;
      ch3_value = 1600;
    break;

    default: 
      ch1_value = 1500;
      ch2_value = 1500;
      ch3_value = 1500;
    break;
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

delay(2000);

}