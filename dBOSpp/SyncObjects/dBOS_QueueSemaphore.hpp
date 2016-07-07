/**
 * @file dBOS_QueueSemaphore.hpp
 * @author David Broadhurst
 * @date 8/05/2016
 *
 * \brief Definition of semaphore synchronization object used for message queues.
 *
 */

// -------------------------------------------------------------------------------------------------
#ifndef DBOS_QUEUESEMAPHORE_H_
#define DBOS_QUEUESEMAPHORE_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Semaphore.hpp"

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
// CONSTANTS

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// CLASSES
class dBOS_QUEUESEMAPHORE : public dBOS_SEMAPHORE {

public:

	dBOS_QUEUESEMAPHORE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, SNATIVECPU_t const InitialCount, SNATIVECPU_t const MaxCount, DBOS_WAITLIST_SORT_t const WaitListType);
	dBOS_QUEUESEMAPHORE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, SNATIVECPU_t const InitialCount, SNATIVECPU_t const MaxCount, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask);

	virtual ~dBOS_QUEUESEMAPHORE();

	dBOS_QUEUESEMAPHORE() = delete;
	dBOS_QUEUESEMAPHORE(dBOS_QUEUESEMAPHORE const &) = delete;
	dBOS_QUEUESEMAPHORE& operator=(dBOS_QUEUESEMAPHORE const &) = delete;

	virtual DBOS_RESULT_t TakeObjectAfterWaitCheck(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask) override;

protected:

private:

};

} // namespace

#endif /* DBOS_QUEUESEMAPHORE_H_ */
