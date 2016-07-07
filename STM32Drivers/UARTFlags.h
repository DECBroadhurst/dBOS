/*
 * UARTFlags.h
 *
 *  Created on: 3/07/2010
 *      Author: David Broadhurst
 */

#ifndef UARTFLAGS_H_
#define UARTFLAGS_H_

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES

// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES
#define USART_HARDWARE_RX_OVERFLOW	0x01
#define USART_HARDWARE_TX_OVERFLOW	0x02
#define USART_SOFTWARE_RX_OVERFLOW	0x04
#define USART_SOFTWARE_TX_OVERFLOW	0x08

// -------------------------------------------------------------------------------------------------
// DATA TYPES

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

#ifdef __cplusplus
}
#endif

#endif
