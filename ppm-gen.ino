/********************************************
 * @author Istvan Sleder (4/18/2014)
 * @email istvan.sleder@gmail.com
*********************************************/

#include <Arduino.h>
#include "src/config.h"
#include "src/main.h"

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
