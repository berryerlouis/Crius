/*
 * DrvPwn.h
 *
 * Created: 23/10/2016 12:58:24
 *  Author: berryer
 */ 


#ifndef DRVPWN_H_
#define DRVPWN_H_

#include "Tools/tools_typedefs.h"
#include "Drv/DrvIo.h"
////////////////////////////////////////////PUBLIC DEFINES///////////////////////////////////////////


//TIMER
#define PRESCALER_NO			0U
#define PRESCALER_1				_BV(CS10)
#define PRESCALER_8			    _BV(CS11)
#define PRESCALER_64			(_BV(CS11) | _BV(CS10))
#define PRESCALER_256			_BV(CS12)
#define PRESCALER_1024			(_BV(CS12) | _BV(CS10))
#define PRESCALER_TX_FALL		(_BV(CS12) | _BV(CS11))
#define PRESCALER_TX_RISE		(_BV(CS12) | _BV(CS11) | _BV(CS10))

#define TIMER1_TICKS_PER_US		2U //	1/16000000MHz * 8 => 2 ticks pour 1 µs

/////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct {
	Boolean enable;
	EIoPin pin;
	Int16U ticks;
	Int16U min;
	Int16U mid;
	Int16U max;
} Pwm;

/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
// Init of Drv PWM
Boolean DrvPwmInit( Int16U period ) ;

// get the width of the selected PWM
Int16U DrvPwmGetWidth( Int8U index ) ;


// set the period of all PWM
Boolean DrvPwmSetPeriod( Int16U period ) ;

// enable or disable the selected PWM
Boolean DrvPwmSetEnable( Int8U index, Boolean enable ) ;
// set the width of the selected PWM
Boolean DrvPwmSetWidth( Int8U index, Int16U width ) ;
// set the minimum width of the selected PWM
Boolean DrvPwmSetWidthMin( Int8U index, Int16U width );
// set the maximum width of the selected PWM
Boolean DrvPwmSetWidthMax( Int8U index, Int16U width );
// set the default width of the selected PWM
Boolean DrvPwmSetWidthMid( Int8U index, Int16U width );

// get the PWM strucutre
Pwm* DrvPwmGetStruture( Int8U index ) ;
#endif /* DRVPWN_H_ */