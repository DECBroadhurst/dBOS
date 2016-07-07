/**
 * @file dBOS_Kernel.hpp
 * @author David Broadhurst
 * @date 21/02/2015
 *
 * \brief Definition of dBOS Kernel.
 *
 */

#ifndef DBOS_KERNEL_HPP_
#define DBOS_KERNEL_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Core_DataTypes.hpp"
#include "dBOS_List.hpp"

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

// Singleton Class.
class dBOS_KERNEL {

public:

	static DBOS_RESULT_t Init(void); // To be called before OS starts.
	static dBOS_KERNEL * GetInstance(void);

	DBOS_RESULT_t Start(DBOS_ID_t const ID, DBOS_PNTEQUIV_t const TaskStackSize, DBOS_TASKENTRY_FUNCTION_PNT_t const pTaskFunction);

	DBOS_RESULT_t RegisterSyncObject(DBOS_SYNCBASE_P_t const pSync);
	DBOS_UNIQUESYNCID_t GetNextSyncObjectID(void) const;

	INTERRUPT_MASK_t GetHighestISRMask(INTERRUPT_MASK_t const BaseMask, DBOS_SYNCBASE_CONST_P_t const * const pSyncObjects, UNATIVECPU_t const NumOfObjects) const;

	DBOS_STKPTR_t const * SelectNextTaskFromISR(void); 				// CALLED FROM WITHIN PendSV HANDLER
	DBOS_STKPTR_t const * GetAddressOfNewStackPointer(void) const;	// CALLED FROM WITHIN SVC HANDLER
	void ProcessISRFlags(void);										// CALLED FROM WITHIN PendSV HANDLER

	void SetISRSignaledFlag(DBOS_UNIQUESYNCID_t const ObjectID);
	void SetISRBroadcastFlag(DBOS_UNIQUESYNCID_t const ObjectID);

	DBOS_RESULT_t CurrentTaskWait(DBOS_TIMECOUNT_t const TimeCounts);
	DBOS_RESULT_t YieldCurrentTask(void);
	DBOS_TASK_PRIORITY_t GetCurrentTaskCurrentPriority(void) const;
	DBOS_TASK_P_t GetCurrentTask(void) const;

	DBOS_RESULT_t CheckAndWaitCurrentTaskOnObject(DBOS_SYNCBASE_P_t const pSyncObject, SIGNALLED_DATA_t * const pSignalData, DBOS_TIMECOUNT_t const Time, INTERRUPT_MASK_t const ISRMask, INTERRUPT_MASK_t * const pPrevISRMask,
			DBOS_TASK_P_t const TaskToSwitchTo);
	DBOS_RESULT_t CheckAndWaitCurrentTaskOnMultipleObjects(DBOS_SYNCBASE_P_t const pSyncObjects[], UNATIVECPU_t NumOfObjects, DBOS_TIMECOUNT_t Time, SNATIVECPU_t * const pSignalledObjectIndex, SIGNALLED_DATA_t * const pSignalData,
			INTERRUPT_MASK_t ISRMask, INTERRUPT_MASK_t * const pPrevISRMask);

	DBOS_RESULT_t WaitOnCV(DBOS_SYNCBASE_P_t const pSyncObject, DBOS_SYNCBASE_P_t const pMutex, SIGNALLED_DATA_t * const pSignalDataOut, DBOS_TIMECOUNT_t const Time, INTERRUPT_MASK_t const ISRMask);

	DBOS_RESULT_t SignalAndWaitOnObject(DBOS_SYNCBASE_P_t const pSyncObject, SIGNALLED_DATA_t const SignalledDataIn, DBOS_SIGNAL_MODE_t const SignalMode, BOOL const SwitchToTaskAtHeadOfWaitList, SIGNALLED_DATA_t * const pSignalDataOut,
			DBOS_TIMECOUNT_t const Time, INTERRUPT_MASK_t const ISRMask, INTERRUPT_MASK_t * const pPrevISRMask);

	DBOS_RESULT_t CurrentTaskTakeObject(DBOS_SYNCBASE_P_t const pSyncObject, SIGNALLED_DATA_t * const pSignalData);
	DBOS_RESULT_t CurrentTaskSignalObject(DBOS_SYNCBASE_P_t const pSyncObject, SIGNALLED_DATA_t const SignalledData, DBOS_SIGNAL_MODE_t const SignalMode, BOOL const SwitchToTaskAtHeadOfWaitList, INTERRUPT_MASK_t const ISRMask,
			INTERRUPT_MASK_t * const pPrevISRMask);

	void AddNewTaskToReadyList(dBOS_LISTNODE<dBOS_TASK> * const pLink);
	void RequeueTaskInReadyList(dBOS_LISTNODE<dBOS_TASK> * const pLink, DBOS_TASK_PRIORITY_t const NewPriority);

	DBOS_TASK_CONST_P_t GetTaskAtReadyListPosition(UNATIVECPU_t const Position) const;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

	void PrintReadyTaskList(void) const;

#endif

protected:

	dBOS_KERNEL();
	~dBOS_KERNEL();
	dBOS_KERNEL(dBOS_KERNEL const &) = delete;
	dBOS_KERNEL& operator=(dBOS_KERNEL const &) = delete;

private:

	DBOS_RESULT_t WaitCurrentTaskOnObject(DBOS_SYNCBASE_P_t const pSyncObject, SIGNALLED_DATA_t * const pSignalData, DBOS_TIMECOUNT_t const Time, INTERRUPT_MASK_t const ISRMask, INTERRUPT_MASK_t * const pPrevISRMask,
			DBOS_TASK_P_t TaskToSwitchTo);

	DBOS_RESULT_t SignalObjectProcess(DBOS_SYNCBASE_P_t const pSyncObject, BOOL const SignalAllTasks, BOOL const SwitchToTaskAtHeadOfWaitList, DBOS_TASK_P_t * const pTaskToSwitchTo);

	void AddTaskToReadyListPriorityTail(DBOS_TASK_P_t const pTask, DBOS_SYNCBASE_CONST_P_t const pSyncObject);
	void AddTaskToReadyListPriorityTail(DBOS_TASK_P_t const pTask);
	void RunNextReadyTask();
	void RunTask(DBOS_TASK_P_t const pTask);

	static dBOS_KERNEL * m_pInstance;

	class dBOS_TASKQUEUE * const m_pReadyTaskList;
	DBOS_TASK_P_t m_pCurrentTask;
	DBOS_TASK_P_t m_pIdleTask;

	DBOS_SYNCBASE_P_t m_SyncObjectList[MAXNUMOFSYNCOBJECTS];
	DBOS_UNIQUESYNCID_t m_SyncObjectCount;

};

} // namespace

#endif /* DBOS_KERNEL_HPP_ */
