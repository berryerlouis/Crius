/*
 * DrvTick.c
 *
 * Created: 03/07/2012 11:56:56
 *  Author: berryer
 */ 
#include "Conf/ConfHard.h"

#include "DrvTick.h"

////////////////////////////////////////PRIVATE DEFINES/////////////////////////////////////////
#define TIMER0_TICKS_PER_US		2U //	1/16000000MHz * 8 => 2 ticks pour 1 µs

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static volatile Int32U tickCounter = 0UL;

///////////////////////////////////////////PUBLIC FUNCTIONS////////////////////////////////////////
//on init le driver
void DrvTickInit(void)
{	
	tickCounter = 0UL;
	//on init le timer
	TCCR0A	= 0U;
	TCNT0	= 0U;
	//normal mode
	BIT_LOW(TCCR0A,WGM00);
	BIT_LOW(TCCR0A,WGM01);
	BIT_LOW(TCCR0B,WGM02);
	//prescaler 8 * 1/Clk => 0,0000005 s => 0.5µs
	BIT_HIGH(TCCR0B,CS01);
	BIT_LOW(TCCR0B,CS00);
	BIT_HIGH(TIMSK0,TOIE0);
}

//get the tick counter
Int32U DrvTickGetTimeUs(void)
{
	Int32U ovf0;
    Int8U tcnt0;
	
	ATOMIC(	
		ovf0 = tickCounter;
		tcnt0 = TCNT0;
		if ((TIFR0 & _BV(TOV0)) && (tcnt0 < 255))
		{
			ovf0++;
		}
	)
     
	//DrvIoSetPinState( EIO_PIN_A_1, IO_LEVEL_LOW );
    return (Int32U)(((ovf0 << 8U) + tcnt0) / TIMER0_TICKS_PER_US);
}


void DrvTickDelayMs( Int16U delay_ms )
{
	DrvTickDelayUs(delay_ms * 1000UL);
}

void DrvTickDelayUs( Int32U delay_us )
{
	Int32U timeToWait = 0UL;
	timeToWait = (Int32U)(DrvTickGetTimeUs() + (Int32U)delay_us);
	do 
	{
		if(DrvTickGetTimeUs() >= timeToWait)
		{
			break;
		}
	} while (TRUE);
}


///////////////////////////////////////////ISR FUNCTIONS//////////////////////////////////////////
//ISR timer system 0.5µs * 256 =  0.128 ms
ISR(TIMER0_OVF_vect) 
{
	tickCounter += 1UL ;
}

	