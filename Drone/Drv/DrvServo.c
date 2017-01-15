/*
 * DrvServo.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"
#include "Drv/DrvServo.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

//compute convert
#define ConvertAngleToUs(angle)		SetRangeInt16(angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX, PULSE_WIDTH_MIN, PULSE_WIDTH_MAX )
#define ConvertUsToAngle(us)		SetRangeInt16(us, PULSE_WIDTH_MIN, PULSE_WIDTH_MAX, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX )

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

/*Int16U valServo = 0;
Boolean upDown = FALSE;
Int16U servoMin = 0U;
Int16U servoMax = 1800U;
double cosServo = (double)valServo/(double)servoMax;
cosServo *= M_PI;
cosServo = cos(cosServo);
cosServo *= (servoMax / 2U);
cosServo += (servoMax / 2U);
DrvServoSetPosition( E_SOFT_PWM_1 , (Int16U)(cosServo) );
DrvServoSetPosition( E_SOFT_PWM_2 , servoMax - (Int16U)(cosServo) );
//DrvServoSetPosition( E_SOFT_PWM_3 , 1800U-toto );
//DrvServoSetPosition( E_SOFT_PWM_4 , toto );
			
if(!upDown)
{
	valServo += 2U;
	if(valServo == servoMax)
	{
		upDown = TRUE;
	}
}
else
{
	valServo -= 2U;
	if(valServo == servoMin)
	{
		upDown = FALSE;
	}
}*/

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init of Drv Servo 
Boolean DrvServoInit( void )
{
	//init PWM to the configured period
	DrvPwmSoftInit(PERIOD_SERVO_MAX);
	for(Int8U indexPwm = 0U; indexPwm < E_NB_SOFT_PWMS ; indexPwm++)
	{		
		DrvPwmSoftSetWidthMin( indexPwm, PULSE_WIDTH_MIN );
		DrvPwmSoftSetWidthMid( indexPwm, PULSE_WIDTH_MID );
		DrvPwmSoftSetWidthMax( indexPwm, PULSE_WIDTH_MAX );
		DrvPwmSoftSetWidth( indexPwm, PULSE_WIDTH_MIN );
		DrvPwmSoftSetEnable( indexPwm, TRUE );
	}
	return TRUE;             
}

// move the selected servo to the desired position [0,1800]
Boolean DrvServoSetPosition( Int8U index, Int16U angle)
{
	angle = SetLimitsInt16U( angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX );
	return DrvPwmSoftSetWidth( index, ConvertAngleToUs(angle) );
}

// get the position to the selected servo
Int16U DrvServoGetTicks( Int8U index ) 
{
	return ConvertUsToAngle( DrvPwmSoftGetWidth( index ) );
}


// get the servo strucutre
PwmSoft* DrvServoGetStruture( Int8U index ) 
{
	return DrvPwmSoftGetStruture( index );
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

///////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////
