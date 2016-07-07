/**
 * @file dBOS_Core_Types.hpp
 * @author David Broadhurst
 * @date 13/01/2015
 *
 * \brief dBOS core data types.
 *
 */

#ifndef DBOS_CORE_DATATYPES_HPP_
#define DBOS_CORE_DATATYPES_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_DataTypes.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// FORWARD DECLERATIONS

// -------------------------------------------------------------------------------------------------
// DATA TYPES
typedef class dBOS_TASK * DBOS_TASK_P_t;
typedef class dBOS_TASK const * DBOS_TASK_CONST_P_t;

typedef class dBOS_SYNCBASE * DBOS_SYNCBASE_P_t;
typedef class dBOS_SYNCBASE const * DBOS_SYNCBASE_CONST_P_t;

typedef class dBOS_FLAG * DBOS_FLAG_P_t;
typedef class dBOS_FLAG const * DBOS_FLAG_CONST_P_t;

typedef class dBOS_MUTEX * DBOS_MUTEX_P_t;
typedef class dBOS_MUTEX const * DBOS_MUTEX_CONST_P_t;

typedef class dBOS_REGISTER * DBOS_REGISTER_P_t;
typedef class dBOS_REGISTER const * DBOS_REGISTER_CONST_P_t;

typedef class dBOS_SEMAPHORE * DBOS_SEMAPHORE_P_t;
typedef class dBOS_SEMAPHORE const * DBOS_SEMAPHORE_CONST_P_t;

typedef class dBOS_SIGNAL * DBOS_SIGNAL_P_t;
typedef class dBOS_SIGNAL const * DBOS_SIGNAL_CONST_P_t;

typedef class dBOS_SYNCTIMER * DBOS_TIMER_P_t;
typedef class dBOS_SYNCTIMER const * DBOS_TIMER_CONST_P_t;

typedef class dBOS_QUEUE * DBOS_QUEUE_P_t;
typedef class dBOS_QUEUE const * DBOS_QUEUE_CONST_P_t;

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

}// namespace

#endif /* DBOS_CORE_DATATYPES_HPP_ */
