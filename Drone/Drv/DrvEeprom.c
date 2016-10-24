////////////////////////////////////////INCLUDES//////////////////////////////////////////////////
#include "Conf\ConfHard.h"

#include "DrvEeprom.h"

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////
#define ADDR_EEPROM_CHECK_EEPROM	( Int8U *)0U
#define ADDR_EEPROM_VERSION			( Int8U *)1U
#define ADDR_EEPROM_ACC_CALIB_X		( Int8U *)2U
#define ADDR_EEPROM_ACC_CALIB_Y		( Int8U *)4U
#define ADDR_EEPROM_ACC_CALIB_Z		( Int8U *)6U
#define ADDR_EEPROM_GYR_CALIB_X		( Int8U *)8U
#define ADDR_EEPROM_GYR_CALIB_Y		( Int8U *)10U
#define ADDR_EEPROM_GYR_CALIB_Z		( Int8U *)12U
#define ADDR_EEPROM_MAG_CALIB_X		( Int8U *)14U
#define ADDR_EEPROM_MAG_CALIB_Y		( Int8U *)16U
#define ADDR_EEPROM_MAG_CALIB_Z		( Int8U *)18U
#define ADDR_EEPROM_PID_X_P			( Int8U *)20U
#define ADDR_EEPROM_PID_X_I			( Int8U *)24U
#define ADDR_EEPROM_PID_X_D			( Int8U *)28U
#define ADDR_EEPROM_PID_Y_P			( Int8U *)32U
#define ADDR_EEPROM_PID_Y_I			( Int8U *)36U
#define ADDR_EEPROM_PID_Y_D			( Int8U *)40U
#define ADDR_EEPROM_PID_Z_P			( Int8U *)44U
#define ADDR_EEPROM_PID_Z_I			( Int8U *)48U
#define ADDR_EEPROM_PID_Z_D			( Int8U *)52U
#define ADDR_EEPROM_PID_ALT_P		( Int8U *)56U
#define ADDR_EEPROM_PID_ALT_I		( Int8U *)60U
#define ADDR_EEPROM_PID_ALT_D		( Int8U *)64U
#define ADDR_EEPROM_ALTITUDE		( Int8U *)66U
#define ADDR_EEPROM_MOTOR_F_L		( Int8U *)70U
#define ADDR_EEPROM_MOTOR_F_R		( Int8U *)72U
#define ADDR_EEPROM_MOTOR_R_L		( Int8U *)74U
#define ADDR_EEPROM_MOTOR_R_R		( Int8U *)76U


#define VAL_EEPROM_CHECK_OK			0U
#define VAL_EEPROM_CHECK_NOK		0xFFU

#define EEPROM_WRITE_ENABLE			TRUE


////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//Fonction de lecture eeprom
static Int8U DrvEepromReadByte ( const Int8U * addr );
//Fonction d ecriture eeprom
static void DrvEepromWriteByte ( Int8U * addr, Int8U byte);
//Fonction de lecture eeprom
static Int16U DrvEepromReadShort( const Int8U * addr );
//Fonction d ecriture eeprom
static void DrvEepromWriteShort ( Int8U *addr, Int16U byte);
//Fonction de lecture eeprom
static float DrvEepromReadFloat( const Int8U * addr );
//enregistre les config du pid
static void DrvEepromWriteFloat ( Int8U *addr, float value);
////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
static Int8U val = 0U;

////////////////////////////////////////PUBILC FUNCTIONS//////////////////////////////////////////
//Fonction d'initialisation
//return : TRUE si ok
Boolean DrvEepromInit ( void )
{
	Boolean oSuccess = FALSE;
	
	val = DrvEepromReadByte( ADDR_EEPROM_CHECK_EEPROM );
	if( val == VAL_EEPROM_CHECK_OK )
	{
		oSuccess = TRUE;	
	}
	else
	{
		//on ecrit la version 
		DrvEepromWriteByte(ADDR_EEPROM_VERSION, VERSION);
	}

	return oSuccess;
}

//ecrit l'etat de config de l'eeprom
void DrvEepromDeconfigure ( void )
{	
	for(Int16U loop = 0U ; loop < 1024U ; loop++)
	{
		DrvEepromWriteByte(( Int8U *)loop,VAL_EEPROM_CHECK_NOK);
	}
}

//ecrit l'etat de config de l'eeprom
void DrvEepromSetConfiguration ( Boolean configuration )
{
	if( TRUE == configuration)
	{
		DrvEepromWriteByte(ADDR_EEPROM_CHECK_EEPROM,VAL_EEPROM_CHECK_OK);
	}
	else
	{
		DrvEepromWriteByte(ADDR_EEPROM_CHECK_EEPROM,VAL_EEPROM_CHECK_NOK);
	}
}

//retourne l'etat de config de l'eeprom
Boolean DrvEepromIsConfigured ( void )
{
	return DrvEepromReadByte( ADDR_EEPROM_CHECK_EEPROM ) == VAL_EEPROM_CHECK_OK ? TRUE : FALSE ;
}

//retourne le numero de version
void DrvEepromReadVersion ( Int8U *version )
{
	version[ 0U ] = DrvEepromReadByte( ADDR_EEPROM_VERSION );
}

//retourne les config de l'accelerometre
void DrvEepromReadAcc ( Int16S calib[ 3U ] )
{
	calib[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_ACC_CALIB_X);
	calib[ 1U ] = DrvEepromReadShort(ADDR_EEPROM_ACC_CALIB_Y);
	calib[ 2U ] = DrvEepromReadShort(ADDR_EEPROM_ACC_CALIB_Z);
}

//enregistre les config de l'accelerometre
void DrvEepromWriteAcc ( Int16S calib[ 3U ] )
{
	DrvEepromWriteShort(ADDR_EEPROM_ACC_CALIB_X,calib[ 0U ]);
	DrvEepromWriteShort(ADDR_EEPROM_ACC_CALIB_Y,calib[ 1U ]);
	DrvEepromWriteShort(ADDR_EEPROM_ACC_CALIB_Z,calib[ 2U ]);
}

//retourne les config du gyroscope
void DrvEepromReadGyr ( Int16S calib[ 3U ] )
{
	calib[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_GYR_CALIB_X);
	calib[ 1U ] = DrvEepromReadShort(ADDR_EEPROM_GYR_CALIB_Y);
	calib[ 2U ] = DrvEepromReadShort(ADDR_EEPROM_GYR_CALIB_Z);
}

//enregistre les config du gyroscope
void DrvEepromWriteGyr ( Int16S calib[ 3U ] )
{
	DrvEepromWriteShort(ADDR_EEPROM_GYR_CALIB_X,calib[ 0U ]);
	DrvEepromWriteShort(ADDR_EEPROM_GYR_CALIB_Y,calib[ 1U ]);
	DrvEepromWriteShort(ADDR_EEPROM_GYR_CALIB_Z,calib[ 2U ]);
}


//retourne les config du magnetometer
void DrvEepromReadMag ( Int16S calib[ 3U ] )
{
	calib[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_MAG_CALIB_X);
	calib[ 1U ] = DrvEepromReadShort(ADDR_EEPROM_MAG_CALIB_Y);
	calib[ 2U ] = DrvEepromReadShort(ADDR_EEPROM_MAG_CALIB_Z);
}

//enregistre les config du magnetometer
void DrvEepromWriteMag ( Int16S calib[ 3U ] )
{
	DrvEepromWriteShort(ADDR_EEPROM_MAG_CALIB_X,calib[ 0U ]);
	DrvEepromWriteShort(ADDR_EEPROM_MAG_CALIB_Y,calib[ 1U ]);
	DrvEepromWriteShort(ADDR_EEPROM_MAG_CALIB_Z,calib[ 2U ]);
}


//retourne les config du pid
void DrvEepromReadPID(Int8U index,float *P, float *I, float *D)
{
	if( index == 0 )
	{
		*P = DrvEepromReadFloat(ADDR_EEPROM_PID_X_P);
		*I = DrvEepromReadFloat(ADDR_EEPROM_PID_X_I);
		*D = DrvEepromReadFloat(ADDR_EEPROM_PID_X_D);
	}
	else if( index == 1 )
	{
		*P = DrvEepromReadFloat(ADDR_EEPROM_PID_Y_P);
		*I = DrvEepromReadFloat(ADDR_EEPROM_PID_Y_I);
		*D = DrvEepromReadFloat(ADDR_EEPROM_PID_Y_D);
	}
	else if( index == 2 )
	{
		*P = DrvEepromReadFloat(ADDR_EEPROM_PID_Z_P);
		*I = DrvEepromReadFloat(ADDR_EEPROM_PID_Z_I);
		*D = DrvEepromReadFloat(ADDR_EEPROM_PID_Z_D);
	}
	else if( index == 3 )
	{
		*P = DrvEepromReadFloat(ADDR_EEPROM_PID_ALT_P);
		*I = DrvEepromReadFloat(ADDR_EEPROM_PID_ALT_I);
		*D = DrvEepromReadFloat(ADDR_EEPROM_PID_ALT_D);
	}
}

//enregistre les config du pid
void DrvEepromWritePID (Int8U index,float P, float I, float D)
{
	if( index == 0 )
	{
		DrvEepromWriteFloat(ADDR_EEPROM_PID_X_P, P);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_X_I, I);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_X_D, D);
	}
	else if( index == 1 )
	{
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Y_P, P);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Y_I, I);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Y_D, D);
	}
	else if( index == 2 )
	{
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Z_P, P);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Z_I, I);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_Z_D, D);
	}
	else if( index == 3 )
	{
		DrvEepromWriteFloat(ADDR_EEPROM_PID_ALT_P, P);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_ALT_I, I);
		DrvEepromWriteFloat(ADDR_EEPROM_PID_ALT_D, D);
	}
}


//retourne les config du altitude
void DrvEepromReadAltitude( Int16S *altitude )
{
	altitude[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_ALTITUDE);
}

//enregistre les config du altitude
void DrvEepromWriteAltitude ( Int16S altitude )
{
	DrvEepromWriteShort(ADDR_EEPROM_ALTITUDE, altitude);
}

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorFrontRight( Int16U *startup)
{
	startup[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_MOTOR_F_R);
}

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorFrontRight ( Int16U startup)
{
	DrvEepromWriteShort(ADDR_EEPROM_MOTOR_F_R, startup);
}

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorFrontLeft( Int16U *startup)
{
	startup[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_MOTOR_F_L);
}

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorFrontLeft ( Int16U startup)
{
	DrvEepromWriteShort(ADDR_EEPROM_MOTOR_F_L, startup);
}

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorRearRight( Int16U *startup)
{
	startup[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_MOTOR_R_R);
}

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorRearRight ( Int16U startup)
{
	DrvEepromWriteShort(ADDR_EEPROM_MOTOR_R_R, startup);
}

//retourne la valeure startup du moteur
void DrvEepromReadStartupMotorRearLeft( Int16U *startup)
{
	startup[ 0U ] = DrvEepromReadShort(ADDR_EEPROM_MOTOR_R_L);
}

//retourne la valure startup du moteur
void DrvEepromWriteStartupMotorRearLeft ( Int16U startup)
{
	DrvEepromWriteShort(ADDR_EEPROM_MOTOR_R_L, startup);
}
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
//Fonction de lecture eeprom
static Int8U DrvEepromReadByte ( const Int8U * addr )
{
	return eeprom_read_byte(addr);
}

//Fonction d ecriture eeprom
static void DrvEepromWriteByte ( Int8U * addr, Int8U byte)
{
	if(EEPROM_WRITE_ENABLE)
	{
		eeprom_write_byte(addr,byte);
	}
}

//Fonction de lecture eeprom
static Int16U DrvEepromReadShort( const Int8U * addr )
{
	Int16U ret = 0;
	ret = eeprom_read_byte(addr) ;
	ret = ret << 8U ;
	ret |= eeprom_read_byte((addr + 1));
	return ret;
}
	
//Fonction d ecriture eeprom
static void DrvEepromWriteShort ( Int8U *addr, Int16U byte)
{
	if(EEPROM_WRITE_ENABLE)
	{
		eeprom_write_byte((addr), (Int8U)(byte >> 8U) );
		eeprom_write_byte((addr + 1) ,(Int8U)(byte));
	}
}

//Fonction de lecture eeprom
static float DrvEepromReadFloat( const Int8U * addr )
{
	return eeprom_read_float((float*)addr);
}
	
//Fonction d ecriture eeprom
static void DrvEepromWriteFloat ( Int8U *addr, float value)
{
	if(EEPROM_WRITE_ENABLE)
	{
		eeprom_write_float((float*)(addr), value );
	}
}
