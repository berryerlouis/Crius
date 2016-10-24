/*
 * ConfHard.h
 *
 * Created: 12/07/2011 18:18:08
 *  Author: berryer
 */ 


#ifndef CONF_HARD_H_
#define CONF_HARD_H_

#include <util/atomic.h>
#include <avr/io.h> 
#include <avr/fuse.h> 
#include <avr/eeprom.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Tools/tools_typedefs.h"
#include "Drv/DrvIo.h"

////////////////////////////////////////PUBLIC VERSION /////////////////////////////////////////////
//version 1.0
#define VERSION 0x10

////////////////////////////////////////PUBLIC RESET CAUSE /////////////////////////////////////////
typedef enum
{
	RESET_CAUSE_POWER_ON	= 1U,
	RESET_CAUSE_EXTERNAL	= 2U,
	RESET_CAUSE_BROWN_OUT	= 4U,
	RESET_CAUSE_WATCHDOG	= 8U,
	RESET_CAUSE_JTAG		= 16U,
}E_ResetCause;


////////////////////////////////////////PUBLIC SENSORS BOARD ///////////////////////////////////////
#define DAISY_7		
//#define CRIUS		

////////////////////////////////////////PUBLIC DEVICE TYPE /////////////////////////////////////////
//#define BI
#define QUADX


////////////////////////////////////////PUBLIC FREQUENCE OSC///////////////////////////////////////
#define		CONF_FOSC_MHZ				16UL
#define		CONF_FOSC_HZ				CONF_FOSC_MHZ * 1000000UL
//-----------------------------------------------------------------------------------------------//

////////////////////////////////////////////PUBLIC TIMER///////////////////////////////////////////

//nb timers used
typedef enum
{
	E_TIMER_BUTTON,
	E_TIMER_LED,
	//----------------- Alaways at end -----------------------
	E_NB_TIMER,
	E_TIMER_NONE
}ETimer;


/////////////////////////////////////PUBLIC LED MANAGMENT///////////////////////////////////////////

//nb leds used
typedef enum
{
	E_LED_OK,
	E_LED_WARNING,
	E_LED_ERROR,
	//----------------- Alaways at end -----------------------
	E_NB_LEDS,
	E_LED_NONE
}ELed;

#if defined( DAISY_7 )
	//tableau des boutons actifs
	#define LEDS_PINS			{ EIO_PIN_B_1, EIO_PIN_B_2, EIO_PIN_B_3 }
#elif defined( CRIUS )
	//tableau des boutons actifs
	#define LEDS_PINS			{ EIO_PIN_B_1, EIO_PIN_B_2, EIO_PIN_B_3 }
#endif

/////////////////////////////////////PUBLIC BOUTON MANAGMENT///////////////////////////////////////////

//nb buttons used
typedef enum
{
	E_BUTTON_SETUP,
	//----------------- Alaways at end -----------------------
	E_NB_BUTTONS,
	E_BUTTON_NONE
}EButton;

#if defined( DAISY_7 )
	//tableau des boutons actifs
	#define BUTTONS_PINS			{ EIO_PIN_B_0 }
#elif defined( CRIUS )
	//tableau des boutons actifs
	#define BUTTONS_PINS			{ EIO_PIN_B_0 }
#endif

////////////////////////////////////PUBLIC SERVOS PORTS///////////////////////////////////////////

//nb pwm used 	
typedef enum
{
	E_PWM_1,
	E_PWM_2,
	E_PWM_3,
	E_PWM_4,
	//----------------- Alaways at end -----------------------
	E_NB_PWMS,
	E_PWM_NONE
}EServos;

#if defined( DAISY_7 )
	//tableau des servos actifs
	#define PWM_PINS				{ EIO_PIN_A_0, EIO_PIN_A_1, EIO_PIN_A_2, EIO_PIN_A_3 }
#elif defined( CRIUS )
	//tableau des servos actifs
	#define PWM_PINS				{ EIO_PIN_C_0, EIO_PIN_C_1, EIO_PIN_C_2, EIO_PIN_C_3 }
#endif


/////////////////////////////////////////////PUBLIC ADC////////////////////////////////////////////

#if defined( DAISY_7 )

#define E_ADC_PIN_0_ID	0U
#define E_ADC_PIN_0		EIO_PIN_A_7
#define E_NB_ADCS		1U

#elif defined( CRIUS )

#define E_NB_ADCS	0U

#endif

/////////////////////////////////////////////PUBLIC UART///////////////////////////////////////////

#if defined( DAISY_7 )

#define E_UART_0	0U
#define E_UART_1	1U
#define E_NB_UARTS	2U

#elif defined( CRIUS )

#define E_UART_0	0U
#define E_NB_UARTS	1U

#endif

////////////////////////////////////////PUBLIC SENSORS AVAILABLES //////////////////////////////////
#ifdef DAISY_7
	#define ACC_LIS331DLH
	#define GYR_L3G4200D
	#define MAG_HMC5883
	#define BAR_BMP085
	#define US_SRF04
	#define GPS_VENUS638LPX

	#define ACC		1U
	#define BAR		1U
	#define MAG		1U
	#define GPS		1U
	#define US		1U
#endif
#ifdef CRIUS
	#define ACC_BMA180
	#define GYR_ITG3205
	#define MAG_HMC5883
	#define BAR_BMP085

	#define ACC		1U
	#define BAR		1U
	#define MAG		1U
	#define GPS		0U
	#define US		0U
#endif

//-----------------------------------------------------------------------------------------------//

/////////////////////////////////////PUBLIC MULTIWII CONFIGURATION/////////////////////////////////
#define UART_COM	E_UART_0
//-----------------------------------------------------------------------------------------------//
/////////////////////////////////////PUBLIC GPS CONFIGURATION//////////////////////////////////////
#if GPS == 1U
	#define UART_GPS		E_UART_1	
#endif // 
//-----------------------------------------------------------------------------------------------//

/////////////////////////////////////PUBLIC PID MANAGMENT//////////////////////////////////////////

//nb pid used
typedef enum
{
	PID_ROULIS	,
	PID_TANGAGE	,
	PID_LACET	,
	PID_ALTITUDE,
	//----------------- Alaways at end -----------------------
	E_NB_PIDS,
	E_PID_NONE
}EPid;



#endif /* CONF_HARD_H_ */