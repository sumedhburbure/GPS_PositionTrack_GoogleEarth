#ifndef __APPLICATION_H
#define __APPLICATION_H
	#include "stm32f10x.h"
	#include "GPS_UART.h"
	#include "COMM_UART.h"
	#include "stdio.h"
	#include "string.h"
	#include "stdint.h"
	//GPRMC structure
	typedef struct{
			char positionLock;
			float UTC;
			char latitude_dir;
			char longitude_dir;
			float latitude;
			float longitude;
			float speed;
			uint32_t date;
	}GPS_GPRMC_struct;
	
	typedef struct{
		 uint8_t numberOfSatellites;
		 float altitude;
	}GPS_GPGGA_struct;
	//function Description
	void GPS_App_GetData(void);

#endif
