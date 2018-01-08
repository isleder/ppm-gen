#ifdef __cplusplus
extern "C" {
#endif

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include "config.h"	

typedef struct packet_tag
{
	uint8_t type;
	uint8_t length;	
	uint8_t packetid;
	uint8_t buff[SIO_MAX_PACKET_LENGTH];

} packet_t;

// double buffered 
// ppm channels
// interrupt read from active buffer
// task writes to not active buffer
typedef struct ppm_channel_buff_tag
{
	uint8_t count;
	uint16_t channels[PPM_CHANNEL_COUNT_MAX];

} ppm_channel_buff_t;

#endif

#ifdef __cplusplus
}
#endif