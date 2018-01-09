# PPM-GEN
ppm-gen outputs a series of RC ppm pulses based on
the incoming packet from the usb serial port  
The output can be connected directly to an RC transmitter's
trainer port input (currently setup for Futaba or compatible)

## ppm pulse format
ppm format is a series of 400us pulse train. The time between the rising or falling edges are each channel pulse width. The pulse width of a channel generally goes from 1000us to 2000us corresponding to 90 degree servo rotation, but on some servos it has an extended range 500us to 2500us for 180 degree rotation. The pulse width vary for each manufacturers.

## Supported platform
Arduino pro micro, but can be ported to other Arduino boards or just plain C

## Serial port settings
115200 8 N 1

## Packet format
```
0x01 - start byte
0xXX - packet length including start and end bytes
0x01 - packet type, 0x01 - output ppm. can be extended later
ch1  - lower byte -- channel value range 800 - 2200 us
ch1  - upper byte
ch2  - lower byte
ch2  - upper byte
...
0xXX - checksum
0x04 - end byte
```  
for example:  
```
01 11 01 dc 05 dc 05 dc 05 dc 05 dc 05 dc 05 59 04
```  
outputs 6 channels pulse, each set to 1500us

## LEDS
- yellow LED flashes on outgoing pulses  
- green LED flashes on detected incoming packets

## Output
- output pin is pin9 on sparkfun arduino pro micro
(atmega32u4 PORTB5)

- output signal can be inverted by enabling PPM_INVERTED
by default signal is mostly high with 300us low pulses

- output signals stops when no incoming packet received within timeout window, but
this can be disabled by enabling PPM_NO_TIMEOUT

## Configuration
configuration settings are in config.h
In future settings could be stored in eeprom
and set/read through serial port commands

## Python test program for host PC
ser2ppm.py is a simple python test program generating packets for the arduino board
from the host PC.  
Command line arguments specify port number and a series of channel values for ppm output
