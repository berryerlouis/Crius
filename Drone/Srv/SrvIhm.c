////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvTick.h"
#include "Drv/DrvLed.h"
#include "Drv/DrvButton.h"
#include "Srv/SrvIhm.h"
#include "Srv/SrvMotors.h"
#include "Srv/SrvImu.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define HEARTBEAT_INDEX_MAX 8U
#define HEARTBEAT_PERIOD_MIN 50000U
#define HEARTBEAT_PERIOD_MAX 150000U
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int8U tabHeartbeat[] = { 0U, 0U, 1U, 2U, 3U, 5U, 8U, 12U, 12U };
Int8U indexHeartbeat;
Boolean upDown = FALSE;
Int32U intervalIhm = 0.0F;
Int32U lastreadIhm = 0U;
float periodIhm = HEARTBEAT_PERIOD_MAX;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
//return : TRUE si ok
Boolean SrvIhmInit ( void )
{
    Boolean oSuccess = TRUE;

	
	intervalIhm = 0.0F;
	lastreadIhm = 0U;
	periodIhm = HEARTBEAT_PERIOD_MAX;
	
    return oSuccess;
}


//Fonction de dispatching d'evenements
void SrvIhmDispatcher ( void )
{
	//test the button status
	if(DrvButtonPushed(E_BUTTON_SETUP) == TRUE)
	{
		//toggle ARM status
		if( imu.status.armed == DISARMED )
		{
			//active les moteurs
			SrvIhmPlatformArm();
		}
		else
		{
			//desactive les moteurs
			SrvIhmPlatformDisarm();
		}
	}
}

//platform init start
void SrvIhmPlatformInitStart ( void )
{
	DrvLedSetState(E_LED_OK, E_LED_STATE_ON);
	DrvLedSetState(E_LED_WARNING, E_LED_STATE_ON);
	DrvLedSetState(E_LED_ERROR, E_LED_STATE_ON);
}

//platform init done
void SrvIhmPlatformInitDone ( void )
{
	DrvLedSetState(E_LED_OK, E_LED_STATE_ON);
	DrvLedSetState(E_LED_WARNING, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_ERROR, E_LED_STATE_OFF);
}

//platform Arm
void SrvIhmPlatformArm ( void )
{
	imu.status.armed	= ARMED;
	periodIhm = HEARTBEAT_PERIOD_MIN;
	DrvLedSetBlinkMode(E_LED_OK, 125U, 125U);
	DrvLedSetState(E_LED_WARNING, E_LED_STATE_OFF);
	imu.moteurs->throttle = 100U;
}
//platform Disarm
void SrvIhmPlatformDisarm ( void )
{
	imu.status.armed	= DISARMED;
	periodIhm = HEARTBEAT_PERIOD_MAX;
	DrvLedSetState(E_LED_WARNING, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_OK, E_LED_STATE_ON);
}
//platform Error
void SrvIhmPlatformError ( void ) 
{
	DrvLedSetState(E_LED_OK, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_WARNING, E_LED_STATE_OFF);
	DrvLedSetState(E_LED_ERROR, E_LED_STATE_ON);
}
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
