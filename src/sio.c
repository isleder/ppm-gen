#include "sio.h"

/*
	parse incoming serial packet

	packet format:
	0 - 0x01 start byte
	1 - n packet length
	2 - packet type
	data
	n - 2 - checksum
	n - 1 - 0x04 end byte	

	output in packet
*/

int8_t sio_rx_data(uint8_t b, packet_t *packet)
{
	static uint8_t bcount;
	static uint8_t checksum; 
	static uint8_t packetlength;		

	if (bcount == 0) 
	{
		if (b != SIO_PACKET_START_BYTE) // wait for start byte
		{
			return SIO_PACKET_ERROR_WAIT_START;
		}
		// found start byte
		checksum = b; 
	}
	else if (bcount == 1) // packet length byte
	{		
		if (b < 5 || b > sizeof(packet->buff)) // wrong packet length			
		{
			bcount = 0; // reset
			return SIO_PACKET_ERROR_WRONG_LENGTH;
		}

		checksum += b;
		packetlength = b;
		packet->length = b - 5; // excludes the type byte
	}
	else if (bcount == 2) // packet type
	{
		packet->type = b;
		checksum += b;
	}
	else if (bcount == packetlength - 2) // checksum byte in packet
	{	
		if (checksum != b) // checksum did not match reset
		{
			bcount = 0;
			return SIO_PACKET_ERROR_CHECKSUM;
		}
	}
	else if (bcount == packetlength - 1) // end byte
	{

		bcount = 0;

		if (b == SIO_PACKET_END_BYTE) // valid packet found (checksum already been tested in previous case)
		{	
			//return packetlength - 5; // head, tail, type, length, checks

			/* in some case there is only 5 bytes in an incoming packet
			< 0 - error
			0 - indicates the packet is not ready yet 
			1 - indicates it is ready
			*/
			return 1; // head, tail, length, checks
		}

		// reset also when the end byte did not match
		return SIO_PACKET_ERROR_ENDBYTE;

	}
	else // all the data bytes
	{
		// bcount is already at 2 after 
		// start byte and length
		if (sizeof(packet->buff) <= bcount - 3)
		{
			return SIO_PACKET_ERROR_OVERFLOW;
		}

		packet->buff[bcount - 3] = b; // remove start, length and type byte
		checksum += b;
	}

	bcount++;

	return 0;

}