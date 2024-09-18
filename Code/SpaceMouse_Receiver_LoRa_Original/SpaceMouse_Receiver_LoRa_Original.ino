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
#include <Servo.h>

Servo escR;
Servo escL;
Servo myServo;

int ch1_value = 0;
int ch2_value = 0;
int ch3_value = 0;
unsigned long lastRecvTime = 0;

 int unpacked_data[3]; // Using int16_t for 2-byte signed integers

void reset_the_Data() 
{
  // 'safe' values to use when NO radio input is detected
  // ch1_value = 1500;
  // ch2_value = 1500;
  // ch3_value = 1500;
}

void setup(void) {

  while (!Serial);

  pinMode(53, OUTPUT);
  pinMode(46, OUTPUT);
  printf_begin();

 Serial.println("Ready to Receive") ;

 reset_the_Data();

  myServo.attach(3); //pin3 sends steering PWM to Servo
  escL.attach(4);    //pin4 is sends motor PWM to left motor
  escR.attach(5);   //pin5 is sends motor PWM to right motor


   // put your setup code here, to run once:

  //pinMode(rxPin, INPUT);
  //pinMode(txPin, OUTPUT);
  delay(1000);
  Serial.begin(115200);
  //Serial.end();
  
  //serial1.begin(115200);
  delay(250);
  Serial.print("AT\r\n");
  //Serial.flush();
  delay(250);
  Serial.print("AT+PARAMETER=7,9,4,7\r\n");
  //Serial.flush();
  delay(250);
  Serial.print("AT+NETWORKID=6\r\n");
  //Serial.flush();
  delay(250);
  Serial.print("AT+ADDRESS=1\r\n");
  //Serial.flush();
  delay(250);
  Serial.print("AT+PARAMETER?\r\n");
  //Serial.flush();
  delay(250);
  Serial.print("AT+NETWORKID?\r\n");
  //Serial.flush();
  delay(250);
  Serial.print("AT+ADDRESS?\r\n");
  //Serial.flush();
  delay(250);
  //Serial.end();
}


void loop(void) {


  // Wait for a response from the module
  //delay(2000);  // Longer delay to allow processing

  // Read and print the response
  String data = "";
  while (Serial.available()) {
    char c = Serial.read();
    data += c;
    //Serial.print(c);
  }
  //Serial.print(data);
  //Serial.flush();



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
    //Serial.println("Data Received");
 

   Serial.println(data0+" "+data1+" "+data2);
   Serial.print(ch1_value);
   Serial.print(" ");
   Serial.print(ch2_value);
   Serial.print(" ");
   Serial.print(ch3_value);
   Serial.println(" ");
  
  myServo.write(ch1_value);
  escL.write(ch2_value);
  escR.write(ch3_value);

  // Reset the module (if needed)
  digitalWrite(22, LOW);
   delay(100);
   digitalWrite(22, HIGH);

}