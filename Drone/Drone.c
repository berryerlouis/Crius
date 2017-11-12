/*
 * Drone.c
 *
 * Created: 03/07/2012 10:06:16
 *  Author: berryer
 */ 

/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"
#include "Drv/DrvTimer.h"
#include "Drv/DrvAdc.h"
#include "Drv/DrvInterrupt.h"
#include "Drv/DrvButton.h"
#include "Drv/DrvLed.h"
#include "Drv/DrvBootload.h"

#include "Srv/SrvKalman.h"
#include "Srv/SrvImu.h"
#include "Srv/SrvIhm.h"
#include "Srv/SrvMultiWii.h"
#include "Srv/SrvStartEngine.h"
#include "Srv/SrvPID.h"
#include "Srv/SrvMotors.h"
#include "Srv/SrvSensors.h"



////////////////////////////////////////PRIVATE FUNCTIONS////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES////////////////////////////////////////
//permits to compute the main loop time
Int32U lastLoopTime = 0U;

// inertial measurement unit
Imu imu;

//fonction principale
int main(void)
{	
	// ********************* Reset Cause **********************************************
	imu.status.lastResetCause = MCUSR;
	//reset status register
	MCUSR = 0U;

	// ********************* Interrupt Vectors ****************************************
	/* Enable change of interrupt vectors */
	MCUCR |= (1<<IVCE);
	/* Move interrupts to app flash section */
	MCUCR &= ~(1<<IVSEL);

	// ********************* Interrupt Disable ****************************************
	DrvInterruptClearAllInterrupts();
	
	// ********************* WatchDog Enable ******************************************
	wdt_enable(WDTO_1S);
	
	// ********************* Drivers system init **************************************
	DrvTickInit();
	DrvTimerInit();
	DrvEepromInit();
	DrvAdcInit();
	DrvButtonInit();
	DrvLedInit();
	
	// ********************* IHM system init ******************************************
	DrvButtonAddButton(E_BUTTON_SETUP);
	DrvLedSetPinLed(E_LED_OK);
	DrvLedSetPinLed(E_LED_WARNING);
	DrvLedSetPinLed(E_LED_ERROR);
	
	// ********************* Interrupt Enable *****************************************
	DrvInterruptSetAllInterrupts();
		
	// ********************* Services init ********************************************
	if( FALSE == SrvSensorsInit() )
	{		
		DrvLedSetState(E_LED_ERROR, E_LED_STATE_ON);
		while(TRUE);
	}
	else
	{
		SrvIhmInit();
		SrvMultiWiiInit();
		SrvPIDInit();
		SrvImuInit();
		SrvMotorsInit();
		
		// ********************* Bootload init ********************************************
		DrvBootloadAppInit();
		
		// ********************* Start IHM ************************************************
		SrvIhmPlatformInitStart() ;
		
		// ********************* Reset time ***********************************************
		DrvTimerTickReset();
		lastLoopTime = DrvTickGetTimeUs();
		
		// ********************* Led ok blink mode ****************************************
		SrvIhmPlatformInitDone();
		
		// ********************* Eeprom is now configured for next startup ****************
		DrvEepromSetConfiguration();
		
		while(TRUE)
		{
			// ********************* Watchdog *********************************************
			wdt_enable(WDTO_15MS);
			
			// ********************* Calcul du temps de cycle *****************************
			Int32U now = DrvTickGetTimeUs();
			imu.status.loopTime = now - lastLoopTime;
			lastLoopTime = now;
			
			// ********************* Calcul du temps de vie en sec ************************
			if( (Int32U)(now / 40000U) - imu.status.lifeTime > 0UL )
			{
				
			}
			imu.status.lifeTime = now / 1000000U;
			
			// ********************* Read sensors *****************************************
			SrvSensorsDispatcher();
			// ********************* IHM **************************************************
			SrvIhmDispatcher();
			// ********************* Compute sensors **************************************
			SrvImuDispatcher();
			// ********************* PID compute ******************************************
			SrvPIDDispatcher();
			// ********************* Update motors ******* ********************************
			SrvMotorsDispatcher();
			// ********************* Receive transmit data ********************************
			SrvMultiWiiDispatcher();
			
		}
	}
}























/*FUSES = 
    {
        .low = LFUSE_DEFAULT,
        .high = (FUSE_SPIEN & FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_EESAVE  & FUSE_JTAGEN)
        .extended = EFUSE_DEFAULT,
    };*/

