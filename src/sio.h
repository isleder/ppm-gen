#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"	

#ifndef SIO_H
#define SIO_H

#define SIO_PACKET_START_BYTE		0x01
#define SIO_PACKET_END_BYTE			0x04

#define SIO_PACKET_ERROR_WAIT_START 	-1
#define SIO_PACKET_ERROR_WRONG_LENGTH 	-2
#define SIO_PACKET_ERROR_CHECKSUM 		-3	
#define SIO_PACKET_ERROR_ENDBYTE 		-4
#define SIO_PACKET_ERROR_OVERFLOW 		-5

#define ACK                         0x01
#define NACK                        0x00

/* 
parses serial data coming through uart result is in packet 

	packet format:
	0 - 0x01 start byte
	1 - n packet length
	2 - packet type
	data
	n - 2 - checksum
	n - 1 - 0x04 end byte	

return  < 0 - error -- error codes above
		0 - data parsed, but no result yet
		> 0 - succes. return payload length 
*/		
int8_t sio_rx_data(uint8_t b, packet_t *packet);


#endif

#ifdef __cplusplus
}
#endif
