/*
 * tools_typedefs.h
 *
 * Created: 28/06/2011 15:55:05
 *  Author: berryer
 */ 


#ifndef TOOLS_TYPEDEFS_H_
#define TOOLS_TYPEDEFS_H_

#include <util/atomic.h>
#include <avr/interrupt.h> 
#include <avr/pgmspace.h>
#include <avr/wdt.h> 
#include <string.h>

// Unsigned 8 bits char
typedef char Char;

// Unsigned 8 bits integer type
typedef unsigned char Int8U;
// Unsigned 16 bits integer type
typedef unsigned int Int16U;
// Unsigned 32 bits integer type
typedef unsigned long Int32U;

// Signed 8 bits integer type
typedef signed char Int8S;
// Signed 16 bits integer type
typedef signed int Int16S;
// Signed 32 bits integer type
typedef signed long Int32S;

// Boolean type
typedef Int8U Boolean;
#define FALSE       0U
#define TRUE        1U
#define	ENABLE		TRUE
#define	DISABLE		FALSE
#define	ARMED		TRUE
#define	DISARMED	FALSE

#define NB_AXIS		3U
typedef struct
{
	union 
	{
		struct
		{
			Int16S x;
			Int16S y;
			Int16S z;
		};
		Int16S axes[NB_AXIS];
	};
}Axis;

typedef struct
{
	union
	{
		struct
		{
			Int32S x;
			Int32S y;
			Int32S z;
		};
		Int32S axes[NB_AXIS];
	};
}Axis32;

typedef struct
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float axes[NB_AXIS];
	};
}AxisFloat;

//buffer def
typedef struct
{
	Int8U *buffer;
	Int16U index;
	Int16U nbData;
	Int16U maxSize;
}Buffer;


// Null pointer
#ifndef NULL
#define NULL     ((void *)0U)
#endif

#define UNUSED(x) (void)(x)
   
#ifndef ATOMIC
#define ATOMIC(code) \
						Int8U sreg = SREG;  \
						cli();\
						do{ code }while(0);\
						SREG = sreg;
#endif




static inline float SetRange(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static inline float SetRangeInt16(Int16U x, Int16U in_min, Int16U in_max, Int16U out_min, Int16U out_max)
{
	return ((Int32U)(x - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min;
}

static inline float SetLimits(float val, float min, float max) 
{ 
	if(val > max)
	{
		val = max;
	}
	else if(val < min)
	{
		val = min;
	}
	return val;
}	

static inline Int16S SetLimitsInt16S(Int16S val, Int16S min, Int16S max)
{
	if(val > max)
	{
		val = max;
	}
	else if(val < min)
	{
		val = min;
	}
	return val;
}

static inline Int16U SetLimitsInt16U(Int16U val, Int16U min, Int16U max)
{
	if(val > max)
	{
		val = max;
	}
	else if(val < min)
	{
		val = min;
	}
	return val;
}

static inline void swap_endianness(void *buf, Int8U size) 
{
  /* we swap in-place, so we only have to
  * place _one_ element on a temporary tray
  */
  Int8U tray;
  Int8U *from;
  Int8U *to;
  /* keep swapping until the pointers have assed each other */
  for (from = (Int8U*)buf, to = &from[size-1]; from < to; from++, to--) {
    tray = *from;
    *from = *to;
    *to = tray;
  }
}
/*
 * Basic Macros
 */
#define ToDeg(x) (x * 57.2957795F)		//	180/M_PI  
#define ToRad(x) (x * 0.0174532925F)	//	M_PI/180

#define BIT_READ( reg , bit )		(reg  & _BV(bit)) > 0U ? 1U : 0U
#define BIT_HIGH( reg , bit )		reg |=  _BV(bit)
#define BIT_LOW( reg , bit )		reg &= ~_BV(bit)
#define BIT_TOGGLE( reg , bit )		reg ^=  _BV(bit)
#define BIT_SET( reg , bit , val)							\
									if( val )				\
									{						\
										BIT_HIGH( reg , bit);\
									}						\
									else					\
									{						\
										BIT_LOW( reg , bit);	\
									};


						
#define MultiS16X16to32(longRes, intIn1, intIn2) \
asm volatile ( \
"clr r26 \n\t" \
"mul %A1, %A2 \n\t" \
"movw %A0, r0 \n\t" \
"muls %B1, %B2 \n\t" \
"movw %C0, r0 \n\t" \
"mulsu %B2, %A1 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"mulsu %B1, %A2 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (longRes) \
: \
"a" (intIn1), \
"a" (intIn2) \
: \
"r26" \
)		
						

/*
 * Basic types for the microcontroler
 */
#define ABS(X)						((X) > 0 ? (X) : -(X))    
#define MID(X,limitdown,limitup)	((X > limitdown ) && (X < limitup ))
#define MIN(A,B)					(((A)<(B)) ? (A) : (B) )
#define MAX(A,B)					(((A)>(B)) ? (A) : (B) )

//definit un type de pointeur de fonction pour abstraire les interruptions micro
typedef void (*ptrfct_Isr_Callback)(void);
//defini un pointeur vers une fct null, reset du micro
#define RESET_SOFT() ptrfct_Isr_Callback ptrfct_null = NULL; ptrfct_null();





#endif /* TOOLS_TYPEDEFS_H_ */