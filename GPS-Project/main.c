#include "main.h"

int main(void){
	systick_init();
	Comm_UART_Init();
	GPS_UART_Init();
	while(1){
		GPS_App_GetData();
		DelayMs(1000);
	}
} 

//		if(Is_Comm_rx_Available()){
//			char data = Comm_ReadFrom_rx_Buff();
//			Comm_StoreIn_tx_Buff(data);
//		}
//		if(Is_GPS_rx_Available()){
//			char data = GPS_ReadFrom_rx_Buff();
//			Comm_StoreIn_tx_Buff(data);
//		}
