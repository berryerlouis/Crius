/*
 * DrvPwm.c
 *
 * Created: 23/10/2016 12:58:07
 *  Author: berryer
 */ 
/*
 * DrvServo.c
 *
 * Created: 28/06/2011 15:52:35
 *  Author: berryer
 */ 
////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf\ConfHard.h"
#include "Drv/DrvPwm.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define OFFSET_TIMING	8U
//compute convert
#define ConvertUsToTicks(us)		(us * TIMER1_TICKS_PER_US)
#define ConvertTicksToUs(ticks)		(ticks / TIMER1_TICKS_PER_US)


////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
//index du servo controllé lors de l'IT
volatile Int8U idPwm = 0U;
static Int16U periodPwm = 0U;

EIoPin pwmPins[ E_NB_PWMS ] = PWM_PINS ;
Pwm pwms[ E_NB_PWMS ];
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////


/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////


// Init of Drv PWM
Boolean DrvPwmInit( Int16U period ) 
{
	//init the structure to the dafault value
	for(Int8U indexPwm = 0U; indexPwm < E_NB_PWMS ; indexPwm++)
	{
		pwms[ indexPwm ].enable = FALSE;
		pwms[ indexPwm ].pin = pwmPins[ indexPwm ];
		pwms[ indexPwm ].ticks = 0U;
		pwms[ indexPwm ].min = 0U;
		pwms[ indexPwm ].mid = 0U;
		pwms[ indexPwm ].max = 0U;
		DrvIoSetPinOutput( pwms[ indexPwm ].pin );
	}
	
	//set idx to 0 at startup
	idPwm = 0U;
	
	//on init le timer 1 tick = 0.5us
	BIT_LOW(TCCR1A,WGM11); //mode normal
	BIT_LOW(TCCR1A,WGM10); //mode normal
	BIT_LOW(TCCR1B,WGM13); //mode normal
	BIT_LOW(TCCR1B,WGM12); //mode normal
	
	BIT_LOW (TIMSK1,OCIE1A);//disable ISR on OCIE1A
	BIT_LOW(TIMSK1,OCIE1B);//disable ISR on OCIE1B
	BIT_LOW (TIMSK1,TOIE1);//disable ISR on TOIE1
	BIT_LOW (TIMSK1,ICIE1);//disable ISR on ICIE1
	
	//set initial period
	DrvPwmSetPeriod(period);
	
	//reset counter to 0
	TCNT1	= 0x0000U;
	return TRUE;
}


// set the period of all PWM
Boolean DrvPwmSetPeriod( Int16U period ) 
{
	periodPwm = period;
	if(periodPwm != 0U)
	{
		OCR1B = periodPwm;
		BIT_LOW (TCCR1B,CS12); //prescaler 8
		BIT_HIGH(TCCR1B,CS11); //prescaler 8
		BIT_LOW (TCCR1B,CS10); //prescaler 8
		BIT_HIGH(TIMSK1,OCIE1B);//enable ISR on OCIE1B
	}
	else
	{
		OCR1B = 0U;
		BIT_LOW (TCCR1B,CS12); //disable
		BIT_LOW (TCCR1B,CS11); //disable
		BIT_LOW (TCCR1B,CS10); //disable
		BIT_LOW(TIMSK1,OCIE1B);//disable ISR on OCIE1B
	}
	return TRUE;
}

// enable or disable the selected PWM
Boolean DrvPwmSetEnable( Int8U index, Boolean enable ) 
{
	pwms[ index ].enable = enable;
	return TRUE;
}
// set the width of the selected PWM
Boolean DrvPwmSetWidth( Int8U index, Int16U width ) 
{	
	if( index < E_NB_PWMS )
	{
		pwms[ index ].ticks = SetLimitsInt16U(ConvertUsToTicks(width),pwms[ index ].min,pwms[ index ].max);
		return TRUE;
	}
	return FALSE;
}
// set the minimum width of the selected PWM
Boolean DrvPwmSetWidthMin( Int8U index, Int16U width )
{
	if( index < E_NB_PWMS )
	{
		pwms[ index ].min = ConvertUsToTicks(width);
		return TRUE;
	}
	return FALSE;
}
// set the maximum width of the selected PWM
Boolean DrvPwmSetWidthMax( Int8U index, Int16U width )
{
	if( index < E_NB_PWMS )
	{
		pwms[ index ].max = ConvertUsToTicks(width);
		return TRUE;
	}
	return FALSE;
}
// set the default width of the selected PWM
Boolean DrvPwmSetWidthMid( Int8U index, Int16U width )
{
	if( index < E_NB_PWMS )
	{
		pwms[ index ].mid = ConvertUsToTicks(width);
		return TRUE;
	}
	return FALSE;
}
// get the width of the selected PWM
Int16U DrvPwmGetWidth( Int8U index ) 
{
	if( index < E_NB_PWMS )
	{
		return pwms[ index ].ticks;
	}
	return FALSE;
}
// get the PWM strucutre
Pwm* DrvPwmGetStruture( Int8U index ) 
{
	if( index < E_NB_PWMS )
	{
		return &pwms[ index ];
	}
	return NULL;
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

///////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////
SIGNAL (TIMER1_COMPB_vect)
{	
	//set servo pin of the last servo to GND 
	DrvIoSetPinState(pwms[ idPwm ].pin, IO_LEVEL_LOW);
	
	//id servo set to zero
	idPwm = 0U;
	
	//if enable
	if(TRUE == pwms[ idPwm ].enable)
	{
		//set first servo pin to VCC
		DrvIoSetPinState(pwms[ idPwm ].pin, IO_LEVEL_HIGH);
	}
		
	//set the next OCR1A to the first servo tick
	OCR1A = pwms[ idPwm ].ticks + TCNT1 + OFFSET_TIMING;
	//servo period
	OCR1B = TCNT1 + periodPwm + OFFSET_TIMING;
			
	//clear flag
	BIT_HIGH(TIFR1,OCF1A);
	//enable ISR on OCIE1A
	BIT_HIGH(TIMSK1,OCIE1A);
}
SIGNAL (TIMER1_COMPA_vect) 
{ 	
	//set servo pin to GND
	DrvIoSetPinState(pwms[ idPwm ].pin, IO_LEVEL_LOW);
	//next servo
	idPwm++;
	if( idPwm < E_NB_PWMS )
	{
		//if enable
		if(TRUE == pwms[ idPwm ].enable)
		{
			//set first servo pin to VCC
			DrvIoSetPinState(pwms[ idPwm ].pin, IO_LEVEL_HIGH);
		}
		//set the next OCR1A to the first servo tick
		OCR1A = pwms[ idPwm ].ticks + TCNT1 + OFFSET_TIMING;
	}
	else
	{
		//disable ISR on OCIE1A
		BIT_LOW(TIMSK1,OCIE1A);
	}
}
