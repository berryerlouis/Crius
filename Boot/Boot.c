/*
 * Boot.c
 *
 * Created: 04/02/2017 18:19:08
 * Author : berryer
 */ 

/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/boot.h>
#include <avr/io.h>

#include "Conf/ConfHard.h"
#include "Drv/DrvUart.h"
#include "Drv/DrvInterrupt.h"
#include "Drv/DrvTimer.h"
#include "Drv/DrvLed.h"
#include "Drv/DrvEeprom.h"
#include "Drv/DrvBootload.h"



/////////////////////////////////////////PRIVATE FUNSTIONS////////////////////////////////////////

int main(void)
{

	// Drv Init
	DrvBootloadBootInit();

    while (1) 
    {	
		//reset wdg
		wdt_enable(WDTO_15MS);
		
		DrvBootloadDispatcher();
    }
}

