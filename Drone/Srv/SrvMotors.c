/*
 * SrvMotors.c
 *
 * Created: 31/05/2012 16:04:33
 *  Author: berryer
 */ 

/////////////////////////////////////////////INCLUDES/////////////////////////////////////////////
#include "Conf/ConfHard.h"

#include "Drv/DrvEeprom.h"
#include "Drv/DrvServo.h"

#include "SrvMotors.h"
#include "SrvPID.h"
#include "SrvImu.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define POWER_TO_ANGLE(power) (Int16U)(SetRange( power , MOTOR_POWER_MIN		,MOTOR_POWER_MAX	,SERVO_ANGLE_MIN	,SERVO_ANGLE_MAX ) ) 
////////////////////////////////////////PRIVATE STRUCTIURES///////////////////////////////////////

////////////////////////////////////////PRIVATE FONCTIONS/////////////////////////////////////////

////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Motors motors;




/////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
/************************************************************************/
/* init des moteurs                                                     */
/************************************************************************/
Boolean SrvMotorsInit ( void ) 
{
	//init des variateurs brushless
	DrvServoInit();
		
	Boolean eep_config = DrvEepromIsConfigured();
	if( eep_config == TRUE )
	{
		//on lit les valeurs 
		DrvEepromReadStartupMotorFrontRight( &motors.frontRight.startup );
		DrvEepromReadStartupMotorRearRight( &motors.rearRight.startup );
		DrvEepromReadStartupMotorFrontLeft( &motors.frontLeft.startup );
		DrvEepromReadStartupMotorRearLeft( &motors.rearLeft.startup );
	}
	else
	{
		//on ecrit les valeurs nulles
		DrvEepromWriteStartupMotorFrontRight( 0U );
		DrvEepromWriteStartupMotorRearRight( 0U );
		DrvEepromWriteStartupMotorFrontLeft( 0U );
		DrvEepromWriteStartupMotorRearLeft( 0U );
		
		motors.frontRight.startup	= MOTOR_POWER_MIN;
		motors.frontLeft.startup	= MOTOR_POWER_MIN;
		motors.rearRight.startup	= MOTOR_POWER_MIN;
		motors.rearLeft.startup		= MOTOR_POWER_MIN;
	}	
	motors.throttle			= MOTOR_POWER_MIN;
	motors.minThrottle		= MOTOR_POWER_MIN;
	motors.midThrottle		= MOTOR_POWER_MID;
	motors.maxThrottle		= MOTOR_POWER_MAX;
	motors.rearRight.speed	= MOTOR_POWER_MIN;
	motors.frontRight.speed	= MOTOR_POWER_MIN;
	motors.rearLeft.speed	= MOTOR_POWER_MIN;
	motors.frontLeft.speed	= MOTOR_POWER_MIN;
	
	
	motors.rearRight.servo	= DrvServoGetStruture(E_PWM_1);
	motors.frontRight.servo	= DrvServoGetStruture(E_PWM_2);
	motors.rearLeft.servo	= DrvServoGetStruture(E_PWM_3);
	motors.frontLeft.servo	= DrvServoGetStruture(E_PWM_4);
	
	return TRUE;
}	

//get structure of motors
Motors * SrvMotorsGetStruct( void )
{
	return &motors;
}

//dispatcher d'evenements
void SrvMotorsDispatcher (void)
{
	//if armed
	if(imu.status.armed == ARMED)
	{
		// *********************Mise à jour des Moteurs ***************************
		SrvMotorsUpdate( imu.pid_error );
	}
	else
	{
		//on met la vitesse de tout les moteurs à zeros 
		SrvPIDResetValues();
		motors.rearRight.speed	= motors.minThrottle;
		motors.frontRight.speed	= motors.minThrottle;
		motors.rearLeft.speed	= motors.minThrottle;
		motors.frontLeft.speed	= motors.minThrottle;
		SrvMotorsSetSpeed( E_PWM_1 , motors.minThrottle );
		SrvMotorsSetSpeed( E_PWM_2 , motors.minThrottle );
		SrvMotorsSetSpeed( E_PWM_3 , motors.minThrottle );
		SrvMotorsSetSpeed( E_PWM_4 , motors.minThrottle );
	}

}


/************************************************************************/
/*Commande des moteurs en fonction de l'angle                           */
/************************************************************************/
void SrvMotorsUpdate(ErrorPid pid_error)
{
	if( ( motors.throttle > motors.minThrottle) && ( motors.throttle <= motors.maxThrottle) )
	{
		// calcul de la vitesse pour chaque moteur
		#ifdef BI		
			motors.rearRight.speed		= SetLimitsInt16S(motors.throttle 
														- pid_error.roulis 
														+ pid_error.tangage,
														motors.rearRight.startup,
														motors.maxThrottle);
			motors.frontLeft.speed		= SetLimitsInt16S(motors.throttle 
														+ pid_error.roulis 
														- pid_error.tangage,
														motors.frontLeft.startup,
														motors.maxThrottle);
		
			DrvServoSetPosition( 0U , motors.rearRight.speed	 );
			DrvServoSetPosition( 3U , motors.frontLeft.speed	 );
		
		#endif
		
		#ifdef QUADX 
			motors.rearRight.speed		= SetLimitsInt16S(motors.throttle 
														+ pid_error.roulis 
														- pid_error.tangage
														+ pid_error.lacet, 
														motors.rearRight.startup,
														motors.maxThrottle);
			motors.frontRight.speed		= SetLimitsInt16S(motors.throttle 
														+ pid_error.roulis
														+ pid_error.tangage 
														- pid_error.lacet, 
														motors.frontRight.startup,
														motors.maxThrottle);
			motors.rearLeft.speed		= SetLimitsInt16S(motors.throttle 
														- pid_error.roulis
														- pid_error.tangage
														- pid_error.lacet,
														motors.rearLeft.startup,
														motors.maxThrottle);
			motors.frontLeft.speed		= SetLimitsInt16S(motors.throttle 
														- pid_error.roulis 
														+ pid_error.tangage 
														+ pid_error.lacet, 
														motors.frontLeft.startup,
														motors.maxThrottle);
		
			SrvMotorsSetSpeed( E_PWM_1 , motors.rearRight.speed );
			SrvMotorsSetSpeed( E_PWM_2 , motors.frontRight.speed );
			SrvMotorsSetSpeed( E_PWM_3 , motors.rearLeft.speed );
			SrvMotorsSetSpeed( E_PWM_4 , motors.frontLeft.speed );
	
		#endif
	}
	else
	{
		//on met la vitesse de tout les moteurs à zeros 
		SrvPIDResetValues();
		motors.rearRight.speed	= motors.minThrottle;
		motors.frontRight.speed	= motors.minThrottle;
		motors.rearLeft.speed	= motors.minThrottle;
		motors.frontLeft.speed	= motors.minThrottle;
		SrvMotorsSetSpeed( E_PWM_1 , motors.minThrottle );
		SrvMotorsSetSpeed( E_PWM_2 , motors.minThrottle );
		SrvMotorsSetSpeed( E_PWM_3 , motors.minThrottle );
		SrvMotorsSetSpeed( E_PWM_4 , motors.minThrottle );
	}
}	


/************************************************************************/
/*recupere la vitesse des moteurs                                       */
/************************************************************************/
Int16U SrvMotorsGetSpeed( void ) 
{
	return motors.throttle;
}


/************************************************************************/
/*applique une vitesse absolue aux moteurs                              */
/************************************************************************/
Boolean SrvMotorsApplyAbsoluteSpeed(Int16U speed)
{
	Boolean o_success = FALSE;
	if( ( speed >= motors.minThrottle ) &&
		( speed <= motors.maxThrottle ))
	{
		motors.throttle = speed;
		o_success = TRUE;
	}
	return o_success;
}

/************************************************************************/
/*applique une vitesse relative aux moteurs                             */
/************************************************************************/
Boolean SrvMotorsApplyRelativeSpeed(Int16S speed)
{
	Boolean o_success = FALSE;
	//la somme doit etre inferieur 
	if( (motors.throttle + speed) <= motors.maxThrottle )
	{
		motors.throttle += speed;
		o_success = TRUE;
	}
	return o_success;
}
/************************************************************************/
/*configure la vitesse des moteurs                                       */
/************************************************************************/
void SrvMotorsSetSpeed( Int8U motorIndex, Int16U speed )
{
	DrvServoSetPosition( motorIndex , POWER_TO_ANGLE(speed) );
}
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

