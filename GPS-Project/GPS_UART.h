#ifndef __GPS_UART_H
#define __GPS_UART_H
	#include "stm32f10x.h"
	#include "string.h"
	#include "BSP.h"
	#include "stdint.h"
	#define GPS_BUFFER_SIZE 300
	//variables declaration
	typedef struct{
		uint8_t buffer[GPS_BUFFER_SIZE];
		volatile int head;
		volatile int tail;
		int searchIndex;
	}GPS_uart;
	extern char string_buffer[200];
//function declarations
	void GPS_UART_Init(void);
  char GPS_ReadFrom_rx_Buff(void);
	void GPS_StoreIn_tx_Buff(char data);
	void GPS_UART_Flush(void);
	uint16_t Is_GPS_rx_Available(void);
  void GPS_SendStringIn_tx_Buff(char *data);
	void GPS_FeedbackLoop(void);
	
	uint8_t GPS_SendCommand(char *command,int delay,char *response);
	uint8_t GPS_CheckForString(char *data,int timeOut);
	void GPS_GetAfter(char *string,char *buffer,int size);
	uint8_t GetData_From_Web(void);
	void GPS_CopyUpto(char * buffer,char *trimafter,char *saveBuffer);
	uint8_t GPS_CopyFrame(char *startString,char *bufferToSave);
#endif
