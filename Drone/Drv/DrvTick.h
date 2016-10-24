/*
 * DrvTick.h
 *
 * Created: 03/07/2012 11:57:04
 *  Author: berryer
 */ 


#ifndef DRVTICK_H_
#define DRVTICK_H_

#include "Conf/ConfHard.h"
#include "Tools/tools_typedefs.h"

//on init le driver
void DrvTickInit(void);

//on reset les compteurs
void DrvTickReset(void);

//get the tick counter in microseconds
Int32U DrvTickGetTimeUs(void);

//wait for delay
void DrvTickDelayUs( Int32U delay_us );
void DrvTickDelayMs( Int16U delay_ms );

#endif /* DRVTICK_H_ */