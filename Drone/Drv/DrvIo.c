////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvIo.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////
static const SIoPin io_table[ EIO_NB_PIN ] =
{
	#if defined (__AVR_ATmega324P__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega1284__)
		{ &PORTA, &DDRA, &PINA, 0U},
		{ &PORTA, &DDRA, &PINA, 1U},
		{ &PORTA, &DDRA, &PINA, 2U},
		{ &PORTA, &DDRA, &PINA, 3U},
		{ &PORTA, &DDRA, &PINA, 4U},
		{ &PORTA, &DDRA, &PINA, 5U},
		{ &PORTA, &DDRA, &PINA, 6U},
		{ &PORTA, &DDRA, &PINA, 7U},
		{ &PORTB, &DDRB, &PINB, 0U},
		{ &PORTB, &DDRB, &PINB, 1U},
		{ &PORTB, &DDRB, &PINB, 2U},
		{ &PORTB, &DDRB, &PINB, 3U},
		{ &PORTB, &DDRB, &PINB, 4U},
		{ &PORTB, &DDRB, &PINB, 5U},
		{ &PORTB, &DDRB, &PINB, 6U},
		{ &PORTB, &DDRB, &PINB, 7U},
		{ &PORTC, &DDRC, &PINC, 0U},
		{ &PORTC, &DDRC, &PINC, 1U},
		{ &PORTC, &DDRC, &PINC, 2U},
		{ &PORTC, &DDRC, &PINC, 3U},
		{ &PORTC, &DDRC, &PINC, 4U},
		{ &PORTC, &DDRC, &PINC, 5U},
		{ &PORTC, &DDRC, &PINC, 6U},
		{ &PORTC, &DDRC, &PINC, 7U},
		{ &PORTD, &DDRD, &PIND, 0U},
		{ &PORTD, &DDRD, &PIND, 1U},
		{ &PORTD, &DDRD, &PIND, 2U},
		{ &PORTD, &DDRD, &PIND, 3U},
		{ &PORTD, &DDRD, &PIND, 4U},
		{ &PORTD, &DDRD, &PIND, 5U},
		{ &PORTD, &DDRD, &PIND, 6U},
		{ &PORTD, &DDRD, &PIND, 7U},
	#endif
};
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////


////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
Boolean DrvIoSetPinOutput ( EIoPin pin ) 
{
	Boolean o_success = FALSE;
	
	if( pin < EIO_NB_PIN )
	{
		*(io_table[ pin ].ddr) |= (1 << io_table[ pin ].bit);
		o_success = TRUE;
	}
	return o_success;
}

Boolean DrvIoSetPinInput ( EIoPin pin ) 
{
	Boolean o_success = FALSE;
	if( pin < EIO_NB_PIN )
	{
		*(io_table[ pin ].ddr) &=~ (1 << io_table[ pin ].bit);
		*(io_table[ pin ].port) &=~ (1 << io_table[ pin ].bit);
		o_success = TRUE;
	}
	return o_success;
}

EIOLevel DrvIoGetPinState ( EIoPin pin )
{
	EIOLevel pin_value = IO_LEVEL_LOW;

	pin_value = *(io_table[pin].pin);
	pin_value = pin_value >> io_table[pin].bit;
	pin_value &= 0x01U;

	return pin_value ;
}


void DrvIoSetPinState ( EIoPin pin , EIOLevel level)
{
	EIOLevel not_level = ((EIOLevel)(~level)) & 0x01U ;

	*(io_table[pin].port) &= (Int8U)(~(Int8U)(not_level << (Int8U)io_table[pin].bit));
	*(io_table[pin].port) |= (Int8U)( level << (Int8U)io_table[pin].bit );
}

void DrvIoTogglePinState( EIoPin pin )
{
	*(io_table[pin].port) ^= (1U << io_table[pin].bit);
}