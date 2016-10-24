/*
 * DrvServo.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf\ConfHard.h"
#include "Drv/DrvServo.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

//compute convert
#define ConvertAngleToUs(angle)		SetRangeInt16(angle,SERVO_ANGLE_MIN,SERVO_ANGLE_MAX, PULSE_WIDTH_MIN,PULSE_WIDTH_MAX )
#define ConvertUsToAngle(us)		SetRangeInt16(us,PULSE_WIDTH_MIN,PULSE_WIDTH_MAX, SERVO_ANGLE_MIN,SERVO_ANGLE_MAX )

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init of Drv Servo 
Boolean DrvServoInit( void )
{
	//init PWM to the configured period
	DrvPwmInit(TICKS_PERIOD_SERVO);
	for(Int8U indexPwm = 0U; indexPwm < E_NB_PWMS ; indexPwm++)
	{		
		DrvPwmSetWidthMin(indexPwm,PULSE_WIDTH_MIN);
		DrvPwmSetWidthMid(indexPwm, PULSE_WIDTH_MID);
		DrvPwmSetWidthMax(indexPwm,PULSE_WIDTH_MAX);
		DrvPwmSetWidth(indexPwm,PULSE_WIDTH_MIN);
		DrvPwmSetEnable(indexPwm,TRUE);
	}
	return TRUE;             
}

// move the selected servo to the desired position [0,1800]
Boolean DrvServoSetPosition( Int8U index, Int16U angle)
{
	angle = SetLimitsInt16U(angle,SERVO_ANGLE_MIN,SERVO_ANGLE_MAX);
	return DrvPwmSetWidth(index,ConvertAngleToUs(angle));
}

// get the position to the selected servo
Int16U DrvServoGetTicks( Int8U index ) 
{
	return ConvertUsToAngle(DrvPwmGetWidth(index));
}


// get the servo strucutre
Pwm* DrvServoGetStruture( Int8U index ) 
{
	return DrvPwmGetStruture(index);
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

///////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////
