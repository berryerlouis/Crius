/*
 * SrvComm.h
 *
 * Created: 31/05/2012 16:04:46
 *  Author: berryer
 */ 


#ifndef SRVCOMM_H_
#define SRVCOMM_H_

#include "Conf/conf_hard.h"
#include "Tools/tools_typedefs.h"

#include "Drv/DrvEvent.h"


////////////////////////////////////////////ANGLES MIN MAX////////////////////////////////////////////
#define		ANGLE_MAX			 90.0F
#define		ANGLE_MIN			-90.0F


////////////////////////////////////////PUBLIC FONCTIONS/////////////////////////////////////////
//init des moteurs
Boolean SrvCommInit (void)  ;
//dispatcher d'evenements
void SrvCommDispatcher (Event_t iEvent) ;

#endif /* SRVCOMM_H_ */