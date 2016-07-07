// STM32_USART.c
//
//  Created on: 4/10/2009
//      Author: David Broadhurst
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES

#include "STM32_USART.h"
#include "UARTFlags.h"
#include "BitBand.h"
#include "MemoryBarrier.h"

// -------------------------------------------------------------------------------------------------
// LOCAL DEFINES

// -------------------------------------------------------------------------------------------------
// LOCAL CONSTANTS

// -------------------------------------------------------------------------------------------------
// LOCAL DATA TYPES

// -------------------------------------------------------------------------------------------------
// LOCAL TABLES

// -------------------------------------------------------------------------------------------------
// LOCAL GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS

void STM32_USART_Init(USART_TypeDef *UART, STM32USART_DRV_t *UART_DrvH, INT32U BaudRate, INT8U HFlow, INT32U apbclock){

	INT32U integerdivider;
	INT32U fractionaldivider;
	INT32U tmpreg;

	UART_DrvH->RxFIFOLevel = 0;
	UART_DrvH->RxReadPnt = 0;
	UART_DrvH->RxWritePnt = 0;

	UART_DrvH->TxFIFOLevel = 0;
	UART_DrvH->TxReadPnt = 0;
	UART_DrvH->TxWritePnt = 0;

	UART_DrvH->STM32UART = UART;

	if(HFlow){
		UART->CR3 = USART_CR3_CTSE | USART_CR3_RTSE;
		UART_DrvH->CR3_Check = USART_CR3_CTSE | USART_CR3_RTSE;
	}
	else{
		UART->CR3 = 0;
		UART_DrvH->CR3_Check = 0;
	}

	UART->CR1 = USART_CR1_UE;
	UART->CR1 |= USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
	UART->CR2 = 0;

	/* Determine the integer part */
	integerdivider = ((0x19 * apbclock) / (0x04 * (BaudRate)));
	tmpreg = (integerdivider / 0x64) << 0x04;
	/* Determine the fractional part */
	fractionaldivider = integerdivider - (0x64 * (tmpreg >> 0x04));
	tmpreg |= ((((fractionaldivider * 0x10) + 0x32) / 0x64)) & ((uint8_t) 0x0F);
	/* Write to USART BRR */
	UART->BRR = (uint16_t) tmpreg;

	UART_DrvH->BRR_Check = (uint16_t) tmpreg;

	UART->GTPR = 0;

	MEMORY_BARRIER

	(*(UART_DrvH->ENABLE_INT_Func))();

	return;
}

INT8U STM32_USART_CheckSetup(STM32USART_DRV_t *UART_DrvH){

	INT8U R;

	R = 1;

	if(UART_DrvH->STM32UART->BRR != UART_DrvH->BRR_Check){
		R = 0;
	}

	if((UART_DrvH->STM32UART->CR1 & ~(USART_CR1_TXEIE)) != (USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE)){
		R = 0;
	}

	if(UART_DrvH->STM32UART->CR2 != 0){
		R = 0;
	}

	if(UART_DrvH->STM32UART->CR3 != UART_DrvH->CR3_Check){
		R = 0;
	}

	if(UART_DrvH->STM32UART->GTPR != 0){
		R = 0;
	}

	return R;
}

INT8U STM32_USART_ReadFlags(STM32USART_DRV_t *UART_DrvH, INT8U Clear){

	INT8U F;

	(*(UART_DrvH->DISABLE_INT_Func))();

	MEMORY_BARRIER

	F = UART_DrvH->Flags;

	UART_DrvH->Flags &= (INT8U) ~(Clear);

	MEMORY_BARRIER

	(*(UART_DrvH->ENABLE_INT_Func))();

	return F;
}

void STM32_USART_IrqHandler(STM32USART_DRV_t *UART_DrvH){

	INT32U TxRpnt, RxWpnt;
	INT8U RxByte;

	INT32U Status;

	INT8U GI;

//	GI = ProjISR_DisableGlobal();	//this is to ensure sequential read of SR followed by DR.

	Status = UART_DrvH->STM32UART->SR;	//read of SR followed by

	if((Status & USART_SR_RXNE) || (Status & USART_SR_ORE)){
		RxByte = (INT8U) UART_DrvH->STM32UART->DR;			//read of DR will clear overrun flag.

//		ProjISR_SetGlobal(GI);

		if(Status & USART_SR_ORE){
			UART_DrvH->Flags |= USART_HARDWARE_RX_OVERFLOW;
		}

		if(Status & USART_SR_RXNE){

			if(UART_DrvH->RxFIFOLevel < UART_DrvH->RxFIFOSize){

				RxWpnt = UART_DrvH->RxWritePnt;

				UART_DrvH->RxFIFO[RxWpnt] = RxByte;

				RxWpnt++;
				if(RxWpnt >= UART_DrvH->RxFIFOSize){
					RxWpnt = 0;
				}

				UART_DrvH->RxWritePnt = RxWpnt;

				UART_DrvH->RxFIFOLevel++;
			}
			else{
				UART_DrvH->Flags |= USART_SOFTWARE_RX_OVERFLOW;
			}

		}

		UART_DrvH->USARTEvent = 1;

	}
	else{
//		ProjISR_SetGlobal(GI);
	}

	if(Status & USART_SR_TXE){	// Transmit Data Register Empty.

		if(UART_DrvH->TxFIFOLevel){

			UART_DrvH->TxFIFOLevel--;

			TxRpnt = UART_DrvH->TxReadPnt;

			UART_DrvH->STM32UART->DR = UART_DrvH->TxFIFO[TxRpnt];

			TxRpnt++;
			if(TxRpnt >= UART_DrvH->TxFIFOSize){
				TxRpnt = 0;
			}

			UART_DrvH->TxReadPnt = TxRpnt;

			if(UART_DrvH->TxFIFOLevel == 0U){
				Per_ResetBit_BB((INT32U ) &UART_DrvH->STM32UART->CR1, 7);	// Clear Interrupt Enable Bit.
			}

		}
		else{	// Buffer empty, finished transmission.
			Per_ResetBit_BB((INT32U ) &UART_DrvH->STM32UART->CR1, 7);	// Clear Interrupt Enable Bit.
		}
	}

	return;
}

INT8U STM32_USART_WriteBytes(STM32USART_DRV_t *UART_DrvH, INT8U const * Data, UNATIVECPU_t NoBytes){

	INT8U R;
	UNATIVECPU_t TxWpnt;
	UNATIVECPU_t FIFOSize;
	UNATIVECPU_t i;

	(*(UART_DrvH->DISABLE_INT_Func))();

	MEMORY_BARRIER

	if(UART_DrvH->TxFIFOSize - UART_DrvH->TxFIFOLevel >= NoBytes){
		R = 1;

		TxWpnt = UART_DrvH->TxWritePnt;
		FIFOSize = UART_DrvH->TxFIFOSize;

		for(i = 0; i < NoBytes ; i++){	//Copy Data into FIFO.
			UART_DrvH->TxFIFO[TxWpnt] = Data[i];
			TxWpnt++;
			if(TxWpnt >= FIFOSize){
				TxWpnt = 0;
			}
		}

		UART_DrvH->TxWritePnt = TxWpnt;
		UART_DrvH->TxFIFOLevel += NoBytes;

		if((UART_DrvH->STM32UART->SR & USART_SR_TXE)){	// || ((UART_DrvH->STM32UART->CR1 & USART_CR1_TXEIE)==0) ){	//If Data register empty OR interrupt not enabled.

			UART_DrvH->STM32UART->DR = UART_DrvH->TxFIFO[UART_DrvH->TxReadPnt];

			UART_DrvH->TxFIFOLevel--;

			UART_DrvH->TxReadPnt++;
			if(UART_DrvH->TxReadPnt >= UART_DrvH->TxFIFOSize){
				UART_DrvH->TxReadPnt = 0;
			}
		}

		Per_SetBit_BB((INT32U ) &UART_DrvH->STM32UART->CR1, 7);	//Set TXE Interrupt Enable Bit.
	}
	else{	// Number of Bytes will not fit in FIFO.
		R = 0;
	}

	MEMORY_BARRIER

	(*(UART_DrvH->ENABLE_INT_Func))();

	return R;
}

INT8U STM32_USART_ReadBytes(STM32USART_DRV_t *UART_DrvH, INT8U *Data, UNATIVECPU_t NoBytes){

	INT8U R;

	UNATIVECPU_t RxRpnt;
	UNATIVECPU_t FIFOSize;
	UNATIVECPU_t i;

	(*(UART_DrvH->DISABLE_INT_Func))();

	MEMORY_BARRIER

	if(NoBytes <= UART_DrvH->RxFIFOLevel){
		R = 1;

		RxRpnt = UART_DrvH->RxReadPnt;
		FIFOSize = UART_DrvH->RxFIFOSize;

		for(i = 0; i < NoBytes ; i++){
			Data[i] = UART_DrvH->RxFIFO[RxRpnt];

			RxRpnt++;
			if(RxRpnt >= FIFOSize){
				RxRpnt = 0;
			}

		}

		UART_DrvH->RxFIFOLevel -= NoBytes;

		UART_DrvH->RxReadPnt = RxRpnt;
	}
	else{
		R = 0;
	}

	MEMORY_BARRIER

	(*(UART_DrvH->ENABLE_INT_Func))();

	return R;
}

UNATIVECPU_t STM32_USART_RxBytes(STM32USART_DRV_t *UART_DrvH){

	UNATIVECPU_t Bytes;

	(*(UART_DrvH->DISABLE_INT_Func))();

	MEMORY_BARRIER

	Bytes = UART_DrvH->RxFIFOLevel;

	MEMORY_BARRIER

	(*(UART_DrvH->ENABLE_INT_Func))();

	return Bytes;
}

UNATIVECPU_t STM32_USART_TxFIFOLevel(STM32USART_DRV_t *UART_DrvH){

	UNATIVECPU_t Bytes;

	(*(UART_DrvH->DISABLE_INT_Func))();

	MEMORY_BARRIER

	Bytes = UART_DrvH->TxFIFOLevel;

	MEMORY_BARRIER

	(*(UART_DrvH->ENABLE_INT_Func))();

	return Bytes;
}

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------

