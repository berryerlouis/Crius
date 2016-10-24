/*
 * SrvComm.c
 *
 * Created: 31/05/2012 16:04:33
 *  Author: berryer
 */ 

#include "Conf/conf_hard.h"

#include "SrvComm.h"
#include "SrvMotors.h"
#include "SrvImu.h"
#include "SrvPID.h"

#include "Drv/DrvUart.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvEeprom.h"

#ifdef DAISY_7
	#include "Cmps/CmpHMC5883.h"
	#include "Cmps/CmpLIS331DLH.h"
	#include "Cmps/CmpBMP085.h"
	#include "Cmps/CmpL3G4200D.h"
	#include "Cmps/CmpVENUS638LPX.h"
#endif // DAISY_7
#ifdef CRIUS
	#include "Cmps/CmpHMC5883.h"
	#include "Cmps/CmpBMA180.h"
	#include "Cmps/CmpBMP085.h"
	#include "Cmps/CmpITG3205.h"
#endif // CRIUS
////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define NB_CHAR_MAX		100U

////////////////////////////////////////PRIVATE FONCTIONS/////////////////////////////////////////
//execute message entrant
static void SrvCommExecute ( void );

//on repporte les donnees
static void SrvCommRepportGeneral( Int8U comm_type_general );

static void SrvCommRepportMotors( Int8U comm_type_motor, Int16U motor_speed );

static void SrvCommRepportAngles( Int8U comm_type_angle, Int16S angles_roulis, Int16S angles_tangage, Int16S angles_lacet);

static void SrvCommRepportSensors( Int8U comm_type_sensor );

static void SrvCommRepportPID( Int8U comm_type_pid, Int8U index, Int16S pid_p, Int16S pid_i, Int16S pid_d );

static void SrvCommWriteAllData( Int16U motor_speed, Int16S angles_roulis, Int16S angles_tangage, Int16S angles_lacet );

static void SrvCommRepportData( void) ;

static void SrvCommRepportError( void) ;

static void SrvCommSendMessage( Char *i_message, Int8U i_message_len );




////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
//passage des params de la trame
/*static STrame ma_trame_comm;*/
static Int8U buffer[ 25 ];
Int8U bufferSize = 0U;
/************************************************************************/
/*init de la communication                                              */
/************************************************************************/
Boolean SrvCommInit (void) 
{
	DrvUartInit( UART_0, UART_SPEED_115200 );
	/*for( Int8U loop = 0U; loop < NB_PARAM ; loop++)
	{
		ma_trame_comm.param[ loop ] = 0U;
	}
	bufferSize = 0;
	for( Int8U loop = 0U; loop < BUFFER_MAX_0 ; loop++)
	{
		buffer[ loop ] = 0U;
	}*/
	return TRUE;
}	

/************************************************************************/
/*dispatcher d'evenements                                               */
/************************************************************************/


void SrvCommDispatcher (Event_t iEvent) 
{
	UNUSED(iEvent);
	//on lit le buffer
	/*if( TRUE == DrvUart0ReadBuffer(buffer ,&bufferSize) )
	{
		//dispatche trame
		SrvCommExecute();
	}*/
	
}
/************************************************************************/
/*execute message entrant                                               */
/************************************************************************/
static void SrvCommExecute ( void )
{
	buffer[ 2U ] = buffer[ 2U ] - 0x30;
	Int16U motor_speed ;
	Int16S angles_roulis ;
	Int16S angles_tangage;
	Int16S angles_lacet ;
	if( buffer[ 2U ] == COMM_GENERAL )
	{
		buffer[ 4U ] = buffer[ 4U ] - 0x30;
		SrvCommRepportGeneral( buffer[ 4U ] );
	}
	else if( buffer[ 2U ] == COMM_MOTORS )
	{
		buffer[ 4U ] = buffer[ 4U ] - 0x30;
		//si on demande le COMM_MOTOR_STARTUP_R_L_WRITE => 10
		if( buffer[ 1U ] == 0x0AU )
		{
			buffer[ 4U ] = (buffer[ 5U ] - 0x30) + 10U;
			motor_speed = (buffer[ 7U ] << 8U) | (buffer[ 8U ]); 
		}
		else
		{
			motor_speed = (buffer[ 6U ] << 8U) | (buffer[ 7U ]); 
		}
		SrvCommRepportMotors(buffer[ 4U ] , motor_speed);
	}
	else if( buffer[ 2U ] == COMM_ANGLES )
	{
		buffer[ 4U ] = buffer[ 4U ] - 0x30;
		angles_roulis = (buffer[ 6U ] << 8U) | (buffer[ 7U ]);
		angles_tangage = (buffer[ 9U ] << 8U) | (buffer[ 10U ]);
		angles_lacet = (buffer[ 12U ] << 8U) | (buffer[ 13U ]);
		SrvCommRepportAngles( buffer[ 4U ], angles_roulis, angles_tangage, angles_lacet);
	}
	else if( buffer[ 2U ] == COMM_SENSORS )
	{
		buffer[ 4U ] = buffer[ 4U ] - 0x30;
		SrvCommRepportSensors( buffer[ 4U ] );
	}
	else if( buffer[ 2U ] == COMM_PID )
	{
		buffer[ 4U ] = buffer[ 4U ] - 0x30;
		Int8U pid_index = buffer[ 6U ] - 0x30;
		Int16S pid_P = (buffer[ 8U ] << 8U) | (buffer[ 9U ]);
		Int16S pid_I = (buffer[ 11U ] << 8U) | (buffer[ 12U ]);
		Int16S pid_D = (buffer[ 14U ] << 8U) | (buffer[ 15U ]);
		SrvCommRepportPID( buffer[ 4U ], pid_index, pid_P, pid_I, pid_D);
	}
	else if( buffer[ 2U ] == COMM_WRITE_ALL )
	{
		motor_speed = (buffer[ 4U ] << 8U) | (buffer[ 5U ]);
		angles_roulis = (buffer[ 7U ] << 8U) | (buffer[ 8U ]);
		angles_tangage = (buffer[ 10U ] << 8U) | (buffer[ 11U ]);
		angles_lacet = (buffer[ 13U ] << 8U) | (buffer[ 14U ]);
		SrvCommWriteAllData(motor_speed, angles_roulis, angles_tangage, angles_lacet);
	}
	else
	{
		SrvCommRepportError();
	}
}	


static void SrvCommRepportError( void ) 
{ 
	Char o_message[ ] = { '*', 0x00 , COMM_ERROR , '*' };
	o_message[ 1U ] = sizeof(o_message);
	SrvCommSendMessage( o_message , sizeof(o_message) );
}

/************************************************************************/
/*on repporte les donnees                                               */
/************************************************************************/
//report general commands
static void SrvCommRepportGeneral( Int8U comm_type_general )
{		
	//read version
	if( comm_type_general == COMM_GENERAL_VERSION)
	{
		//read eeprom
		Int8U version = 0U;
		DrvEepromReadVersion(&version);
		Char o_message[ ] = { '*', 0x00, '1', '+', '1', '+', version, '*' };
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	else if( comm_type_general == COMM_GENERAL_RESET)
	{
		//reset
		Char o_message[ ] = { '*', 0x00, '1', '+', '2', '*' };
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
		//wait for reset
		DrvTickDelayMs(10);
		RESET_SOFT();
	}
	else if( comm_type_general == COMM_GENERAL_FULL_RESET)
	{
		//deconfigure eeprom
		DrvEepromDeconfigure();
		Char o_message[ ] = { '*', 0x00, '1', '+', '3', '*' };
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
		//wait for reset
		DrvTickDelayMs(10);
		RESET_SOFT();
	}
	else
	{
		SrvCommRepportError();
	}
}

//report or write motors values
static void SrvCommRepportMotors( Int8U comm_type_motor, Int16U motor_speed )
{
	if( comm_type_motor == COMM_MOTOR_WRITE)
	{
		//write puissance
		//controle validité data
		if( motor_speed <= MOTOR_POWER_MAX )
		{
			//applique la vitesse au moteurs
			SrvMotorsApplyAbsoluteSpeed(motor_speed);
			Char o_message[ ] = { '*', 0x00, '2', '+', '1', '*'};
			o_message[ 1U ] = sizeof(o_message);
			SrvCommSendMessage( o_message , sizeof(o_message) );
		}
		else
		{
			SrvCommRepportError();
		}
	}
	else if( comm_type_motor == COMM_MOTOR_READ)
	{
		//read motors
		Char o_message[ ] = { '*', 0x00, '2', '+', '2', '+',
							 (Int8U)(imu.moteurs->frontRight.speed >> 8U),
							 (Int8U)imu.moteurs->frontRight.speed,
							 '+',
							 (Int8U)(imu.moteurs->frontLeft.speed >> 8U),
							 (Int8U)imu.moteurs->frontLeft.speed,
							 '+',
							 (Int8U)(imu.moteurs->rearRight.speed >> 8U),
							 (Int8U)imu.moteurs->rearRight.speed,
							 '+',
							 (Int8U)(imu.moteurs->rearLeft.speed >> 8U),
							 (Int8U)imu.moteurs->rearLeft.speed,
							 '+',
							 (Int8U)(imu.moteurs->throttle >> 8U),
							 (Int8U)imu.moteurs->throttle,
							 '*'		 
						 };
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	else if( comm_type_motor == COMM_MOTOR_STARTUP_F_R_READ)	
	{
		Int16U startup = 0U;
		//retourne la valeure startup du moteur
		DrvEepromReadStartupMotorFrontRight( &startup);
		Char o_message[ ] = { '*', 0x00, '2', '+', '3', '+',
			(Int8U)(startup >> 8U),
			(Int8U)startup,
			'*'
		};
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	else if( comm_type_motor == COMM_MOTOR_STARTUP_F_R_WRITE)
	{
		//write puissance
		//controle validité data
		if( motor_speed <= MOTOR_POWER_MAX )
		{
			DrvEepromWriteStartupMotorFrontRight( motor_speed );
			Char o_message[ ] = { '*', 0x00, '2', '+', '4', '*'};
			o_message[ 1U ] = sizeof(o_message);
			SrvCommSendMessage( o_message , sizeof(o_message) );
		}
		else
		{
			SrvCommRepportError();
		}
	}
	else if( comm_type_motor == COMM_MOTOR_STARTUP_R_R_READ)
	{
		Int16U startup = 0U;
		//retourne la valeure startup du moteur
		DrvEepromReadStartupMotorRearRight( &startup );
		Char o_message[ ] = { '*', 0x00, '2', '+', '5', '+',
			(Int8U)(startup >> 8U),
			(Int8U)startup,
			'*'
		};
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	else if( comm_type_motor == COMM_MOTOR_STARTUP_R_R_WRITE)
	{
		//write puissance
		//controle validité data
		if( motor_speed <= MOTOR_POWER_MAX )
		{
			DrvEepromWriteStartupMotorRearRight( motor_speed );
			Char o_message[ ] = { '*', 0x00, '2', '+', '6', '*'};
			o_message[ 1U ] = sizeof(o_message);
			SrvCommSendMessage( o_message , sizeof(o_message) );
		}
		else
		{
			SrvCommRepportError();
		}
	}
	//*****************************
	else if( comm_type_motor == COMM_MOTOR_STARTUP_F_L_READ)
	{
		Int16U startup = 0U;
		//retourne la valeure startup du moteur
		DrvEepromReadStartupMotorFrontLeft( &startup);
		Char o_message[ ] = { '*', 0x00, '2', '+', '7', '+',
			(Int8U)(startup >> 8U),
			(Int8U)startup,
			'*'
		};
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	else if( comm_type_motor == COMM_MOTOR_STARTUP_F_L_WRITE)
	{
		//write puissance
		//controle validité data
		if( motor_speed <= MOTOR_POWER_MAX )
		{
			DrvEepromWriteStartupMotorFrontLeft( motor_speed );
			Char o_message[ ] = { '*', 0x00, '2', '+', '8', '*'};
			o_message[ 1U ] = sizeof(o_message);
			SrvCommSendMessage( o_message , sizeof(o_message) );
		}
		else
		{
			SrvCommRepportError();
		}
	}
	else if( comm_type_motor == COMM_MOTOR_STARTUP_R_L_READ)
	{
		Int16U startup = 0U;
		//retourne la valeure startup du moteur
		DrvEepromReadStartupMotorRearLeft( &startup );
		Char o_message[ ] = { '*', 0x00, '2', '+', '9', '+',
			(Int8U)(startup >> 8U),
			(Int8U)startup,
			'*'
		};
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	else if( comm_type_motor == COMM_MOTOR_STARTUP_R_L_WRITE)
	{
		//write puissance
		//controle validité data
		if( motor_speed <= MOTOR_POWER_MAX )
		{
			DrvEepromWriteStartupMotorRearLeft( motor_speed );
			Char o_message[ ] = { '*', 0x00, '2', '+', '1', '0', '*'};
			o_message[ 1U ] = sizeof(o_message);
			SrvCommSendMessage( o_message , sizeof(o_message) );
		}
		else
		{
			SrvCommRepportError();
		}
	}
	else
	{
		SrvCommRepportError();
	}
}

static void SrvCommRepportAngles( Int8U comm_type_angle, Int16S angles_roulis, Int16S angles_tangage, Int16S angles_lacet)
{
	//report angles
	if( comm_type_angle == COMM_ANGLE_WRITE)
	{
		//applique les angle souhaité
		imuDesire.angles.roulis	= angles_roulis;
		SetLimits((float)imuDesire.angles.roulis, ANGLE_MIN, ANGLE_MAX);
		imuDesire.angles.tangage	= angles_tangage;
		SetLimits((float)imuDesire.angles.tangage, ANGLE_MIN, ANGLE_MAX);
		imuDesire.angles.lacet		= angles_lacet;
		Char o_message[ ] = { '*', 0x00, '3', '+', '1' , '*' };
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	else if( comm_type_angle == COMM_ANGLE_READ)
	{
		Char o_message[ ] = { '*', 0x00, '3', '+', '2',
							(Int8U)(imu.angles.roulis >> 8U),
							(Int8U)imu.angles.roulis,
							'+',
							(Int8U)(imu.angles.tangage >> 8U),
							(Int8U)imu.angles.tangage,
							'+',
							(Int8U)(imu.angles.lacet >> 8U),
							(Int8U)imu.angles.lacet,
							'*'			 
							};
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	else
	{
		SrvCommRepportError();
	}
}

static void SrvCommRepportSensors( Int8U comm_type_sensor )
{
	//report acc angles 
	if( comm_type_sensor == COMM_SENSOR_ACC_READ)
	{		
		Char o_message[ ] = { '*', 0x00, '4', '+', '1',
							(Char)(Int8U)((Int16U)imu.sensors->acc->rawData.x >> 8U),
							(Char)(Int8U)imu.sensors->acc->rawData.x,
							'+',
							(Char)(Int8U)((Int16U)imu.sensors->acc->rawData.y >> 8U),
							(Char)(Int8U)imu.sensors->acc->rawData.y,
							'+',
							(Char)(Int8U)((Int16U)imu.sensors->acc->rawData.z >> 8U),
							(Char)(Int8U)imu.sensors->acc->rawData.z,
							'*'
							};
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	//report gyr angles 
	else if( comm_type_sensor == COMM_SENSOR_GYR_READ)
	{
		Char o_message[ ] = { '*', 0x00, '4', '+', '2',
							(Char)(Int8U)((Int16U)imu.sensors->gyr->rawData.x >> 8U),
							(Char)(Int8U)imu.sensors->gyr->rawData.x,
							'+',
							(Char)(Int8U)((Int16U)imu.sensors->gyr->rawData.y >> 8U),
							(Char)(Int8U)imu.sensors->gyr->rawData.y,
							'+',
							(Char)(Int8U)((Int16U)imu.sensors->gyr->rawData.z >> 8U),
							(Char)(Int8U)imu.sensors->gyr->rawData.z,
							'*'
							};
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	//report mag angles 
	else if( comm_type_sensor == COMM_SENSOR_MAG_READ)
	{
		Char o_message[ ] = { '*', 0x00, '4', '+', '3',
							(Char)(Int8U)((Int16U)imu.sensors->mag->rawData.x >> 8U),
							(Char)(Int8U)imu.sensors->mag->rawData.x,
							'+',
							(Char)(Int8U)((Int16U)imu.sensors->mag->rawData.y >> 8U),
							(Char)(Int8U)imu.sensors->mag->rawData.y,
							'+',
							(Char)(Int8U)((Int16U)imu.sensors->mag->rawData.z >> 8U),
							(Char)(Int8U)imu.sensors->mag->rawData.z,
							'*'
							};
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	//report baro angles 
	else if( comm_type_sensor == COMM_SENSOR_BAR_READ)
	{
		Char o_message[ ] = { '*', 0x00, '4', '+', '4',
							(Char)(Int8U)((Int16U)imu.sensors->bar->altitude >> 8U),
							(Char)(Int8U)imu.sensors->bar->altitude,
							'+',
							(Char)(Int8U)((Int16U)imu.sensors->bar->temperature >> 8U),
							(Char)(Int8U)imu.sensors->bar->temperature,
							'+',
							(Char)(Int8U)(((Int32U)imu.sensors->bar->pressure) >> 24U),
							(Char)(Int8U)(((Int32U)imu.sensors->bar->pressure) >> 16U),
							(Char)(Int8U)(((Int32U)imu.sensors->bar->pressure) >> 8U),
							(Char)(Int8U)((Int32U)imu.sensors->bar->pressure),
							'+',
							(Char)(Int8U)imu.sensors->bar->weather,
							'*'
							};
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	else
	{
		SrvCommRepportError();
	}
}

static void SrvCommRepportPID( Int8U comm_type_pid, Int8U index, Int16S pid_p, Int16S pid_i, Int16S pid_d )
{		
	//report pid
	if( comm_type_pid == COMM_PID_WRITE)
	{
		//applique les PIDs souhaité
		float P = 0;
		float I = 0;
		float D = 0;
		if( index < NB_PID )
		{
			P =  (float)( pid_p / 1000.0 );
			I =  (float)( pid_i / 1000.0 );
			D =  (float)( pid_d / 1000.0 );
			DrvEepromWritePID( index, P, I, D );
			SrvPIDSetValues( index, P, I, D );
			//on renvoie les valeures PID
			DrvEepromReadPID(index,&P,&I,&D);
			Char o_message[ ] = { '*', 0x00, '5', '+', '1', '+',
								index,
								'+',
								(Char)(Int8U)((Int16S)( P * 1000 ) >> 8U),
								(Char)(Int8U)((Int16S)( P * 1000 )),
								'+',
								(Char)(Int8U)((Int16S)( I * 1000 ) >> 8U),
								(Char)(Int8U)((Int16S)( I * 1000 )),
								'+',
								(Char)(Int8U)((Int16S)( D * 1000 ) >> 8U),
								(Char)(Int8U)((Int16S)( D * 1000 )),
								'*'		 
								};
			o_message[ 1U ] = sizeof(o_message);
			SrvCommSendMessage( o_message , sizeof(o_message) );
		}
		else
		{
			SrvCommRepportError();
		}
	}
	else if( comm_type_pid == COMM_PID_READ)
	{	
		float P = 0;
		float I = 0;
		float D = 0;
		
		if( index < NB_PID )
		{
			DrvEepromReadPID(index,&P,&I,&D);
			Char o_message[ ] = { '*', 0x00, '5', '+', '2', '+',
								index,
								'+',
								(Char)(Int8U)((Int16S)( P * 1000 ) >> 8U),
								(Char)(Int8U)((Int16S)( P * 1000 )),
								'+',
								(Char)(Int8U)((Int16S)( I * 1000 ) >> 8U),
								(Char)(Int8U)((Int16S)( I * 1000 )),
								'+',
								(Char)(Int8U)((Int16S)( D * 1000 ) >> 8U),
								(Char)(Int8U)((Int16S)( D * 1000 )),
								'*'		 
								};
			o_message[ 1U ] = sizeof(o_message);
			SrvCommSendMessage( o_message , sizeof(o_message) );
		}
		else
		{
			SrvCommRepportError();
		}
		
	}
	else
	{
		SrvCommRepportError();
	}
}

static void SrvCommWriteAllData( Int16U motor_speed, Int16S angles_roulis, Int16S angles_tangage, Int16S angles_lacet )
{
	if( motor_speed <= MOTOR_POWER_MAX )
	{
		//applique la vitesse au moteurs
		SrvMotorsApplyAbsoluteSpeed(motor_speed);
		Char o_message[ ] = { '*', 0x00, '2', '+', '1', '*'};
		o_message[ 1U ] = sizeof(o_message);
		SrvCommSendMessage( o_message , sizeof(o_message) );
	}
	//applique les angles souhaités
	imuDesire.angles.roulis	= angles_roulis;
	SetLimits((float)imuDesire.angles.roulis, ANGLE_MIN, ANGLE_MAX);
	imuDesire.angles.tangage	= angles_tangage;
	SetLimits((float)imuDesire.angles.tangage, ANGLE_MIN, ANGLE_MAX);
	imuDesire.angles.lacet		= angles_lacet;
	SrvCommRepportData();
	
}

static void SrvCommRepportData( void )
{
	Char o_message[ ] = { '*', 0x00, '6', '+', 
				(Char)(Int8U)((Int16U)imu.angles.roulis >> 8U),
				(Char)(Int8U)imu.angles.roulis,
				(Char)(Int8U)((Int16U)imu.angles.tangage >> 8U),
				(Char)(Int8U)imu.angles.tangage,
				(Char)(Int8U)((Int16U)imu.angles.lacet >> 8U),
				(Char)(Int8U)imu.angles.lacet,
				(Char)(Int8U)((Int16U)imu.sensors->mag->heading >> 8U),
				(Char)(Int8U)imu.sensors->mag->heading,
				(Char)(Int8U)((Int16U)imu.sensors->bar->altitude >> 8U),
				(Char)(Int8U)imu.sensors->bar->altitude,
				(Char)(Int8U)((Int16U)imu.moteurs->throttle >> 8U),
				(Char)(Int8U)imu.moteurs->throttle,
				(Char)(Int8U)((Int16U)imu.moteurs->frontLeft.speed >> 8U),
				(Char)(Int8U)imu.moteurs->frontLeft.speed,
				(Char)(Int8U)((Int16U)imu.moteurs->frontRight.speed >> 8U),
				(Char)(Int8U)imu.moteurs->frontRight.speed,
				(Char)(Int8U)((Int16U)(Int16U)imu.moteurs->rearLeft.speed >> 8U),
				(Char)(Int8U)imu.moteurs->rearLeft.speed,
				(Char)(Int8U)(imu.moteurs->rearRight.speed >> 8U),
				(Char)(Int8U)imu.moteurs->rearRight.speed,
				(Char)(Int8U)((Int16U)imu.sensors->bar->temperature >> 8U),
				(Char)(Int8U)imu.sensors->bar->temperature,
				(Char)(Int8U)(((Int32U)imu.sensors->bar->pressure) >> 24U),
				(Char)(Int8U)(((Int32U)imu.sensors->bar->pressure) >> 16U),
				(Char)(Int8U)(((Int32U)imu.sensors->bar->pressure) >> 8U),
				(Char)(Int8U)((Int32U)imu.sensors->bar->pressure),
				(Char)(Int8U)((Int16U)imu.sensors->acc->rawData.x >> 8U),
				(Char)(Int8U)imu.sensors->acc->rawData.x,
				(Char)(Int8U)((Int16U)imu.sensors->acc->rawData.y >> 8U),
				(Char)(Int8U)imu.sensors->acc->rawData.y,
				(Char)(Int8U)((Int16U)imu.sensors->acc->rawData.z >> 8U),
				(Char)(Int8U)imu.sensors->acc->rawData.z,
				(Char)(Int8U)((Int16U)imu.sensors->gyr->rawData.x >> 8U),
				(Char)(Int8U)imu.sensors->gyr->rawData.x,
				(Char)(Int8U)((Int16U)imu.sensors->gyr->rawData.y >> 8U),
				(Char)(Int8U)imu.sensors->gyr->rawData.y,
				(Char)(Int8U)((Int16U)imu.sensors->gyr->rawData.z >> 8U),
				(Char)(Int8U)imu.sensors->gyr->rawData.z,
				(Char)(Int8U)((Int16U)imu.sensors->mag->rawData.x >> 8U),
				(Char)(Int8U)imu.sensors->mag->rawData.x,
				(Char)(Int8U)((Int16U)imu.sensors->mag->rawData.y >> 8U),
				(Char)(Int8U)imu.sensors->mag->rawData.y,
				(Char)(Int8U)((Int16U)imu.sensors->mag->rawData.z >> 8U),
				(Char)(Int8U)imu.sensors->mag->rawData.z,
				(Char)(Int8U)(((Int32U)imu.loop_time) >> 24U),
				(Char)(Int8U)(((Int32U)imu.loop_time) >> 16U),
				(Char)(Int8U)(((Int32U)imu.loop_time) >> 8U),
				(Char)(Int8U)((Int32U)imu.loop_time),
				'*',
			};
	o_message[ 1U ] = sizeof(o_message);
	SrvCommSendMessage( o_message , sizeof(o_message) );
}


static void SrvCommSendMessage( Char *i_message, Int8U i_message_len )
{
	//DrvUart0SendMessage (i_message , i_message_len );
}