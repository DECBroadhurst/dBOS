/**
 * @file dBOS_QueueSemaphore.cpp
 * @author David Broadhurst
 * @date 8/05/2016
 *
 * \brief Implementation of semaphore synchronization object used for message queues.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_QueueSemaphore.hpp"
#include "dBOS_Task.hpp"
#include "dBOS_Assert.hpp"

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
dBOS_QUEUESEMAPHORE::dBOS_QUEUESEMAPHORE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, SNATIVECPU_t const InitialCount, SNATIVECPU_t const MaxCount, DBOS_WAITLIST_SORT_t const WaitListType) :
		dBOS_SEMAPHORE(ID, SyncID, InitialCount, MaxCount, WaitListType){

}

dBOS_QUEUESEMAPHORE::dBOS_QUEUESEMAPHORE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, SNATIVECPU_t const InitialCount, SNATIVECPU_t const MaxCount, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask) :
		dBOS_SEMAPHORE(ID, SyncID, InitialCount, MaxCount, WaitListType, Mask){

}

dBOS_QUEUESEMAPHORE::~dBOS_QUEUESEMAPHORE(){

}

DBOS_RESULT_t dBOS_QUEUESEMAPHORE::TakeObjectAfterWaitCheck(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask){

	(void) pSignalData;
	(void) pCurrentTask;

	return DBOS_DONE;
}
// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
