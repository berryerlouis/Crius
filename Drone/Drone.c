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

	// ********************* Interrupt Disable ****************************************
	DrvInterruptClearAllInterrupts();
	
	// ********************* WatchDog Enable ******************************************
	wdt_enable(WDTO_1S);
	
	// ********************* Drivers system init **************************************
	DrvTickInit();
	DrvTimerInit();
	DrvEepromInit();
	DrvAdcInit();
	
	// ********************* Interrupt Enable *****************************************
	DrvInterruptSetAllInterrupts();
	
	// ********************* Services init ********************************************
	//SrvSensorsInit();
	//SrvIhmInit();
	//SrvMultiWiiInit();
	//SrvPIDInit();
	//SrvImuInit();
	SrvMotorsInit();
	
	// ********************* Start IHM ************************************************
	SrvIhmPlatformInitStart() ;
	
	// ********************* Reset time ***********************************************
	DrvTimerTickReset();
	lastLoopTime = DrvTickGetTimeUs();
	
	// ********************* Led ok blink mode ****************************************
	SrvIhmPlatformInitDone();
	
	
	//DrvIoSetPinOutput( E_ADC_PIN_0 );
	static Int16U toto[4] = {600,900,1200,900};
	static Int8U titi = 0;
	static Int32U time = 0;
    while(TRUE)
    {		
		// ********************* Watchdog ********************************************* 
		wdt_enable(WDTO_15MS);
		
		// ********************* Calcul du temps de cycle *****************************
		Int32U now = DrvTickGetTimeUs();
		imu.status.loopTime = now - lastLoopTime;
		lastLoopTime = now;
		
		// ********************* Calcul du temps de vie en sec ************************
		imu.status.lifeTime = now / 1000000U;
		
		if(time < now)
		{
			time = now + 1000000;
			//DrvServoSetPosition(0,tata);
			DrvServoSetPosition(0,toto[titi++]);
			if(titi == 4)titi=0;
		}
		/*
		// ********************* Read sensors *****************************************
		SrvSensorsDispatcher();		//1ms
		// ********************* IHM **************************************************
		SrvIhmDispatcher();			//100탎
		// ********************* Compute sensors **************************************
		SrvImuDispatcher();			//500탎
		// ********************* PID compute ******************************************
		SrvPIDDispatcher();			//500탎
		// ********************* Update motors ******* ********************************
		SrvMotorsDispatcher();		//120탎
		// ********************* Receive transmit data ********************************
		SrvMultiWiiDispatcher();	//120탎
		*/
		
		
		//toto = DrvAdcRead(E_ADC_PIN_0);
	}	
}























/*FUSES = 
    {
        .low = LFUSE_DEFAULT,
        .high = (FUSE_SPIEN & FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_EESAVE  & FUSE_JTAGEN)
        .extended = EFUSE_DEFAULT,
    };*/

