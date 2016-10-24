/*
 * SnrVariometer.h
 *
 * Created: 06/07/2012 17:29:08
 *  Author: berryer
 */ 


#ifndef CMPVARIOMETER_H
#define CMPVARIOMETER_H

#include "Conf/ConfHard.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

////////////////////////////////////////INCLUDES//////////////////////////////////////////////////

////////////////////////////////////////PUBLIC DEFINES////////////////////////////////////////////


////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////
typedef struct
{
	Int16S variation;
} VariometerData;



typedef struct
{
	VariometerData *data;
}Variometer;

////////////////////////////////////////PUBLIC FUNCTIONS//////////////////////////////////////////


Boolean SnrVariometerInit				( void );
Boolean SnrVariometerGetVariation     	( void );
VariometerData* SnrVariometerGetStruct	( void );

#endif //CMPVARIOMETER_H