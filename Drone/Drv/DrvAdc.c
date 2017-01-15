////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf\ConfHard.h"

#include "Drv/DrvAdc.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Adc adc[E_NB_ADCS];
////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
//return : TRUE si ok
Boolean DrvAdcInit ( void )
{
	Boolean oSuccess = TRUE;
	//AVCC with external capacitor at AREF pin
	BIT_HIGH(ADMUX,REFS0);
	//single ended input
	BIT_LOW(ADMUX,MUX4);
	BIT_LOW(ADMUX,MUX3);
	
	//free running
	BIT_LOW(ADCSRB,ADTS0);
	BIT_LOW(ADCSRB,ADTS1);
	BIT_LOW(ADCSRB,ADTS2);
	
	//disable digital input reg for ADC pins
	BIT_LOW(DIDR0,ADC0D);
	BIT_LOW(DIDR0,ADC1D);
	BIT_LOW(DIDR0,ADC2D);
	BIT_LOW(DIDR0,ADC3D);
	BIT_HIGH(DIDR0,ADC4D);
	BIT_HIGH(DIDR0,ADC5D);
	BIT_HIGH(DIDR0,ADC6D);
	BIT_HIGH(DIDR0,ADC7D);
	
	//no start now
	BIT_LOW(ADCSRA,ADSC);
	//no auto trigger
	BIT_LOW(ADCSRA,ADATE);
	//flag
	BIT_LOW(ADCSRA,ADIF);
	//disable ADC interrupt
	BIT_LOW(ADCSRA,ADIE);
	//division factor set to 8
	BIT_HIGH(ADCSRA,ADPS0);
	BIT_HIGH(ADCSRA,ADPS1);
	//enable ADC
	BIT_HIGH(ADCSRA,ADEN);
	
	return oSuccess;
}


//Fonction de lecture ADC
Int16S DrvAdcRead ( EIoPin pin )
{
	//only on ADC pins
	if(pin < EIO_PORT_A)
	{		
		ADMUX = ((ADMUX & 0xE0U) | pin);
		BIT_HIGH(ADCSRA,ADSC);
		while(ADCSRA & (1<<ADSC));
	}
	
	//
	adc[E_ADC_1].value = ADC;
	return adc[E_ADC_1].value;		
}


////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////