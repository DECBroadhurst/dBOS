/**
 * @file dBOS_DataTypes.hpp
 * @author David Broadhurst
 * @date 13/01/2015
 *
 * \brief dBOS Data Types.
 *
 */

#ifndef DBOS_DATATYPES_HPP_
#define DBOS_DATATYPES_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_PortTypes.hpp"
#include "dBOS_Config.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// CONSTANTS
INT8U const TIMER_COUNTUP_FLAG = 0x01U;
INT8U const TIMER_COUNTDOWN_FLAG = 0x02U;
INT8U const TIMER_RELOAD_FLAG = 0x04U;
INT8U const TIMER_ALLOW_NEGATIVE = 0x08U;

// -------------------------------------------------------------------------------------------------
// FORWARD DECLERATIONS
class dBOS_TASKINTERFACE;

// -------------------------------------------------------------------------------------------------
// DATA TYPES
typedef dBOS_TASKINTERFACE * dBOS_TASKINTERFACE_P_t;
typedef dBOS_TASKINTERFACE const * dBOS_TASKINTERFACE_CONST_P_t;

#if (DBOS_USE_OBJECTNAMES == 1U)
typedef CHAR const * DBOS_ID_t;
#else
typedef UNATIVECPU_t DBOS_ID_t;
#endif

typedef UNATIVECPU_t DBOS_UNIQUESYNCID_t;

typedef void (* const DBOS_TASKENTRY_FUNCTION_PNT_t)(void *);

typedef enum {
	DBOS_DONE = 0U,
	DBOS_TASK_SWITCHED,
	DBOS_TASK_NOT_SWITCHED,
	DBOS_RUNNING_TASK_PRIORITY_LOWERED,
	DBOS_RUNNING_TASK_PRIORITY_RAISED,
	DBOS_TASK_READY_TASK_PRIORITY_LOWERED,
	DBOS_TASK_READY_TASK_PRIORITY_RAISED,
	DBOS_MUTEX_ERROR_NOT_A_RECURSIVE_MUTEX,
	DBOS_MUTEX_ERROR_THIS_TASK_NOT_THE_OWNER,
	DBOS_MUTEX_ERROR_HAS_NO_OWNER,
	DBOS_MUTEX_ERROR_IS_NOT_LAST_MUTEX_ACQUIRED,
	DBOS_OBJECT_NOT_AVAILABLE,
	DBOS_OBJECT_SIGNALLED_SIGNALONETASK,
	DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS,
	DBOS_OBJECT_SIGNAL_NO_TASKS,
	DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH,
	DBOS_OBJECT_SIGNALLED_TASK_SWITCHED,
	DBOS_OBJECT_SIGNALLED_TASK_SWITCH_REQUIRED,
	DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING,
	DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET,
	DBOS_OBJECT_WAIT_NO_WAIT,
	DBOS_OBJECT_WAIT_WAITED,
	DBOS_OBJECT_WAIT_WAITED_TIMEOUT,
	DBOS_OBJECT_WAIT_TIMEOUT, /**< Returned when specified wait time is zero and object is not signaled */
	DBOS_Q_WRITE_FAIL_FULL_TIMEOUT,
	DBOS_Q_WRITE_FAIL_FULL,
	DBOS_Q_WRITTEN,
	DBOS_Q_WRITTEN_WAITED,
	DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN,
	DBOS_Q_WRITE_MULTIPLE_NONE_WRITTEN,
	DBOS_Q_WRITE_MULTIPLE_SOME_WRITTEN,
	DBOS_Q_READ_FAIL_EMPTY_TIMEOUT,
	DBOS_Q_READ_FAIL_EMPTY,
	DBOS_Q_READ,
	DBOS_Q_READ_WAITED,
	DBOS_SYNC_OBJECT_NO_WAIT_REQD,
	DBOS_SYNC_OBJECT_WAIT_REQUIRED,
	DBOS_SYNC_OBJECT_INVALID_OBJECT_TYPE,
	DBOS_ERROR_INVALID_HANDLE,
	DBOS_ERROR_INVALID_ARGUMENT,
	DBOS_NOT_A_PRIORITY_BASED_WAITLIST,
	DBOS_OUT_OF_MEMORY,
	DBOS_MAX_NUM_OF_SYNCOBJECTS_REACHED,
	DBOS_INTERNAL_ERROR
} DBOS_RESULT_t;

typedef enum {
	PRIORITYLIST = 0U, FIRSTTOWAITFIRSTTORUNLIST
} DBOS_WAITLIST_SORT_t;

typedef enum {
	SIGNAL_AUTO = 0U, SIGNAL_ALL, SIGNAL_ONE
} DBOS_SIGNAL_MODE_t;

typedef enum {
	TIMERRUNNING = 0U, TIMERSTOPPED
} TIMERSTATE_t;

typedef enum {
	DBOS_TASK_STARTUP = 0U, DBOS_TASK_RUNNING, DBOS_TASK_READY_TO_RUN, DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS
} DBOS_TASK_STATE_t;

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

}// namespace

#endif /* DBOS_DATATYPES_HPP_ */
