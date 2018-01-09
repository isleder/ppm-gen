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
#include <avr/io.h>
#include "sio.h"
#include "config.h"

uint32_t ppm_timer = 0; // ppm timout timer

volatile uint8_t ppm_ch = 0; // current ppm channel count
volatile uint16_t ppm_channels_total; // sum of pulse widths
volatile struct ppm_channel_buff_t ppm_channels[2]; // double buffered ppm channels
volatile uint8_t ppm_active_buff = 0; // current active buffer to read from
volatile uint16_t ch_val = PPM_INTERVAL; // next channel timing
volatile uint8_t ppm_timeout = 1;
volatile uint8_t ppm_off = 1; // turns off ppm pulse at the sync

struct packet_t sio_rx_packet; // incoming packet from serial port

/******************************************
 * timer1 overflow interrupt
 * interrupt timing overhead until ICR1 is set
 * can be compansated for by counting assembly instructions (later)
 *****************************************/
ISR(TIMER1_OVF_vect)
{
	volatile struct ppm_channel_buff_t *ppm_ch_buff; // pointer to active buffer

	ppm_ch_buff = &ppm_channels[ppm_active_buff];

	if (ppm_ch < ppm_ch_buff->count) // output pulse
	{
		ICR1 = ch_val;	// set pulse total in timer1
		ch_val = ppm_ch_buff->channels[ppm_ch]; // get next channel value

		if (ppm_off)
		{
			OCR1A = 0xFFFF; // stop pulses
		}
		else
		{
			OCR1A = ch_val - PPM_PULSE_WIDTH ;
			LEDYELLOW_INV(); // flip yellow led to indicate output pulses
		}

		ppm_channels_total += ch_val;
		ppm_ch++;
	}
	else // output sync
	{
		ICR1 = ch_val;

		// switch to lower frame rate to maintain min sync pulse width
		if (ppm_channels_total  > PPM_INTERVAL - PPM_SYNC_MIN)
		{
			ch_val = PPM_INTERVAL_MAX - ppm_channels_total;
		}
		else
		{
			ch_val = PPM_INTERVAL - ppm_channels_total;
		}

		// stop pulse output at sync only and not in the middle
		// here detect ppm_timeout signal and notify the channel
		// output branch to turn off via ppm_off
		if (ppm_timeout)
		{
			ppm_off = 1;
			OCR1A = 0xFFFF; // stop pulses
		}
		else
		{
			OCR1A = ch_val - PPM_PULSE_WIDTH;
			ppm_off = 0;
			LEDYELLOW_INV(); // flip yellow led to indicate output pulses
		}

		ppm_channels_total = 0;
		ppm_ch = 0;
	}
}

/* uc init */
void init()
{
	LEDGREEN_OUT();
  	LEDYELLOW_OUT();
  	OC1A_OUT();

	// initial values for interrupt
	// timer1 is running at 2Mhz 8x prescaler
	// setup timer1 with 500ns clock (16MHz clock div 8)
	// all channel values are doubled to get usec
    #ifdef PPM_INVERTED
	TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1A0);// COM1A0 -- inverts output signal
	#else
	TCCR1A = (1 << WGM11) | (1 << COM1A1);
	#endif

    // divide clock by 8 for timer1
	TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS11);
  	OCR1A = 0xFFFF;  // no pulse out by default
  	ICR1 = 3000; // initial value

  	// enable timer1 overflow interrupt
  	TIMSK1 |= (1 << TOIE1);
}


/* serial port task */
void sio_rx_task()
{
	int c; // some odd reason Arduino use 16 bit signed int for Serial.read

	while ((c = Serial.read()) > -1)
	{
		int8_t res =  sio_rx_data(c, &sio_rx_packet);

		if (res > 0) // fully defined packet
		{

			uint8_t dest = ppm_active_buff ^= 0x01; // write to inactive buffer
			uint8_t count = sio_rx_packet.length >> 1; // div 2 truncates to lower count

			if (count > PPM_CHANNEL_COUNT_MAX) // limit incoming channel count
				count = PPM_CHANNEL_COUNT_MAX;

			ppm_channels[dest].count = count;

			uint8_t i;

			for (i = 0; i < count; i++)
			{
				uint8_t ix = i * 2;

				uint16_t val = sio_rx_packet.buff[ix]; // lower byte
				val |= (sio_rx_packet.buff[ix + 1] << 8); // upper byte
				val <<= 1; // multiply by 2 for 500ns clock

				// constrain values
				if (val < PPM_VALUE_MIN) val = PPM_VALUE_MIN;
				if (val > PPM_VALUE_MAX) val = PPM_VALUE_MAX;

				ppm_channels[dest].channels[i] = val; // set value in inactive buffer
			}

			// activate new buffer
			ppm_active_buff = dest;

			ppm_timer = micros(); // clear timeout
			ppm_timeout = 0;
		}
		else if (res < 0) // error
		{
			switch(res)
			{
				case SIO_PACKET_ERROR_WAIT_START: break; // no start byte yet
				case SIO_PACKET_ERROR_WRONG_LENGTH: Serial.print("max packet len error"); break;
				case SIO_PACKET_ERROR_CHECKSUM: Serial.print("wrong checksum"); break;
				case SIO_PACKET_ERROR_ENDBYTE: Serial.print("wrong end byte"); break;
				case SIO_PACKET_ERROR_OVERFLOW: Serial.print("max packet len overflow error"); break;
				default: Serial.print("unknown error");
			}
		}

		LEDGREEN_INV(); // flip LED when there is incoming data
	} //while
}

void main_task()
{
	while(1)
	{
		uint32_t systime = micros();

		if (systime >= ppm_timer + PPM_TIMEOUT)
		{
			LEDGREEN_OFF();
			#ifndef PPM_NO_TIMEOUT
                        LEDYELLOW_OFF();
			ppm_timeout = 1;
			#endif
		}

		sio_rx_task();
	}
}
