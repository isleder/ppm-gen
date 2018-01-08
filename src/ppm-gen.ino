/********************************************
 * @author Istvan Sleder (4/18/2014)
 * @email istvan.sleder@gmail.com
*********************************************

usb_to_trainer outputs a series of RC ppm pulses based on
the incoming packet from the usb serial port
the output can be connected directly to an RC transmitter's
trainer port input

currently supported platform: Sparkfun arduino pro micro
it Can be easily ported to other arduino boards or just plain C

Serial port settings: 115200 8 N 1

Packet format:
--------------
0x01 - start byte
0xXX - packet length including start and end bytes
0x01 - packet type, 0x01 - output ppm. can be extended later
ch1 lower byte -- channel value range 800 - 2200 us
ch1 upper byte
ch2 lower byte
ch2 upper byte
...
0xXX - checksum
0x04 - end byte
---------------

for example:
01 11 01 dc 05 dc 05 dc 05 dc 05 dc 05 dc 05 59 04
outputs 6 channels set to 1500us

LEDS:
-----
yellow LED flashes on outgoing pulses
green LED flashes on detected incoming packets

OUTPUT:
-------
output pin is pin9 on sparkfun arduino pro micro
(atmega32u4 PORTB5)

output signal can be inverted by enabling PPM_INVERTED
by default signal is mostly high with 300us low pulses

output signals stops when no incoming packet received within timeout window
this can be disabled by enabling PPM_NO_TIMEOUT

CONFFIGURATDION:
----------------
for configuration settings see config.h

In future development settings could be stored in eeprom
and set/read through serial port commands

usb2train.py is a simple python test program
*********************************************/

#include <Arduino.h>
#include "config.h"
#include "main.h"

/* arduino init */
void setup()
{
	init();
	// setup serial port
	Serial.begin(SERIAL_PORT_BAUD);
}


/* arduino main */
void loop()
{
	main_task();
}
