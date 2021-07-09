#include "GPS_UART.h"
GPS_uart GPS_rx;
GPS_uart GPS_tx;


//temp
#include "COMM_UART.h"
#include "stdio.h"
char DEBUG1[100];
char string_buffer[200];
/*************************************
*	USART 2 For Communicating with ESP 
* PA2  -> TX
* PA3  -> RX
* Baud Rate -> 115200
**************************************/
void GPS_UART_Init(void){
	//----USART 2 Initialization----//
	RCC->APB1ENR |= ((0x01)<<17);  //Enable USART2 Clock
	GPIOA->CRL |= (0xA00) ;//PA2 pin TX as alternate function output pushpull 
	//PA3 already configured as input pushpull so no need to configure it FOR RX
	USART2->BRR = 0x138;//Baud rate setting to 115200
	USART2->CR1 |= ((0x01)<<2) | ((0x01)<<3) | ((0x01)<<13) | ((0x01)<<5); //TX RX AND USART ENABLE
	NVIC_EnableIRQ(USART2_IRQn);
}

/*******************************************
* GPS_UART_Flush function is used for 
* flushing the uart rx ring buffer
********************************************/
void GPS_UART_Flush(void){
	memset(GPS_rx.buffer,'\0',GPS_BUFFER_SIZE);
	GPS_rx.head = 0;
	GPS_rx.tail = 0;
}

/*******************************************
* GPS_StoreIn_rx_Buff function is used for 
* storing rx buffer from UART DATA Register
********************************************/
void GPS_StoreIn_rx_Buff(char data){
	int i = (GPS_rx.head + 1) % GPS_BUFFER_SIZE;
	if(i != GPS_rx.tail){
		GPS_rx.buffer[GPS_rx.head] = data;
		GPS_rx.head = i;	
	}
}

/*******************************************
* GPS_StoreIn_tx_Buff function is used for 
* storing tx ring buffer to send it to UART 
* DATA Register
********************************************/
void GPS_StoreIn_tx_Buff(char data){
	int i = (GPS_tx.head + 1) % GPS_BUFFER_SIZE;
	GPS_tx.buffer[GPS_tx.head] = data;
	GPS_tx.head = i;
	USART2->CR1 |= (USART_CR1_TXEIE); // Enable TX Data Register Empty Interrupt
}

/*******************************************
* GPS_ReadFrom_rx_Buff function is used for 
* reading rx ring buffer to send it to UART 
* DATA Register
********************************************/
char GPS_ReadFrom_rx_Buff(void){
	int i = (GPS_rx.tail + 1) % GPS_BUFFER_SIZE;
	char data = GPS_rx.buffer[GPS_rx.tail];
	GPS_rx.tail = i;
	return data;
}

/*******************************************
* GPS_SendStringIn_tx_Buff function is used 
* for storing string in tx buffer to send it
* to UART DATA Register
********************************************/
void GPS_SendStringIn_tx_Buff(char *data){
	while(* data){
		GPS_StoreIn_tx_Buff(*data++);
	}
}

/*******************************************
* Is_GPS_rx_Available function is used for 
* checking if anything is left to read.
********************************************/
uint16_t Is_GPS_rx_Available(void){
	return (uint16_t) ((GPS_BUFFER_SIZE -  (GPS_rx.head - GPS_rx.tail)) % GPS_BUFFER_SIZE);
}

/*******************************************
* GPS_FeedbackLoop function is used for 
* debugging purpose. It receives data in Rx
* buffer and sends it back to tx buffer
********************************************/
void GPS_FeedbackLoop(void){
	if(Is_GPS_rx_Available()){
		char data = GPS_ReadFrom_rx_Buff();
		GPS_StoreIn_tx_Buff(data);
	}
}

/*******************************************
* USART1_IRQHandler function is an interrupt
* handler for usart1.
********************************************/
void USART2_IRQHandler(void){
	if(USART2->SR & USART_SR_RXNE){
		if(GPS_rx.head < 450){
			char c = USART2->DR;
			GPS_StoreIn_rx_Buff(c);
		}
	}
	else if(USART2->SR & (USART_SR_TXE)){
		if(GPS_tx.head == GPS_tx.tail){
			USART2->CR1 &= ~(USART_CR1_TXEIE); //Disable TX Data Register Empty Interrupt
		}
		else{
			volatile int i = (GPS_tx.tail +1) % GPS_BUFFER_SIZE;
			USART2->DR = GPS_tx.buffer[GPS_tx.tail];
			GPS_tx.tail = i;
		}
	}
}


//------------------------------------String Functions-------------------------------------------------//


/*******************************************
* GPS_CheckForString function is used to 
* check whether the desired response is 
* received in GPS RX buffer or not
********************************************/
uint8_t GPS_CheckForString(char *data,int timeOut){
	DelayMs(timeOut);
	GPS_rx.searchIndex = 0;
	int strLength = strlen(data);
	int pos = 0;
	int match = 0;
	while(GPS_rx.searchIndex < GPS_BUFFER_SIZE){
			if(data[pos] != GPS_rx.buffer[GPS_rx.searchIndex]){
				GPS_rx.searchIndex++;
				match = 0;
				pos = 0;
			}
			else if(data[pos] == GPS_rx.buffer[GPS_rx.searchIndex]){
				pos++;
				match++;
				GPS_rx.searchIndex++;
				if(match == strLength){
					return 1;
				}
			}
	}
	return 0;
}

/*****************************************************
**	This functions copies individual GPS frames and 
**  trims them.
*****************************************************/
uint8_t GPS_CopyFrame(char *startString,char *bufferToSave){
	uint8_t sizeOfStartSting = strlen(startString);
	GPS_rx.searchIndex = 0;
	uint16_t pos = 0;
	uint8_t match = 0;
	uint8_t found = 0;
	uint16_t clipper_start = 0;
	uint16_t clipper_end = 0;
	while(GPS_rx.searchIndex < GPS_BUFFER_SIZE){
		if(startString[pos] != GPS_rx.buffer[GPS_rx.searchIndex]){
			pos = 0;
			match = 0;
			GPS_rx.searchIndex++;
		}
		else if(startString[pos] == GPS_rx.buffer[GPS_rx.searchIndex]){
			match++;
			pos++;
			GPS_rx.searchIndex++;
			if(match == sizeOfStartSting){
				found = 1;
				break;
			}
		}
	}
	if(found){
		clipper_start = GPS_rx.searchIndex - sizeOfStartSting;
	}
	else{
		GPS_UART_Flush();			//flush uart if our frame is not found, so that other frames fill the uart rx buffer
	}
	if(found){
			found = 0;
			while(GPS_rx.searchIndex < GPS_BUFFER_SIZE){
			if('*' == GPS_rx.buffer[GPS_rx.searchIndex++]){
				found = 1;
				break;
			}
		}	
	}
	if(found){
		clipper_end = (1 + GPS_rx.searchIndex);
		GPS_rx.searchIndex = clipper_start;
		pos = 0;
		while(GPS_rx.searchIndex < clipper_end){
			bufferToSave[pos++] = GPS_rx.buffer[GPS_rx.searchIndex++];
		}
		return 1;
	}
	else{
		GPS_UART_Flush();
		return 0;
	}	
}
