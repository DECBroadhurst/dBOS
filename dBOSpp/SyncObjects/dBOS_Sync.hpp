/**
 * @file dBOS_Sync.hpp
 * @author David Broadhurst
 * @date 15/01/2015
 *
 * \brief Definition of synchronization object base classes.
 *
 */

#ifndef DBOS_SYNC_HPP_
#define DBOS_SYNC_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Base.hpp"
#include "dBOS_Task_Queue.hpp"

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

// -------------------------------------------------------------------------------------------------
// CLASSES
class dBOS_SYNCBASE : public dBOS_BASE {

public:

	dBOS_SYNCBASE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID);
	dBOS_SYNCBASE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, INTERRUPT_MASK_t const Mask);

	virtual ~dBOS_SYNCBASE();

	virtual SIGNALLED_DATA_t GetSignalData(void) const = 0;
	virtual DBOS_TASK_PRIORITY_t GetPriority(void) const = 0;
	virtual DBOS_TASK_P_t GetOwnerTask(void) const = 0;

	DBOS_UNIQUESYNCID_t GetUniqueSyncID(void) const;
	INTERRUPT_MASK_t GetISRMask(void) const;

	virtual DBOS_RESULT_t InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const = 0;
	virtual DBOS_RESULT_t FROMISR_InitialWaitCheck() const = 0;
	virtual DBOS_RESULT_t PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const = 0;
	virtual DBOS_RESULT_t GiveObject(DBOS_TASK_P_t const pRxTask) = 0;
	virtual DBOS_RESULT_t TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask) = 0;
	virtual DBOS_RESULT_t TakeObjectAfterWaitCheck(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask);
	virtual DBOS_RESULT_t FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData) = 0;
	virtual DBOS_RESULT_t SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask) = 0;
	virtual DBOS_RESULT_t FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode) = 0;

	virtual DBOS_RESULT_t RequeueTaskInObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink, DBOS_TASK_PRIORITY_t const NewPriority) = 0;
	virtual DBOS_RESULT_t InsertTaskIntoObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink) = 0;
	virtual BOOL TaskWouldBeFirstInObjectWaitList(dBOS_LISTNODE<dBOS_TASK> const * const pTaskWaitLink) const = 0;
	virtual DBOS_TASK_P_t FromFrontRemoveTaskFromObjectWaitList(void) = 0;
	virtual DBOS_RESULT_t RemoveTaskFromObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink) = 0;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails(void) const;
	virtual void PrintTypeAndID(void) const = 0;
	virtual void PrintWaitList(void) const = 0;
#endif

	virtual dBOS_TASKINTERFACE_CONST_P_t GetTaskAtWaitListPosition(UNATIVECPU_t const Position) const = 0;

protected:

	dBOS_SYNCBASE() = delete;
	dBOS_SYNCBASE(dBOS_SYNCBASE const &) = delete;
	dBOS_SYNCBASE& operator=(dBOS_SYNCBASE const &) = delete;

	BOOL CheckISRDisabled(void) const;

private:

	INTERRUPT_MASK_t const m_InterruptMask;

	DBOS_UNIQUESYNCID_t const m_SyncID;
};

class dBOS_SYNC : public dBOS_SYNCBASE {

public:

	dBOS_SYNC(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType);
	dBOS_SYNC(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask);

	virtual ~dBOS_SYNC();

	virtual SIGNALLED_DATA_t GetSignalData(void) const override = 0;
	virtual DBOS_TASK_PRIORITY_t GetPriority(void) const override;
	virtual DBOS_TASK_P_t GetOwnerTask(void) const override = 0;

	virtual DBOS_RESULT_t InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override = 0;
	virtual DBOS_RESULT_t FROMISR_InitialWaitCheck() const override = 0;
	virtual DBOS_RESULT_t PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override = 0;
	virtual DBOS_RESULT_t GiveObject(DBOS_TASK_P_t const pRxTask) override = 0;
	virtual DBOS_RESULT_t TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask) override = 0;
	virtual DBOS_RESULT_t FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData) override = 0;
	virtual DBOS_RESULT_t SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask) override = 0;
	virtual DBOS_RESULT_t FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode) override = 0;

	virtual DBOS_RESULT_t RequeueTaskInObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink, DBOS_TASK_PRIORITY_t const NewPriority) override;
	virtual DBOS_RESULT_t InsertTaskIntoObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink) override;
	virtual BOOL TaskWouldBeFirstInObjectWaitList(dBOS_LISTNODE<dBOS_TASK> const * const pTaskWaitLink) const override;
	virtual DBOS_TASK_P_t FromFrontRemoveTaskFromObjectWaitList(void) override;
	virtual DBOS_RESULT_t RemoveTaskFromObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink) override;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails(void) const override;
	virtual void PrintTypeAndID(void) const override = 0;
	virtual void PrintWaitList(void) const override;
#endif

	virtual dBOS_TASKINTERFACE_CONST_P_t GetTaskAtWaitListPosition(UNATIVECPU_t const Position) const override;

protected:

	dBOS_SYNC() = delete;
	dBOS_SYNC(dBOS_SYNC const &) = delete;
	dBOS_SYNC& operator=(dBOS_SYNC const &) = delete;

	virtual dBOS_TASKQUEUE_ORDER_t GetWaitListOrderType(void) const;

private:

	dBOS_TASKQUEUE * m_pWaitList;		// List of tasks waiting on this object.
	DBOS_TASK_PRIORITY_t m_Priority;	// Priority of Sync Object, should always be set equal to priority of highest priority task in LinkedWaitList

};

class dBOS_BASICSYNC : public dBOS_SYNCBASE {

public:

	dBOS_BASICSYNC(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID);
	dBOS_BASICSYNC(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, INTERRUPT_MASK_t const Mask);

	virtual ~dBOS_BASICSYNC();

	virtual SIGNALLED_DATA_t GetSignalData(void) const override = 0;
	virtual DBOS_TASK_PRIORITY_t GetPriority(void) const override;
	virtual DBOS_TASK_P_t GetOwnerTask(void) const override = 0;

	virtual DBOS_RESULT_t InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override = 0;
	virtual DBOS_RESULT_t FROMISR_InitialWaitCheck() const override = 0;
	virtual DBOS_RESULT_t PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override = 0;
	virtual DBOS_RESULT_t GiveObject(DBOS_TASK_P_t const pRxTask) override = 0;
	virtual DBOS_RESULT_t TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask) override = 0;
	virtual DBOS_RESULT_t FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData) override = 0;
	virtual DBOS_RESULT_t SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask) override = 0;
	virtual DBOS_RESULT_t FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode) override = 0;

	virtual DBOS_RESULT_t RequeueTaskInObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink, DBOS_TASK_PRIORITY_t const NewPriority) override;
	virtual DBOS_RESULT_t InsertTaskIntoObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink) override;
	virtual BOOL TaskWouldBeFirstInObjectWaitList(dBOS_LISTNODE<dBOS_TASK> const * const pTaskWaitLink) const override;
	virtual DBOS_TASK_P_t FromFrontRemoveTaskFromObjectWaitList(void) override;
	virtual DBOS_RESULT_t RemoveTaskFromObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink) override;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails(void) const override;
	virtual void PrintTypeAndID(void) const override = 0;
	virtual void PrintWaitList(void) const override;
#endif

	virtual dBOS_TASKINTERFACE_CONST_P_t GetTaskAtWaitListPosition(UNATIVECPU_t const Position) const override;

protected:

	dBOS_BASICSYNC() = delete;
	dBOS_BASICSYNC(dBOS_BASICSYNC const &) = delete;
	dBOS_SYNC& operator=(dBOS_BASICSYNC const &) = delete;

private:

	DBOS_TASK_P_t m_pTask;

};

} // namespace

#endif /* DBOS_SYNC_HPP_ */
