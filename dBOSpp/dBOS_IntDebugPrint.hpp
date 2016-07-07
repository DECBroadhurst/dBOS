/**
 * @file dBOS_IntDebugPrint.hpp
 * @author David Broadhurst
 * @date 11/04/2014
 *
 * \brief Definition of interface to character output services.
 *
 */

#ifndef DBOS_INTDEBUGPRINT_HPP_
#define DBOS_INTDEBUGPRINT_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_DataTypes.hpp"
#include <stdarg.h>

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// DATA TYPES

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
void DBOS_Print_IntReturnValue(DBOS_RESULT_t Result);

void DBOS_Intprintf(CHAR const * const S, ...);

void DBOS_DebugPrint_printf(CHAR const * const S, va_list vl);

}// namespace

#endif /* DBOS_INTDEBUGPRINT_HPP_ */
