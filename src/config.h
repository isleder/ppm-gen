/********************************************
 * @author Istvan Sleder (4/18/2014) 
 * @email istvan.sleder@gmail.com
*********************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_H
#define CONFIG_H

#define SERIAL_PORT_BAUD 115200

/**** PORT SETUP ****/	

// Arduino pro micro pins Atmega32U4
#define LEDGREEN_OUT()		DDRD |= (1<<5)
#define LEDGREEN_OFF()		PORTD |= (1<<5)
#define LEDGREEN_ON()		PORTD &= ~(1<<5)
#define LEDGREEN_INV()		PORTD ^= (1<<5)	

#define LEDYELLOW_OUT()		DDRB |= (1<<0)
#define LEDYELLOW_OFF()		PORTB |= (1<<0)
#define LEDYELLOW_ON()		PORTB &= ~(1<<0)
#define LEDYELLOW_INV()		PORTB ^= (1<<0)

#define OC1A_OUT() DDRB |= (1<<5) // ppm out pin PORTB5

/**** PPM ****/
#define PPM_CHANNEL_COUNT_MAX 8
#define SIO_MAX_PACKET_LENGTH (PPM_CHANNEL_COUNT_MAX * 2 + 10)
#define PPM_TIMEOUT	100000 // 100ms to stop pulse output
//#define PPM_NO_TIMEOUT // continue outputting ppm even if there is no incoming packet
//#define PPM_INVERTED // when defined output signal pulse is inverted

/**** TIMINGS ****/
// all of the following values are doubled to get us with 500ns clock
#define PPM_VALUE_MAX 4400 // 2200us
#define PPM_VALUE_MIN 1600 // 800us
#define PPM_INTERVAL 40000 // 20ms
#define PPM_INTERVAL_MAX 50000 // 25ms
#define PPM_PULSE_WIDTH 600 // 300us
#define PPM_SYNC_MIN 8000 // 4ms

#endif

#ifdef __cplusplus
}
#endif	