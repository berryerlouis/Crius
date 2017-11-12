/*
 * DrvServo.h
 *
 * Created: 28/06/2011 15:53:09
 *  Author: berryer
 */ 


#ifndef DRV_SERVO_H_
#define DRV_SERVO_H_

#include "Tools/tools_typedefs.h"
#include "Drv/DrvPwmSoft.h"
////////////////////////////////////////////PUBLIC DEFINES///////////////////////////////////////////


#define SERVO_RANGE				600			//range 60� for S3003
#define SERVO_ANGLE_MID			900U		//mid position always 1500탎
#define SERVO_ANGLE_MIN			0U
#define SERVO_ANGLE_MAX			1800U

#define PULSE_WIDTH_MIN			500U	//탎
#define PULSE_WIDTH_MID			1400U	//탎
#define PULSE_WIDTH_MAX			2300U	//탎

#define PERIOD_SERVO_MAX		15000U  //탎




/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init of Drv Servo 
Boolean DrvServoInit( void ) ;

// move the selected servo to the desired position [0,1800]
Boolean DrvServoSetPosition( Int8U index, Int16U angle ) ;

// get the position to the selected servo 
Int16U DrvServoGetTicks( Int8U index ) ;

// get the servo strucutre
PwmSoft* DrvServoGetStruture( Int8U index ) ;


#endif /* DRV_SERVO_H_ */