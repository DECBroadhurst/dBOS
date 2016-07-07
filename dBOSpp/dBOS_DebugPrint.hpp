/**
 * @file dBOS_DebugPrint.hpp
 * @author David Broadhurst
 * @date 14/03/2015
 *
 * \brief Definition of print output functionality.
 *
 */

#ifndef DBOS_DEBUGPRINT_HPP_
#define DBOS_DEBUGPRINT_HPP_

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
// FORWARD DECLERATIONS

// -------------------------------------------------------------------------------------------------
// DATA TYPES

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void DBOS_printf(const CHAR *S, ...);
void DBOS_Print_ReturnValue(DBOS_RESULT_t Result);

#endif

}// namespace

#endif /* DBOS_DEBUGPRINT_HPP_ */
