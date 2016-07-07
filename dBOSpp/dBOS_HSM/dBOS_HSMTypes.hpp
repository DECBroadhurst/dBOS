/**
 * @file dBOS_HSMTypes.hpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Definition of dBOS Hierarchical State Machine Data Types.
 *
 */

// -------------------------------------------------------------------------------------------------
#ifndef DBOS_HSMTYPES_H_
#define DBOS_HSMTYPES_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "DataTypes.h"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES
#define INVALIDEVENT 0U

// -------------------------------------------------------------------------------------------------
// FORWARD DECLERATIONS

// -------------------------------------------------------------------------------------------------
// DATA TYPES
typedef UNATIVECPU_t dBOS_HSM_EVENT_ID_t;

/**
 * \brief Return values from state event handlers.
 */
typedef enum {
	dBOS_HSM_EVENT_HANDLED, /**< Event has been handled by this state. */
	dBOS_HSM_ENTRY_CHECKS_PASSED, /**< Entry checks passed. */
	dBOS_HSM_EVENT_NOTHANDLED, /**< Event has not been handled by this state and should be passed to the parent state. */
	dBOS_HSM_EVENT_HANDLED_PASS_TO_PARENT, /**< Event HAS been handled by this state but should also be passed to the parent state. */
	dBOS_HSM_TRANSISTION, /**< Transition to a new state required. */
} dBOS_HSM_HANDLER_RETURN_t;

/**
 * \brief Debug Mode Setting type.
 */
typedef enum {
	HSM_DEBUG_NONE, HSM_DEBUG_TRANSISTIONS_ONLY, HSM_DEBUG_ALL_EVENTS
} dBOS_HSM_DEBUG_MODE_t;
// -------------------------------------------------------------------------------------------------
// CONSTANTS

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// CLASSES

}// namespace

#endif /* DBOS_HSMTYPES_H_ */
