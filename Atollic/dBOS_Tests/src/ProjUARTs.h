// ProjUARTs.h
//
//  Created on: 4/10/2009
//      Author: David Broadhurst


#ifndef PROJUARTS_H_
#define PROJUARTS_H_

//-------------------------------------------------------------------------------------------------
//INCLUDE FILES
#include "DataTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------------------------------
//EXTERNS (if a matching code file exists)

//-------------------------------------------------------------------------------------------------
//DEFINES

//-------------------------------------------------------------------------------------------------
//DATA TYPES

//-------------------------------------------------------------------------------------------------
//GLOBAL VARIABLES

//-------------------------------------------------------------------------------------------------
//MACRO’S

//-------------------------------------------------------------------------------------------------
//FUNCTION PROTOTYPES
void ProjUARTs_InitCOMM2(void);
INT8U ProjUARTs_WriteCOMM2(INT8U *Data, UNATIVECPU_t NoBytes);
INT8U ProjUARTs_ReadCOMM2(INT8U *Data, UNATIVECPU_t NoBytes);
UNATIVECPU_t ProjUARTs_COMM2RxBytes(void);
UNATIVECPU_t ProjUARTs_DebugTxBytes(void);

void ProjUARTs_DebugUARTTxSingleByte(CHAR B);

void ProjUARTs_DebugUARTTxMutipleBytes(CHAR const * B, UNATIVECPU_t Bytes);

//-------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* PROJUARTS_H_ */
