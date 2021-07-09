#include "application.h"
GPS_GPRMC_struct GPRMC_data;
GPS_GPGGA_struct GPGGA_data;
#include <stdlib.h>
char debug[100];
char saveBuffer[300];		//for debug

/**************************************************
**   Function to convert Co-ordinates in degrees
***************************************************/
float getCoOrdinatesInDegree(float value){
	int degree = 0;
	float minutes = 0;
	degree = (int)(value / 100);
	minutes = (value / 100) - degree;
	minutes = (minutes*100)/60;
	return (((float)degree)+minutes);
}


/**************************************************
**   Function to convert string to float.
**   (However , due to non accuracy of decimal points,
**    I am not using this function. )
***************************************************/
float custom_atof(const char *Payload){
	int before_decimal = 0;
	float after_decimal = 0;
	while(*Payload != '.'){
		before_decimal = (before_decimal * 10) + (*Payload - '0');
		Payload++;
	}
	Payload++;
	int hits = 0;
	while(*Payload != NULL){
		after_decimal = (after_decimal * 10) + (*Payload - '0');
		Payload++;
		hits ++;
	}
	while(hits --){
		after_decimal /= 10;
	}
	float final_value = before_decimal + after_decimal;
	return final_value;
}



/**************************************************
**   Function for Parsing the GPRMC frame and 
**   assigning the values to variables. 
***************************************************/
void GPRMC_Parser(char *Payload){
	char * token;
	token = strtok(Payload,",");
	token = strtok(NULL,",");
	GPRMC_data.UTC = atof(token);
	token = strtok(NULL,",");
	GPRMC_data.positionLock = *token;
	token = strtok(NULL,",");
	GPRMC_data.latitude = getCoOrdinatesInDegree(atof(token));
	token = strtok(NULL,",");
	GPRMC_data.latitude_dir = *token;
	token = strtok(NULL,",");
	GPRMC_data.longitude = getCoOrdinatesInDegree(atof(token));
	token = strtok(NULL,",");
	GPRMC_data.longitude_dir = *token;
	token = strtok(NULL,",");
	GPRMC_data.speed = atof(token);
	token = strtok(NULL,",");
	token = strtok(NULL,",");
	GPRMC_data.date = atoi(token);
	token = strtok(NULL,",");
	token = strtok(NULL,",");
	token = strtok(NULL,",");
}


/**************************************************
**   Function for Parsing $GPGGA frame and 
**   assigning the values to variables.
***************************************************/
void GPGGA_Parser(char *Payload){
	char *token;
	token = strtok(Payload,",");
	token = strtok(NULL,",");
	token = strtok(NULL,",");
	token = strtok(NULL,",");
	token = strtok(NULL,",");
	token = strtok(NULL,",");
	token = strtok(NULL,",");
	token = strtok(NULL,",");
	GPGGA_data.numberOfSatellites = atof(token);
	token = strtok(NULL,",");
	token = strtok(NULL,",");
	GPGGA_data.altitude = atof(token);
}


/**************************************************
**   Function checks for $GPRMC and $GPGGA frames
**   and trims the frame.
***************************************************/
void GPS_App_GetData(void){
	if(GPS_CheckForString("$GPRMC",1)){
		if(GPS_CopyFrame("$GPRMC",saveBuffer)){
			GPRMC_Parser(saveBuffer);
			GPS_UART_Flush();
	}
		DelayMs(1000);
	}
	if(GPS_CheckForString("$GPGGA",1)){
		if(GPS_CopyFrame("$GPGGA",saveBuffer)){
			GPGGA_Parser(saveBuffer);
			GPS_UART_Flush();
		}
	}
	if(GPRMC_data.positionLock == 'A'){
		sprintf(debug,"%f,%f,%f ",GPRMC_data.longitude,GPRMC_data.latitude,GPGGA_data.altitude);
		Comm_SendStringIn_tx_Buff(debug);								//Print the data in KML format for google earth
	}
	else{
		GPS_UART_Flush();			//Flash The UART RX Buffer if useful data is not found in frame
	}
			
}

