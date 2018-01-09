#ifdef __cplusplus
extern "C" {
#endif

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include "config.h"

struct packet_t
{
	uint8_t type;
	uint8_t length;
	uint8_t packetid;
	uint8_t buff[SIO_MAX_PACKET_LENGTH];

};

// double buffered
// ppm channels
// interrupt read from active buffer
// task writes to not active buffer
struct ppm_channel_buff_t
{
	uint8_t count;
	uint16_t channels[PPM_CHANNEL_COUNT_MAX];

};

#endif

#ifdef __cplusplus
}
#endif
