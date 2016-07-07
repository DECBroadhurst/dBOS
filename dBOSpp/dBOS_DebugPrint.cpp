/**
 * @file dBOS_DebugPrint.cpp
 * @author David Broadhurst
 * @date 14/03/2015
 *
 * \brief Implementation of print output functionality.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_DebugPrint.hpp"
#include "dBOS_IntDebugPrint.hpp"
#include "dBOS_Port.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// LOCAL DEFINES

// -------------------------------------------------------------------------------------------------
// LOCAL CONSTANTS

// -------------------------------------------------------------------------------------------------
// LOCAL DATA TYPES

// -------------------------------------------------------------------------------------------------
// LOCAL TABLES

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void DBOS_printf(const CHAR *S, ...){

	va_list args;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical();		// Enter OS Critical Section

	va_start(args, S);

	DBOS_DebugPrint_printf(S, args);

	DBOS_Port_ExitCritical(PrevCS);		// Exit OS Critical Section

	return;
}

void DBOS_Print_ReturnValue(DBOS_RESULT_t Result){

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical();		// Enter OS Critical Section

	DBOS_Print_IntReturnValue(Result);

	DBOS_Port_ExitCritical(PrevCS);		// Enter OS Critical Section

}

#endif

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
