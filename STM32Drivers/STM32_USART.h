// STM32_USART.h
//
//  Created on: 4/10/2009
//      Author: David Broadhurst

#ifndef STM32_USART_H_
#define STM32_USART_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "DataTypes.h"
#include "Device.h"
#include "ProjConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// DATA TYPES
typedef struct {

	USART_TypeDef *STM32UART;

	INT8U *RxFIFO;					//Rx FIFO.
	UNATIVECPU_t RxReadPnt;
	UNATIVECPU_t RxWritePnt;
	UNATIVECPU_t RxFIFOLevel;
	UNATIVECPU_t RxFIFOSize;

	INT8U *TxFIFO;					//Tx FIFO.
	UNATIVECPU_t TxReadPnt;
	UNATIVECPU_t TxWritePnt;
	UNATIVECPU_t TxFIFOLevel;
	UNATIVECPU_t TxFIFOSize;

	INT16U BRR_Check;
	INT16U CR3_Check;

	INT8U Flags;

	void (*DISABLE_INT_Func)(void);
	void (*ENABLE_INT_Func)(void);

	unsigned char USARTEvent;

} STM32USART_DRV_t;

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// -----------------------------------------------

void STM32_USART_Init(USART_TypeDef *UART, STM32USART_DRV_t *UART_DrvH, INT32U BaudRate, INT8U HFlow, INT32U apbclock);
// UART 		- Pointer to STM32 structure locating hardware registers.
// UART_DrvH	- Pointer to USART driver structure.
// BaudRate	- Baud Rate to set hardware to.
// HFlow		- Flow Control On/Off
// apbclock	- Clock Frequency MHz.

// -----------------------------------------------
INT8U STM32_USART_CheckSetup(STM32USART_DRV_t *UART_DrvH);

// -----------------------------------------------
INT8U STM32_USART_ReadFlags(STM32USART_DRV_t *UART_DrvH, INT8U Clear);

// -----------------------------------------------
void STM32_USART_IrqHandler(STM32USART_DRV_t *UART_DrvH);
// Function must be called from within USART Hardware IRQ handler.

//-----------------------------------------------
INT8U STM32_USART_WriteBytes(STM32USART_DRV_t *UART_DrvH, INT8U const * Data, UNATIVECPU_t NoBytes);
// Writes bytes to Driver Tx FIFO and starts hardware transmitting.
// UART_DrvH	- Pointer to USART driver structure.
// Data		- Data to write.
// NoBytes		- Number of Bytes to Write to FIFO

// returns		1	if done.
//				0	if error, not enough space in FIFO - No data is written to the FIFO in this case.

// -----------------------------------------------
INT8U STM32_USART_ReadBytes(STM32USART_DRV_t *UART_DrvH, INT8U *Data, UNATIVECPU_t NoBytes);
// Read bytes from Driver Rx FIFO
// UART_DrvH	- Pointer to USART driver structure.
// Data		- Pointer to memory to write data to.
// NoBytes		- Number of bytes to read.

//	returns		1	if done
//				0	if NoBytes argument is greater than the number of bytes in the FIFO, no data is read in this case.

// -----------------------------------------------
UNATIVECPU_t STM32_USART_RxBytes(STM32USART_DRV_t *UART_DrvH);
// Returns number of bytes currently in Rx FIFO
// UART_DrvH	- Pointer to USART driver structure.

// -----------------------------------------------
UNATIVECPU_t STM32_USART_TxFIFOLevel(STM32USART_DRV_t *UART_DrvH);
// Returns number of bytes currently in Tx FIFO
// UART_DrvH	- Pointer to USART driver structure.

// -------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
