// ProjUARTs.c
//
//  Created on: 4/10/2009
//      Author: David Broadhurst
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//INCLUDE FILES
#include "ProjUARTs.h"
#include "STM32_USART.h"
#include "MemoryBarrier.h"
//-------------------------------------------------------------------------------------------------
//LOCAL DEFINES
#define UART2_TxFIFOSize 512
#define UART2_RxFIFOSize 32
//-------------------------------------------------------------------------------------------------
//LOCAL CONSTANTS

//-------------------------------------------------------------------------------------------------
//LOCAL DATA TYPES

//-------------------------------------------------------------------------------------------------
//LOCAL TABLES

//-------------------------------------------------------------------------------------------------
//LOCAL GLOBAL VARIABLES
static INT8U UART2TxFIFO[UART2_TxFIFOSize];
static INT8U UART2RxFIFO[UART2_RxFIFOSize];

STM32USART_DRV_t COMM2;
//-------------------------------------------------------------------------------------------------
//LOCAL FUNCTION PROTOTYPES
static void P_ProjUARTs_UART2Int_Enable(void);
static void P_ProjUARTs_UART2Int_Disable(void);

//-------------------------------------------------------------------------------------------------
//PUBLIC FUNCTIONS
void ProjUARTs_InitCOMM2(void){

	RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	COMM2.RxFIFO = UART2RxFIFO;
	COMM2.RxFIFOSize = UART2_RxFIFOSize;
	COMM2.TxFIFO = UART2TxFIFO;
	COMM2.TxFIFOSize = UART2_TxFIFOSize;

	COMM2.DISABLE_INT_Func = P_ProjUARTs_UART2Int_Disable;
	COMM2.ENABLE_INT_Func = P_ProjUARTs_UART2Int_Enable;

	STM32_USART_Init(USART2, &COMM2, 256000, 0, 36000000);

	NVIC_SetPriority(USART2_IRQn, 1);

	return;
}

INT8U ProjUARTs_WriteCOMM2(INT8U *Data, UNATIVECPU_t NoBytes){

	return STM32_USART_WriteBytes(&COMM2, Data, NoBytes);
}

INT8U ProjUARTs_ReadCOMM2(INT8U *Data, UNATIVECPU_t NoBytes){

	return STM32_USART_ReadBytes(&COMM2, Data, NoBytes);
}

UNATIVECPU_t ProjUARTs_COMM2RxBytes(void){

	return STM32_USART_RxBytes(&COMM2);
}

UNATIVECPU_t ProjUARTs_DebugTxBytes(void){

	return STM32_USART_TxFIFOLevel(&COMM2);
}

void ProjUARTs_DebugUARTTxSingleByte(CHAR B){

	while(STM32_USART_WriteBytes(&COMM2, (INT8U const *) &B, 1) == 0){

	}

	return;
}

void ProjUARTs_DebugUARTTxMutipleBytes(CHAR const * B, UNATIVECPU_t Bytes){

	while(STM32_USART_WriteBytes(&COMM2, (INT8U const *) B, Bytes) == 0){

	}

}
//-------------------------------------------------------------------------------------------------
//PRIVATE FUNCTIONS
void USART2_IRQHandler(void){

	STM32_USART_IrqHandler(&COMM2);

	return;
}

void P_ProjUARTs_UART2Int_Enable(){

	NVIC_EnableIRQ(USART2_IRQn);

	return;
}

void P_ProjUARTs_UART2Int_Disable(){

	NVIC_DisableIRQ(USART2_IRQn);

	return;
}

//-------------------------------------------------------------------------------------------------
