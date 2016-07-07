/**
 * @file dBOS_Task.hpp
 * @author David Broadhurst
 * @date 14/01/2015
 *
 * \brief Definition of dBOS Task.
 *
 */
#ifndef DBOS_TASK_HPP_
#define DBOS_TASK_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Base.hpp"
#include "dBOS_List.hpp"
#include "dBOS_TaskTimer.hpp"

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

class dBOS_TASK : public dBOS_BASE {

public:

	dBOS_TASK(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const UniqueSyncID, DBOS_TASK_PRIORITY_t const Priority, DBOS_PNTEQUIV_t const TaskStackSize, DBOS_STK_t const * const pStackSpace, DBOS_TASKENTRY_FUNCTION_PNT_t const pTaskFunction,
			void const * const pTaskEntryData, dBOS_TASKINTERFACE_P_t const pInterface);
	virtual ~dBOS_TASK(void);

	dBOS_TASK() = delete;
	dBOS_TASK(dBOS_TASK const &) = delete;
	dBOS_TASK& operator=(dBOS_TASK const &) = delete;

	DBOS_TASK_PRIORITY_t GetPriority(void) const;
	DBOS_TASK_STATE_t GetState(void) const;
	DBOS_SYNCBASE_CONST_P_t GetSignalledObject(void) const;

	DBOS_STKPTR_t const * GetStackPointerAddress(void) const;

	DBOS_RESULT_t RaiseTaskPriority(DBOS_TASK_PRIORITY_t const NewPriorityIn);
	DBOS_RESULT_t LowerTaskPriority(DBOS_TASK_PRIORITY_t const NewPriorityIn);
	DBOS_TASK_PRIORITY_t CalcTaskPriorityFromHeldMutexObjects(void) const;

	DBOS_SYNCBASE_CONST_P_t RequeueTaskInWaitListsAndUpdatePriority(DBOS_TASK_PRIORITY_t const NewPriority);
	DBOS_RESULT_t RemoveTaskFromWaitListsWithException(DBOS_SYNCBASE_CONST_P_t const pSyncObject);

	void AddMutex(dBOS_LISTNODE<dBOS_MUTEX> * const pLink);
	DBOS_MUTEX_CONST_P_t GetLastAddedMutex(void) const;
	DBOS_RESULT_t RemoveMutexAndUpdatePriority();

	void ChangedSignalledObjectOfRunningTask(DBOS_SYNCBASE_CONST_P_t const pSyncObject);

	void SetStartUpState(void);
	void SetRunningState(DBOS_SYNCBASE_CONST_P_t const pSyncObject);
	void SetRunningState(void);
	void SetRunningStateFromStartup(void);
	void SetReadyToRunState(DBOS_SYNCBASE_CONST_P_t const pSyncObject);
	void SetReadyToRunState(void);
	void SetReadyToRunStateFromStartup(void);
	void SetWaitingOnMultipleObjectsWithTimeOut(DBOS_SYNCBASE_P_t * const pObjects, dBOS_LISTNODE<dBOS_TASK> * const pWaitLinks, UNATIVECPU_t const NumOfObjects);

	dBOS_LISTNODE<dBOS_TASK> * GetpReadyLink(void);

	dBOS_TASKTIMER m_TaskTimer;
	dBOS_TASKINTERFACE_P_t const m_pInterface;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	void PrintDetails(void) const;
	void PrintTypeAndID(void) const;
	void PrintFreeStackSpace(void) const;
	DBOS_SYNCBASE_CONST_P_t PrintState(void) const;
	DBOS_SYNCBASE_CONST_P_t PrintObjectsTaskWaitingOn(void) const;
#endif

protected:

private:

	DBOS_TASK_STATE_t m_State;
	dBOS_LINKEDLIST<dBOS_MUTEX> m_MutexList;
	DBOS_TASK_PRIORITY_t const m_PresetPriority;		// Priority set by application.
	DBOS_TASK_PRIORITY_t m_CurrentPriority;				// Current Priority of task.
	dBOS_LISTNODE<dBOS_TASK> * m_pWaitLinks;
	DBOS_SYNCBASE_P_t const * m_pWaitObjects;
	DBOS_SYNCBASE_CONST_P_t m_pSignalledObject;			// Object that was signalled allowing task to run.
	UNATIVECPU_t m_NumberOfObjectsWaitingOn;
	dBOS_LISTNODE<dBOS_TASK> m_ReadyLink;

	DBOS_STKPTR_t m_StackPointer;
	DBOS_PNTEQUIV_t const m_TaskStackSize;
	DBOS_STK_t const * const m_pStackSpace;

	static class dBOS_KERNEL * m_pK;

};

}			// namespace

#endif /* DBOS_TASK_HPP_ */
