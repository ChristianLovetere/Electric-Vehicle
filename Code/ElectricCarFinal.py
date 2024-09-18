#!/usr/bin/env python
#input from spacemouse sends 3 variables from rpi to arduino
#transmitter code
#pi@raspberrypi:~/Desktop/Dahlander_Motor $ sudo python SpaceMouse_Transmitter_Final.py


import RPi.GPIO as GPIO
import time
import spidev
spi = spidev.SpiDev()
spi.open(0,1)
import sys
import struct
import usb
import usb.core
import usb.util

from time import gmtime, strftime
import time
import serial

#instantiate serial channel 
ser = serial.Serial('/dev/ttyS0', baudrate=115200)
def setup():
    GPIO.setmode(GPIO.BCM)                   # set the gpio mode

    #List of all the commands we want to run for reyax setup
    listToBeWrittenToSerial = [b'AT+PARAMETER=7,9,4,7\r\n', b'AT+NETWORKID=6\r\n',
                               b'AT+ADDRESS=0\r\n', b'AT+PARAMETER?\r\n', b'AT+NETWORKID?\r\n',
                               b'AT+ADDRESS?\r\n', b'AT+IPR?\r\n']

    print('##########Setup Phase##########')
    #for loop that writes each command, waits a sec, then prints reyax's response
    for command in listToBeWrittenToSerial: #for all strings in the list
        decodedCommand = command.decode('utf-8') #create decoded command
        cleanCommand = decodedCommand.replace("AT+", "").replace("\r\n", "") #readable version of the command
        try:
            ser.write(command) #write command to serial line
            time.sleep(0.2) #wait 200ms
            response = ser.read_all().decode() #get Reyax's response
            print("from", cleanCommand, "| Reyax RYLR896 says:", response) #readable feedback during code execution
        except UnicodeDecodeError: #throw out data if a decoding error happens
            print("Data corrupted. Could not decode ", command, ".")

    time.sleep(1)

def main(args):

    setup()
    # Look for SpaceNavigator
    dev = usb.core.find(idVendor=0x256f, idProduct=0xc635)

    cfg = dev.get_active_configuration()
    intf = cfg[(0,0)]
    ep = usb.util.find_descriptor(intf, custom_match = lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)
    
    reattach = False
    if dev.is_kernel_driver_active(0):
        reattach = True
        dev.detach_kernel_driver(0)
    
    ep_in = dev[0][(0,0)][0]
    ep_out = dev[0][(0,0)][1]
    
    print ('Exit by pressing any button on the SpaceNavigator\n')
    time.sleep(1)

###########################

    rz = 0
    motLeft = 0
    motRight = 0
    
    run = True
    while run :
        try:
            data = dev.read(int(ep_in.bEndpointAddress), int(ep_in.bLength), 0)
            #print data
    
            if data[0] == 1:
                # translation packet
                tx = data[1] + (data[2]*256)
                ty = data[3] + (data[4]*256)
                tz = data[5] + (data[6]*256)
                
                if data[2] > 127:
                    tx -= 65536
                if data[4] > 127:
                    ty -= 65536
                if data[6] > 127:
                    tz -= 65536
    
            if data[0] == 2:
                # rotation packet
                rx = data[1] + (data[2]*256)
                ry = data[3] + (data[4]*256)
                rz = data[5] + (data[6]*256)
                
                if data[2] > 127:
                    rx -= 65536
                if data[4] > 127:
                    ry -= 65536
                if data[6] > 127:
                    rz -= 65536
    
                
            if data[0] == 3 and data[1] == 0:
                # button packet - exit on the release
                run = False
                
    
            #motor
            # -ty controls forward movement, +ty backwards
            # -rz turns right, rz turns left
            motRight = -ty - rz/4;              #rz/x changes degree of steering 
            motLeft = -ty + rz/4;
            if motRight > 350:
               motRight = 350;
            if motRight < -350:
               motRight = -350;
            if motLeft > 350:
               motLeft = 350;
            if motLeft < -350:
               motLeft = -350;

            try:
                c1 = 'AT+SEND=1,' #first segment of message
                SMdata = "@"+str(int(rz))+"#"+str(int(motLeft))+"$"+str(int(motRight))+"%" #third segment of message - Mouse data enclosed with parsing identifiers
                lengthOfSMdata = str(len(SMdata)) #second segment of message
                c2 = '\r\n' #fourth segment of message

                fullMessage = c1+lengthOfSMdata+","+SMdata+c2 #Full unencoded string for command
                encodedMessage = fullMessage.encode("utf-8") #encode message to utf-8

                cleanMessage = fullMessage.replace("AT+", "").replace("\r\n", "") #readable version of the unencoded message
                ser.write(encodedMessage) #send encoded message to serial
                time.sleep(0.10) #wait 100ms
                response = ser.read_all().decode() #read reyax's response to the command we sent
                print("from", cleanMessage, "| Reyax RYLR896 says:", response) #readable feedback during code execution
            except UnicodeDecodeError: #if a decoding error occurs, that message is not sent
                print("Data corrupted. Skipping data.")

        except usb.core.USBError:
             print("USB error")
             run = False
        except Exception as e:
             print("read failed")
             print(e)

        # end while
    usb.util.dispose_resources(dev)
        
    if reattach:
        dev.attach_kernel_driver(0)
        return 0

if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))
