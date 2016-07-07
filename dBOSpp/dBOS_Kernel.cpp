/**
 * @file dBOS_Kernel.cpp
 * @author David Broadhurst
 * @date 21/02/2015
 *
 * \brief Implementation of dBOS Kernel.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Kernel.hpp"
#include "dBOS_TimerManager.hpp"
#include "dBOS_Task_Queue.hpp"
#include "dBOS_Task.hpp"
#include "dBOS_Sync.hpp"
#include "dBOS_Assert.hpp"
#include "dBOS_Port.hpp"
#include "dBOS_PortFunctions.hpp"
#include "dBOS_TaskInterface.hpp"
#include "dBOS_PortSyncBitmap.hpp"

#if DBOS_TESTSENABLED == 1U
#include "TestSys.hpp"
#endif

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
dBOS_KERNEL * dBOS_KERNEL::m_pInstance = NULLPTR;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
DBOS_RESULT_t dBOS_KERNEL::Init(void){

	DBOS_RESULT_t Result;

	m_pInstance = new dBOS_KERNEL();

	if(m_pInstance != NULLPTR){

		Result = dBOS_TIMERMANAGER::Init();

		if(Result == DBOS_DONE){
			DBOS_Port_Init();
		}
	}
	else{
		Result = DBOS_OUT_OF_MEMORY;
	}

	return Result;
}

dBOS_KERNEL * dBOS_KERNEL::GetInstance(void){

	ASSERT_FROM_OSCS(m_pInstance != NULLPTR)

	return m_pInstance;
}

DBOS_RESULT_t dBOS_KERNEL::Start(DBOS_ID_t const ID, DBOS_PNTEQUIV_t const TaskStackSize, DBOS_TASKENTRY_FUNCTION_PNT_t const pTaskFunction){

	DBOS_RESULT_t Result;

	dBOS_TASKINTERFACE_P_t pIdleTaskInterface;

	DBOS_STK_t const * const pStackSpace = new DBOS_STK_t[TaskStackSize];

	if(pStackSpace != NULLPTR){

		pIdleTaskInterface = new dBOS_TASKINTERFACE(m_pIdleTask);

		if(pIdleTaskInterface != NULLPTR){

			m_pIdleTask = new dBOS_TASK(ID, m_SyncObjectCount, IDLETASKPRIORITY, TaskStackSize, pStackSpace, pTaskFunction, NULLPTR, pIdleTaskInterface);

			if(m_pIdleTask != NULLPTR){

				Result = RegisterSyncObject(&m_pIdleTask->m_TaskTimer);

				if(Result == DBOS_DONE){
					m_pIdleTask->SetRunningStateFromStartup();

					m_pCurrentTask = m_pIdleTask;

					DBOS_Port_StartIdleTask(m_pIdleTask->GetStackPointerAddress(), pStackSpace, TaskStackSize, pTaskFunction, pIdleTaskInterface);
				}

			}
			else{
				Result = DBOS_OUT_OF_MEMORY;
			}
		}
		else{
			Result = DBOS_OUT_OF_MEMORY;
		}
	}
	else{
		Result = DBOS_OUT_OF_MEMORY;
	}

	return Result;
}

DBOS_RESULT_t dBOS_KERNEL::RegisterSyncObject(DBOS_SYNCBASE_P_t const pSync){

	DBOS_RESULT_t Result;

	if(m_SyncObjectCount < MAXNUMOFSYNCOBJECTS){

		ASSERT_FROM_OSCS(m_SyncObjectList[m_SyncObjectCount] == NULLPTR)

		m_SyncObjectList[m_SyncObjectCount] = pSync;

		m_SyncObjectCount++;

		Result = DBOS_DONE;
	}
	else{
		Result = DBOS_MAX_NUM_OF_SYNCOBJECTS_REACHED;
	}

	return Result;
}

DBOS_UNIQUESYNCID_t dBOS_KERNEL::GetNextSyncObjectID(void) const{

	return m_SyncObjectCount;
}

INTERRUPT_MASK_t dBOS_KERNEL::GetHighestISRMask(INTERRUPT_MASK_t const BaseMask, DBOS_SYNCBASE_CONST_P_t const * const pSyncObjects, UNATIVECPU_t const NumOfObjects) const{

	UNATIVECPU_t Index;

	INTERRUPT_MASK_t Result;

	Result = BaseMask;

	for(Index = 0U; Index < NumOfObjects ; Index++){
		if(DBOS_Port_MaskALowerOrEqualPriorityThanB(pSyncObjects[Index]->GetISRMask(), Result) == FALSE){
			// == FALSE -> If Mask A Priority GREATER THAN Mask B
			Result = pSyncObjects[Index]->GetISRMask();
		}
	}

	return Result;
}

DBOS_STKPTR_t const * dBOS_KERNEL::SelectNextTaskFromISR(void){ // CALLED FROM WITHIN ISR (e.g. PendSV Handler for Cortex M3)
	// ASYNCHRONOUS CONTEXT SWITCH TO NEXT READY TASK

	dBOS_LISTNODE<dBOS_TASK> * pLink;

	ASSERT_FROM_OSCS(m_pCurrentTask != NULLPTR)

	pLink = m_pReadyTaskList->GetNextTaskFromQueue();
	// pLink will be NULL when the idle task is the currently running task and no other tasks are ready to run.
	if(pLink != NULLPTR){

		DBOS_TASK_P_t const pTask = pLink->GetItem();

		ASSERT_FROM_OSCS(pTask != NULLPTR)

		if(pTask->GetPriority() >= m_pCurrentTask->GetPriority()){
			// If the next task in the ready list has a priority equal to or greater than current task then switch.

			m_pReadyTaskList->RemoveTaskFromQueue(pLink);

			AddTaskToReadyListPriorityTail(m_pCurrentTask); // going from running to ready to run state.

			m_pCurrentTask = pTask;

			m_pCurrentTask->SetRunningState(); // going from ready to run to running state.
		}
	}

	return m_pCurrentTask->GetStackPointerAddress(); // Return to ISR.
}

DBOS_STKPTR_t const * dBOS_KERNEL::GetAddressOfNewStackPointer(void) const{	// CALLED FROM WITHIN SVC HANDLER

	ASSERT_FROM_OSCS(m_pCurrentTask != NULLPTR)

	return m_pCurrentTask->GetStackPointerAddress();
}

void dBOS_KERNEL::ProcessISRFlags(void){ // CALLED FROM WITHIN PendSV HANDLER

	UNATIVECPU_t Word;
	UNATIVECPU_t BitNumber;
	UNATIVECPU_t FlagNumber;

	Word = 0U;

	while(Word < TOTAL_NUM_ISR_FLAG_WORDS){

		BitNumber = dBOS_PortSyncBitmap_CheckAndClearSignalledFlag(Word);

		while(BitNumber != CPU_BITS_PER_WORD){		// While flags still set.

			FlagNumber = BitNumber + (Word * CPU_BITS_PER_WORD);

			ASSERT_FROM_OSCS(FlagNumber < MAXNUMOFSYNCOBJECTS)

			DBOS_SYNCBASE_P_t const pSync = m_SyncObjectList[FlagNumber];

			ASSERT_FROM_OSCS(pSync != NULLPTR)

			DBOS_TASK_P_t const pTask = pSync->FromFrontRemoveTaskFromObjectWaitList();

			if(pTask != NULLPTR){
				// pTask would be NULL if there were no tasks waiting on the object when it was signalled.

				ASSERT_FROM_OSCS(pTask->GetState() == DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS)
				// Task has been removed from a waitlist so this must be true

				if(pTask->RemoveTaskFromWaitListsWithException(pSync) == DBOS_RUNNING_TASK_PRIORITY_LOWERED){
					// Possible return value of DBOS_TASK_RUNNING_TASK_PRIORITY_LOWERED meaning
					// Priority of pCurrentTask has lowered (due to priority inheritance)
					// Nothing needed here since the call to ProcessISRFlags will be followed by SelectNextTaskFromISR if preemptive switching enabled.
					// SelectNextTaskFromISR will use updated priority for decision making.
					COVERAGE_PRINT("Running Task Priority Lowered")
				}

				AddTaskToReadyListPriorityTail(pTask, pSync);
			}

			BitNumber = dBOS_PortSyncBitmap_CheckAndClearSignalledFlag(Word);
		}

		Word++;
	}

	Word = 0U;

	while(Word < TOTAL_NUM_ISR_FLAG_WORDS){

		BitNumber = dBOS_PortSyncBitmap_CheckAndClearBroadcastFlag(Word);

		while(BitNumber != CPU_BITS_PER_WORD){		// While flags still set.

			FlagNumber = BitNumber + (Word * CPU_BITS_PER_WORD);

			ASSERT_FROM_OSCS(FlagNumber < MAXNUMOFSYNCOBJECTS)

			DBOS_SYNCBASE_P_t const pSync = m_SyncObjectList[FlagNumber];

			ASSERT_FROM_OSCS(pSync != NULLPTR)

			DBOS_TASK_P_t pTask = pSync->FromFrontRemoveTaskFromObjectWaitList();

			while(pTask != NULLPTR){
				// pTask would be NULL if there were no tasks waiting on the object when it was signalled.

				ASSERT_FROM_OSCS(pTask->GetState() == DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS)
				// Task has been removed from a waitlist so this must be true

				if(pTask->RemoveTaskFromWaitListsWithException(pSync) == DBOS_RUNNING_TASK_PRIORITY_LOWERED){
					// Possible return value of DBOS_TASK_RUNNING_TASK_PRIORITY_LOWERED meaning
					// Priority of pCurrentTask has lowered (due to priority inheritance)
					// Nothing needed here since the call to ProcessISRFlags will be followed SelectNextTaskFromISR if preemptive switching enabled.
					// SelectNextTaskFromISR will use updated priority for decision making.
					COVERAGE_PRINT("Running Task Priority Lowered")
				}

				AddTaskToReadyListPriorityTail(pTask, pSync);

				pTask = pSync->FromFrontRemoveTaskFromObjectWaitList();
			}

			BitNumber = dBOS_PortSyncBitmap_CheckAndClearBroadcastFlag(Word);
		}

		Word++;
	}

	return;
}

void dBOS_KERNEL::SetISRSignaledFlag(DBOS_UNIQUESYNCID_t const ObjectID){
	// CALLED FROM WITHIN AN ISR

	dBOS_PortSyncBitmap_SetSignalledFlag(ObjectID);

	DBOS_Port_SetPendContextSwitch();

	return;
}

void dBOS_KERNEL::SetISRBroadcastFlag(DBOS_UNIQUESYNCID_t const ObjectID){
	// CALLED FROM WITHIN AN ISR

	dBOS_PortSyncBitmap_SetBroadcastFlag(ObjectID);

	DBOS_Port_SetPendContextSwitch();

	return;
}

DBOS_RESULT_t dBOS_KERNEL::CurrentTaskWait(DBOS_TIMECOUNT_t const TimeCounts){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t PrevISRMask;
	INTERRUPT_MASK_t TestISRMask;

	if(TimeCounts > 0){

		ASSERT_FROM_OSCS(m_pCurrentTask != NULLPTR)

		dBOS_LISTNODE<dBOS_TASK> WaitLink(m_pCurrentTask);

		DBOS_SYNCBASE_P_t pTimer = &m_pCurrentTask->m_TaskTimer;

		// Disable timer interrupt when accessing the timer.
		PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

		m_pCurrentTask->m_TaskTimer.SetTime(TimeCounts);
		m_pCurrentTask->m_TaskTimer.Run();

		TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
		ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
		// Check ISR Mask Was Still Raised

		m_pCurrentTask->SetWaitingOnMultipleObjectsWithTimeOut(&pTimer, &WaitLink, 1U);

		RunNextReadyTask();

		ASSERT_FROM_OSCS(m_pCurrentTask->GetSignalledObject() == &m_pCurrentTask->m_TaskTimer)

		// Disable timer interrupt when accessing the timer.
		PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

		m_pCurrentTask->m_TaskTimer.ResetAndPause();

		TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
		ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
		// Check ISR Mask Was Still Raised

		Result = DBOS_DONE;
	}
	else if(TimeCounts == 0){
		Result = DBOS_DONE;
	}
	else{
		Result = DBOS_ERROR_INVALID_ARGUMENT;
	}

	return Result;
}

DBOS_RESULT_t dBOS_KERNEL::YieldCurrentTask(void){

	DBOS_RESULT_t Result;

	dBOS_LISTNODE<dBOS_TASK> * pLink;

	ASSERT_FROM_OSCS(m_pCurrentTask != NULLPTR)

	pLink = m_pReadyTaskList->GetNextTaskFromQueue();

	if(pLink != NULLPTR){

		DBOS_TASK_P_t const pTask = pLink->GetItem();

		ASSERT_FROM_OSCS(pTask != NULLPTR)

		if(pTask->GetPriority() >= m_pCurrentTask->GetPriority()){
			// If the next task in the ready list has a priority equal to or greater than current task then switch.

			m_pReadyTaskList->RemoveTaskFromQueue(pLink);

			AddTaskToReadyListPriorityTail(m_pCurrentTask); // going from running to ready to run state.

			RunTask(pTask); // going from ready to run to running state.

			Result = DBOS_TASK_SWITCHED;
		}
		else{
			Result = DBOS_TASK_NOT_SWITCHED;
		}
	}
	else{
		// pLink is NULL when no tasks in ready list, occurs for example when calling DBOS_Yield() from idle task loop.
		Result = DBOS_TASK_NOT_SWITCHED;
	}

	return Result;
}

DBOS_TASK_PRIORITY_t dBOS_KERNEL::GetCurrentTaskCurrentPriority(void) const{

	DBOS_TASK_PRIORITY_t Result;

	ASSERT_FROM_OSCS(m_pCurrentTask != NULLPTR)

	Result = m_pCurrentTask->GetPriority();

	return Result;
}

DBOS_TASK_P_t dBOS_KERNEL::GetCurrentTask(void) const{

	ASSERT_FROM_OSCS(m_pCurrentTask != NULLPTR)

	return m_pCurrentTask;
}

DBOS_RESULT_t dBOS_KERNEL::CheckAndWaitCurrentTaskOnObject(DBOS_SYNCBASE_P_t const pSyncObject, SIGNALLED_DATA_t * const pSignalData, DBOS_TIMECOUNT_t const Time, INTERRUPT_MASK_t const ISRMask, INTERRUPT_MASK_t * const pPrevISRMask,
		DBOS_TASK_P_t const TaskToSwitchTo){

	// ISRs ALREADY MASKED ON ENTRY TO THIS FUNCTION.
	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(ISRMask) == TRUE)

	DBOS_RESULT_t Result;
	DBOS_RESULT_t TakeResult;

	ASSERT_FROM_OSCS(pSyncObject != NULLPTR)
	ASSERT_FROM_OSCS(m_pCurrentTask != NULLPTR)

	*pSignalData = SIGNALDATANONE;

	Result = pSyncObject->InitialWaitCheck(m_pCurrentTask);

	switch(Result){
	case DBOS_SYNC_OBJECT_NO_WAIT_REQD:

		TakeResult = pSyncObject->TakeObjectAfterWaitCheck(pSignalData, m_pCurrentTask);

		ASSERT_FROM_OSCS(TakeResult == DBOS_DONE)

		m_pCurrentTask->ChangedSignalledObjectOfRunningTask(pSyncObject);

		Result = DBOS_OBJECT_WAIT_NO_WAIT;

		break;
	case DBOS_SYNC_OBJECT_WAIT_REQUIRED:

		Result = WaitCurrentTaskOnObject(pSyncObject, pSignalData, Time, ISRMask, pPrevISRMask, TaskToSwitchTo);

		break;
	default:
		*pSignalData = SIGNALDATANONE;
		// Some other error.
		break;
	}

	return Result;
}

DBOS_RESULT_t dBOS_KERNEL::CheckAndWaitCurrentTaskOnMultipleObjects(DBOS_SYNCBASE_P_t const pSyncObjects[], UNATIVECPU_t const NumOfObjects, DBOS_TIMECOUNT_t const Time, SNATIVECPU_t * const pSignalledObjectIndex,
		SIGNALLED_DATA_t * const pSignalData, INTERRUPT_MASK_t const ISRMask, INTERRUPT_MASK_t * const pPrevISRMask){

	// ISRs ALREADY MASKED ON ENTRY TO THIS FUNCTION.
	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(ISRMask) == TRUE)

	DBOS_RESULT_t Result;
	DBOS_RESULT_t TakeResult;
	DBOS_RESULT_t PostWaitResult;

	INTERRUPT_MASK_t TestISRMask;

	UNATIVECPU_t Index;

	ASSERT_FROM_OSCS(NumOfObjects > 0U)
	ASSERT_FROM_OSCS(m_pCurrentTask != NULLPTR)

	*pSignalData = SIGNALDATANONE;

	Index = 0U;
	// Find first valid object handle.
	while((pSyncObjects[Index] == NULLPTR) && (Index < NumOfObjects)){
		Index++;
	}

	ASSERT_FROM_OSCS(Index != NumOfObjects)
	// Assert at least one valid handle.

	Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;

	while((Index < NumOfObjects) && (Result == DBOS_SYNC_OBJECT_WAIT_REQUIRED)){

		if(pSyncObjects[Index] != NULLPTR){
			Result = pSyncObjects[Index]->InitialWaitCheck(m_pCurrentTask);
		}
		else{
			Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
		}

		if(Result == DBOS_SYNC_OBJECT_WAIT_REQUIRED){
			Index++;
		}

	}

	if(Result == DBOS_SYNC_OBJECT_WAIT_REQUIRED){

		if(Time == 0){
			*pSignalledObjectIndex = -1;
			Result = DBOS_OBJECT_WAIT_TIMEOUT;
		}
		else{

			DBOS_SYNCBASE_CONST_P_t pSignalledObject = NULLPTR;

			dBOS_LISTNODE<dBOS_TASK> WaitLinks[NumOfObjects + 1U]; // run-time sized array, plus 1 for task timer.
			DBOS_SYNCBASE_P_t pWaitObjects[NumOfObjects + 1U]; // run-time sized array, plus 1 for task timer.

			for(Index = 0U; Index < NumOfObjects ; Index++){
				pWaitObjects[Index] = pSyncObjects[Index];
				WaitLinks[Index].SetItem(m_pCurrentTask);
			}

			if(Time != TIMEWAITFOREVER){
				pWaitObjects[NumOfObjects] = &m_pCurrentTask->m_TaskTimer;
				WaitLinks[NumOfObjects].SetItem(m_pCurrentTask);

				m_pCurrentTask->m_TaskTimer.SetTime(Time);
				m_pCurrentTask->m_TaskTimer.Run();
			}

			do{

				TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(*pPrevISRMask); // Exit ISR critical section
				ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
				// Check ISR Mask Was Still Raised

				if(Time != TIMEWAITFOREVER){
					m_pCurrentTask->SetWaitingOnMultipleObjectsWithTimeOut(pWaitObjects, WaitLinks, NumOfObjects + 1U);
					// TODO deal with return value
				}
				else{
					m_pCurrentTask->SetWaitingOnMultipleObjectsWithTimeOut(pWaitObjects, WaitLinks, NumOfObjects);
					// TODO deal with return value
				}

				RunNextReadyTask();

				pSignalledObject = m_pCurrentTask->GetSignalledObject();

				ASSERT_FROM_OSCS(pSignalledObject != NULLPTR)

				*pPrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

				PostWaitResult = pSignalledObject->PostWaitCheck(m_pCurrentTask);

#if DBOS_COVERAGEPRINTOUTPUT == 1U
				if(PostWaitResult == DBOS_SYNC_OBJECT_WAIT_REQUIRED){
					COVERAGE_PRINT("Loop: ")
					m_pCurrentTask->PrintID();
				}
#endif

#if DBOS_TESTSENABLED == 1U
				if(PostWaitResult == DBOS_SYNC_OBJECT_WAIT_REQUIRED){
					DBOS_Test_LogEvent('^');
				}
#endif
				// A higher priority thread may have taken the object
				// before this thread resumed, so re-check again using this loop.
			}while(PostWaitResult == DBOS_SYNC_OBJECT_WAIT_REQUIRED);

			if(Time != TIMEWAITFOREVER){
				m_pCurrentTask->m_TaskTimer.ResetAndPause();
			}

			if(pSignalledObject == &m_pCurrentTask->m_TaskTimer){
				// Timed Out
				ASSERT_FROM_OSCS(Time != TIMEWAITFOREVER)

				*pSignalledObjectIndex = -1;

				Result = DBOS_OBJECT_WAIT_WAITED_TIMEOUT;
			}
			else{

				ASSERT_FROM_OSCS(PostWaitResult == DBOS_SYNC_OBJECT_NO_WAIT_REQD)

				Index = 0U;
				while((pSignalledObject != pSyncObjects[Index]) && (Index < NumOfObjects)){
					Index++;
				}

				ASSERT_FROM_OSCS(Index < NumOfObjects)

				TakeResult = pSyncObjects[Index]->TakeObjectAfterWaitCheck(pSignalData, m_pCurrentTask);

				ASSERT_FROM_OSCS(TakeResult == DBOS_DONE)

				*pSignalledObjectIndex = static_cast<SNATIVECPU_t>(Index);

				Result = DBOS_OBJECT_WAIT_WAITED;
			}

		}

	}
	else if(Result == DBOS_SYNC_OBJECT_NO_WAIT_REQD){

		ASSERT_FROM_OSCS(pSyncObjects[Index] != NULLPTR)

		TakeResult = pSyncObjects[Index]->TakeObjectAfterWaitCheck(pSignalData, m_pCurrentTask);

		ASSERT_FROM_OSCS(TakeResult == DBOS_DONE)

		m_pCurrentTask->ChangedSignalledObjectOfRunningTask(pSyncObjects[Index]);

		*pSignalledObjectIndex = static_cast<SNATIVECPU_t>(Index);

		Result = DBOS_OBJECT_WAIT_NO_WAIT;
	}
	else{
		// Result is not DBOS_SYNC_OBJECT_NO_WAIT_REQUIRED or DBOS_SYNC_OBJECT_WAIT_REQUIRED
		// Some other condition, not possible to wait on at least one of the objects.
		*pSignalledObjectIndex = static_cast<SNATIVECPU_t>(Index);
	}

	return Result;
}

DBOS_RESULT_t dBOS_KERNEL::WaitOnCV(DBOS_SYNCBASE_P_t const pSyncObject, DBOS_SYNCBASE_P_t const pMutex, SIGNALLED_DATA_t * const pSignalDataOut, DBOS_TIMECOUNT_t const Time, INTERRUPT_MASK_t const ISRMask){

	DBOS_RESULT_t Result;
	DBOS_RESULT_t SignalMutexResult;

	INTERRUPT_MASK_t PrevISRMask;
	INTERRUPT_MASK_t TestISRMask;

	SIGNALLED_DATA_t SignalDataM;

	DBOS_TASK_P_t TaskToSwitchTo = NULLPTR;

	SignalMutexResult = pMutex->SignalObject(0U, SIGNAL_AUTO, m_pCurrentTask);

	switch(SignalMutexResult){
	case DBOS_OBJECT_SIGNALLED_SIGNALONETASK:

		SignalObjectProcess(pMutex, FALSE, TRUE, &TaskToSwitchTo);

		PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

		Result = CheckAndWaitCurrentTaskOnObject(pSyncObject, pSignalDataOut, Time, ISRMask, &PrevISRMask, TaskToSwitchTo);

		TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
		ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
		// Check ISR Mask Was Still Raised

		PrevISRMask = DBOS_Port_GetInterruptMaskingLevel();

		CheckAndWaitCurrentTaskOnObject(pMutex, &SignalDataM, TIMEWAITFOREVER, ISRMASK_NONE, &PrevISRMask, NULLPTR); // Acquire Mutex Again

		break;
	case DBOS_MUTEX_ERROR_HAS_NO_OWNER:
	case DBOS_MUTEX_ERROR_THIS_TASK_NOT_THE_OWNER:
	case DBOS_MUTEX_ERROR_IS_NOT_LAST_MUTEX_ACQUIRED:
		Result = SignalMutexResult;
		break;
	case DBOS_OBJECT_SIGNAL_NO_TASKS:
		Result = DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET;
		break;
	default:
		Result = DBOS_INTERNAL_ERROR;
		ASSERT_ERROR_FROM_OSCS()
		;
		break;
	}

	return Result;
}

DBOS_RESULT_t dBOS_KERNEL::SignalAndWaitOnObject(DBOS_SYNCBASE_P_t const pSyncObject, SIGNALLED_DATA_t const SignalledDataIn, DBOS_SIGNAL_MODE_t const SignalMode, BOOL const SwitchToTaskAtHeadOfWaitList,
		SIGNALLED_DATA_t * const pSignalDataOut, DBOS_TIMECOUNT_t const Time, INTERRUPT_MASK_t const ISRMask, INTERRUPT_MASK_t * const pPrevISRMask){

	// ISRs ALREADY MASKED ON ENTRY TO THIS FUNCTION.
	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(ISRMask) == TRUE)

	ASSERT_FROM_OSCS(pSyncObject != NULLPTR)

	DBOS_RESULT_t Result;
	DBOS_RESULT_t SignalObjectResult;
	DBOS_RESULT_t SignalObjectProcessResult;

	INTERRUPT_MASK_t TestISRMask;

	DBOS_TASK_P_t TaskToSwitchTo = NULLPTR;

	SignalObjectResult = pSyncObject->SignalObject(SignalledDataIn, SignalMode, m_pCurrentTask);
	// Signal object might lower priority of this running task if the object being signaled is a mutex.

	dBOS_LISTNODE<dBOS_TASK> WaitLink(m_pCurrentTask);

	switch(SignalObjectResult){
	case DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS:

		TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(*pPrevISRMask); // Exit ISR critical section
		ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
		// Check ISR Mask Was Still Raised

		SignalObjectProcessResult = SignalObjectProcess(pSyncObject, TRUE, SwitchToTaskAtHeadOfWaitList, &TaskToSwitchTo);

		if(SignalObjectProcessResult == DBOS_OBJECT_SIGNALLED_TASK_SWITCH_REQUIRED){

			DBOS_RESULT_t PostWaitResult;

			ASSERT_FROM_OSCS(TaskToSwitchTo != NULLPTR)
			ASSERT_FROM_OSCS(TaskToSwitchTo != m_pCurrentTask)

			*pPrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask); // Enter ISR critical section

			pSyncObject->GiveObject(TaskToSwitchTo); // TODO Assert task is waiting on the object it is being given.

			TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(*pPrevISRMask); // Exit ISR critical section
			ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
			// Check ISR Mask Was Still Raised

			m_pCurrentTask->ChangedSignalledObjectOfRunningTask(pSyncObject);

			AddTaskToReadyListPriorityTail(m_pCurrentTask); // going from running to ready to run state.

			RunTask(TaskToSwitchTo);

			DBOS_SYNCBASE_CONST_P_t const pSignalledObject = m_pCurrentTask->GetSignalledObject();

			ASSERT_FROM_OSCS(pSignalledObject != NULLPTR)
			ASSERT_FROM_OSCS(pSignalledObject == pSyncObject)

			*pPrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask); // Enter ISR critical section

			PostWaitResult = pSignalledObject->PostWaitCheck(m_pCurrentTask);

			if(PostWaitResult == DBOS_SYNC_OBJECT_WAIT_REQUIRED){

#if DBOS_TESTSENABLED == 1U
				DBOS_Test_LogEvent(')'); // Tested
#endif

				Result = WaitCurrentTaskOnObject(pSyncObject, pSignalDataOut, Time, ISRMask, pPrevISRMask, NULLPTR);
			}
			else{

				*pSignalDataOut = pSignalledObject->GetSignalData();

#if DBOS_TESTSENABLED == 1U
				DBOS_Test_LogEvent('{'); // Tested
#endif

				Result = DBOS_OBJECT_SIGNALLED_TASK_SWITCHED;
			}

		}
		else{

			DBOS_RESULT_t TakeResult;

			*pPrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask); // Enter ISR critical section

			TakeResult = pSyncObject->TakeObjectAfterWaitCheck(pSignalDataOut, m_pCurrentTask);

			ASSERT_FROM_OSCS(TakeResult == DBOS_DONE)

			m_pCurrentTask->ChangedSignalledObjectOfRunningTask(pSyncObject);

#if DBOS_TESTSENABLED == 1U
			DBOS_Test_LogEvent('}'); // Tested
#endif

			Result = SignalObjectProcessResult;
		}

		break;
	case DBOS_OBJECT_SIGNALLED_SIGNALONETASK:

		TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(*pPrevISRMask); // Exit ISR critical section
		ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
		// Check ISR Mask Was Still Raised

		if(pSyncObject->TaskWouldBeFirstInObjectWaitList(&WaitLink) == TRUE){

			DBOS_RESULT_t TakeResult;

			*pPrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask); // Enter ISR critical section

			TakeResult = pSyncObject->TakeObjectAfterWaitCheck(pSignalDataOut, m_pCurrentTask);

			ASSERT_FROM_OSCS(TakeResult == DBOS_DONE)

			m_pCurrentTask->ChangedSignalledObjectOfRunningTask(pSyncObject);

#if DBOS_TESTSENABLED == 1U
			DBOS_Test_LogEvent('['); // Tested
#endif

			Result = DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH;
		}
		else{

			if(SwitchToTaskAtHeadOfWaitList == TRUE){

				SignalObjectProcessResult = SignalObjectProcess(pSyncObject, FALSE, TRUE, &TaskToSwitchTo);

				ASSERT_FROM_OSCS(SignalObjectProcessResult == DBOS_OBJECT_SIGNALLED_TASK_SWITCH_REQUIRED)

				ASSERT_FROM_OSCS(TaskToSwitchTo != NULLPTR)
				ASSERT_FROM_OSCS(TaskToSwitchTo != m_pCurrentTask)

				*pPrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask); // Enter ISR critical section

				pSyncObject->GiveObject(TaskToSwitchTo); // TODO Assert task is waiting on the object it is being given.

				Result = WaitCurrentTaskOnObject(pSyncObject, pSignalDataOut, Time, ISRMask, pPrevISRMask, TaskToSwitchTo);

#if DBOS_TESTSENABLED == 1U
				DBOS_Test_LogEvent(']'); // Tested
#endif

				if((Result != DBOS_OBJECT_WAIT_WAITED_TIMEOUT) && (Result != DBOS_OBJECT_WAIT_WAITED)){
					// If the WaitCurrentTaskOnObject didn't actually result in a context switch.

#if DBOS_TESTSENABLED == 1U
					DBOS_Test_LogEvent('<'); // Tested
#endif

					TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(*pPrevISRMask); // Exit ISR critical section
					ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
					// Check ISR Mask Was Still Raised

					AddTaskToReadyListPriorityTail(m_pCurrentTask); // going from running to ready to run state.

					RunTask(TaskToSwitchTo);

					*pPrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask); // Enter ISR critical section

				}

			}
			else{

				DBOS_RESULT_t TakeResult;

				*pPrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask); // Enter ISR critical section

				TakeResult = pSyncObject->TakeObjectAfterWaitCheck(pSignalDataOut, m_pCurrentTask);

				ASSERT_FROM_OSCS(TakeResult == DBOS_DONE)

				m_pCurrentTask->ChangedSignalledObjectOfRunningTask(pSyncObject);

#if DBOS_TESTSENABLED == 1U
				DBOS_Test_LogEvent('@'); // Tested
#endif

				Result = DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH;

			}

		}

		break;
	case DBOS_OBJECT_SIGNAL_NO_TASKS:

#if DBOS_TESTSENABLED == 1U
		DBOS_Test_LogEvent('>'); // Tested
#endif
		Result = CheckAndWaitCurrentTaskOnObject(pSyncObject, pSignalDataOut, Time, ISRMask, pPrevISRMask, NULLPTR);

		break;
	default:

		Result = SignalObjectResult; // Some other Error from attempt to signal the object.

		break;
	}

	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(ISRMask) == TRUE)

	return Result;
}

DBOS_RESULT_t dBOS_KERNEL::CurrentTaskTakeObject(DBOS_SYNCBASE_P_t const pSyncObject, SIGNALLED_DATA_t * const pSignalData){

	// ISRs ALREADY MASKED ON ENTRY TO THIS FUNCTION.
	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(pSyncObject->GetISRMask()) == TRUE)

	DBOS_RESULT_t Result;

	Result = pSyncObject->InitialWaitCheck(m_pCurrentTask);

	switch(Result){
	case DBOS_SYNC_OBJECT_NO_WAIT_REQD:
		Result = pSyncObject->TakeObject(pSignalData, m_pCurrentTask);
		break;
	case DBOS_SYNC_OBJECT_WAIT_REQUIRED:
		Result = DBOS_OBJECT_NOT_AVAILABLE;
		break;
	default:
		// Some other error returned from InitialWaitCheck
		break;
	}

	return Result;
}

DBOS_RESULT_t dBOS_KERNEL::CurrentTaskSignalObject(DBOS_SYNCBASE_P_t const pSyncObject, SIGNALLED_DATA_t const SignalledData, DBOS_SIGNAL_MODE_t const SignalMode, BOOL const SwitchToTaskAtHeadOfWaitList, INTERRUPT_MASK_t const ISRMask,
		INTERRUPT_MASK_t * const pPrevISRMask){

	// ISRs ALREADY MASKED ON ENTRY TO THIS FUNCTION.
	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(ISRMask) == TRUE)

	DBOS_RESULT_t Result;

	DBOS_TASK_P_t TaskToSwitchTo;

	Result = pSyncObject->SignalObject(SignalledData, SignalMode, m_pCurrentTask);
	// Signal object might lower priority of this running task if the object being signaled in a mutex.

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(*pPrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
	// Check ISR Mask Was Still Raised

	switch(Result){
	case DBOS_OBJECT_SIGNALLED_SIGNALONETASK:
		Result = SignalObjectProcess(pSyncObject, FALSE, SwitchToTaskAtHeadOfWaitList, &TaskToSwitchTo);

		if(Result == DBOS_OBJECT_SIGNALLED_TASK_SWITCH_REQUIRED){

			ASSERT_FROM_OSCS(TaskToSwitchTo != NULLPTR)

			AddTaskToReadyListPriorityTail(m_pCurrentTask); // going from running to ready to run state.

			RunTask(TaskToSwitchTo);

			Result = DBOS_OBJECT_SIGNALLED_TASK_SWITCHED;
		}
		break;
	case DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS:
		Result = SignalObjectProcess(pSyncObject, TRUE, SwitchToTaskAtHeadOfWaitList, &TaskToSwitchTo);

		if(Result == DBOS_OBJECT_SIGNALLED_TASK_SWITCH_REQUIRED){

			ASSERT_FROM_OSCS(TaskToSwitchTo != NULLPTR)

			AddTaskToReadyListPriorityTail(m_pCurrentTask); // going from running to ready to run state.

			RunTask(TaskToSwitchTo);

			Result = DBOS_OBJECT_SIGNALLED_TASK_SWITCHED;
		}
		break;
	case DBOS_OBJECT_SIGNAL_NO_TASKS:
		Result = DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET;
		break;
	default:
		// Some other Error from attempt to signal the object.
		break;
	}

	*pPrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask); // Enter ISR critical section

	return Result;
}

void dBOS_KERNEL::AddNewTaskToReadyList(dBOS_LISTNODE<dBOS_TASK> * const pLink){

	DBOS_TASK_P_t const pTask = pLink->GetItem();

	ASSERT_FROM_OSCS(pTask != NULLPTR)

	pTask->SetReadyToRunStateFromStartup();

	m_pReadyTaskList->AddTaskToQueue(pLink);

	return;
}

void dBOS_KERNEL::RequeueTaskInReadyList(dBOS_LISTNODE<dBOS_TASK> * const pLink, DBOS_TASK_PRIORITY_t const NewPriority){

	m_pReadyTaskList->RequeueTaskInQueue(pLink, NewPriority);

	return;
}

DBOS_TASK_CONST_P_t dBOS_KERNEL::GetTaskAtReadyListPosition(UNATIVECPU_t const Position) const{

	return m_pReadyTaskList->GetTaskAtPosition(Position);
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_KERNEL::PrintReadyTaskList(void) const{

	DBOS_Intprintf("\nReady Task List: ");
	m_pReadyTaskList->PrintQueue();

	return;
}

#endif

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
dBOS_KERNEL::dBOS_KERNEL() :
		m_pReadyTaskList(new dBOS_TASKBITQUEUE()), m_pCurrentTask(NULLPTR), m_pIdleTask(NULLPTR), m_SyncObjectList { NULLPTR }, m_SyncObjectCount(0U){

	ASSERT_FROM_OSCS(m_pReadyTaskList != NULLPTR)
}

dBOS_KERNEL::~dBOS_KERNEL(){

}

DBOS_RESULT_t dBOS_KERNEL::WaitCurrentTaskOnObject(DBOS_SYNCBASE_P_t const pSyncObject, SIGNALLED_DATA_t * const pSignalData, DBOS_TIMECOUNT_t const Time, INTERRUPT_MASK_t const ISRMask, INTERRUPT_MASK_t * const pPrevISRMask,
		DBOS_TASK_P_t TaskToSwitchTo){

	// ISRs ALREADY MASKED ON ENTRY TO THIS FUNCTION.
	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(ISRMask) == TRUE)

	DBOS_RESULT_t Result;
	DBOS_RESULT_t PostWaitResult;
	DBOS_RESULT_t TakeResult;

	INTERRUPT_MASK_t TestISRMask;

	if(Time == 0){
		*pSignalData = SIGNALDATANONE;
		Result = DBOS_OBJECT_WAIT_TIMEOUT;
	}
	else{

		DBOS_SYNCBASE_CONST_P_t pSignalledObject = NULLPTR;

		dBOS_LISTNODE<dBOS_TASK> WaitLinks[2U];
		DBOS_SYNCBASE_P_t WaitObjects[2U];

		WaitObjects[0U] = pSyncObject;
		WaitLinks[0U].SetItem(m_pCurrentTask);

		if(Time != TIMEWAITFOREVER){
			WaitObjects[1U] = &m_pCurrentTask->m_TaskTimer;
			WaitLinks[1U].SetItem(m_pCurrentTask);

			m_pCurrentTask->m_TaskTimer.SetTime(Time);
			m_pCurrentTask->m_TaskTimer.Run();
		}

		do{

			TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(*pPrevISRMask); // Exit ISR critical section
			ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
			// Check ISR Mask Was Still Raised

			if(Time != TIMEWAITFOREVER){
				m_pCurrentTask->SetWaitingOnMultipleObjectsWithTimeOut(WaitObjects, WaitLinks, 2U);
				// TODO deal with return value.
			}
			else{
				m_pCurrentTask->SetWaitingOnMultipleObjectsWithTimeOut(WaitObjects, WaitLinks, 1U);
				// TODO deal with return value.
			}

			if(TaskToSwitchTo == NULLPTR){
				RunNextReadyTask();
			}
			else{
				RunTask(TaskToSwitchTo);
				TaskToSwitchTo = NULLPTR;
			}

			pSignalledObject = m_pCurrentTask->GetSignalledObject();

			ASSERT_FROM_OSCS(pSignalledObject != NULLPTR)

			*pPrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

			PostWaitResult = pSignalledObject->PostWaitCheck(m_pCurrentTask);

#if DBOS_COVERAGEPRINTOUTPUT == 1U
			if(PostWaitResult == DBOS_SYNC_OBJECT_WAIT_REQUIRED){
				COVERAGE_PRINT("Loop: ")
				m_pCurrentTask->PrintID();
			}
#endif

#if DBOS_TESTSENABLED == 1U
			if(PostWaitResult == DBOS_SYNC_OBJECT_WAIT_REQUIRED){
				DBOS_Test_LogEvent('(');
			}
#endif
			// A higher priority thread may have taken the object
			// before this thread resumed, so re-check again using this loop.
		}while(PostWaitResult == DBOS_SYNC_OBJECT_WAIT_REQUIRED);

		if(Time != TIMEWAITFOREVER){
			m_pCurrentTask->m_TaskTimer.ResetAndPause();
		}

		if(pSignalledObject == &m_pCurrentTask->m_TaskTimer){
			// Timed Out
			ASSERT_FROM_OSCS(Time != TIMEWAITFOREVER)

			*pSignalData = SIGNALDATANONE;
			Result = DBOS_OBJECT_WAIT_WAITED_TIMEOUT;
		}
		else{
			ASSERT_FROM_OSCS(pSignalledObject == pSyncObject)
			ASSERT_FROM_OSCS(PostWaitResult == DBOS_SYNC_OBJECT_NO_WAIT_REQD)

			TakeResult = pSyncObject->TakeObjectAfterWaitCheck(pSignalData, m_pCurrentTask);

			ASSERT_FROM_OSCS(TakeResult == DBOS_DONE)

			Result = DBOS_OBJECT_WAIT_WAITED;
		}

	}

	return Result;
}

DBOS_RESULT_t dBOS_KERNEL::SignalObjectProcess(DBOS_SYNCBASE_P_t const pSyncObject, BOOL const SignalAllTasks, BOOL const SwitchToTaskAtHeadOfWaitList, DBOS_TASK_P_t * const pTaskToSwitchTo){

	DBOS_RESULT_t Result;

	*pTaskToSwitchTo = NULLPTR;

	// Remove task from object wait list.
	DBOS_TASK_P_t const pFirstPriorityWaitingTask = pSyncObject->FromFrontRemoveTaskFromObjectWaitList();

	if(pFirstPriorityWaitingTask != NULLPTR){

		// Remove task from wait list of other objects it is waiting on.
		if(pFirstPriorityWaitingTask->RemoveTaskFromWaitListsWithException(pSyncObject) == DBOS_RUNNING_TASK_PRIORITY_LOWERED){
			// Priority of pCurrentTask has lowered (due to priority inheritance)
			// Updated priority is used for decisions below so nothing to do here.

			// How might execution end up here?
			// Task 1 acquires mutex 1.
			// Task 2 (a higher priority task) is set waiting on mutex 1 and on a signal. Task 1 inherits priority of task 2.
			// Task 1 signals the signal.
			// Task 2 is removed from wait list of signal and mutex -> priority of task 1 drops back down.

			COVERAGE_PRINT("Test Point 1")

#if DBOS_TESTSENABLED == 1U
			DBOS_Test_LogEvent('&');
#endif
		}

		AddTaskToReadyListPriorityTail(pFirstPriorityWaitingTask, pSyncObject);

		if(SignalAllTasks == TRUE){

			DBOS_TASK_P_t pWaitingTask;

			pWaitingTask = pSyncObject->FromFrontRemoveTaskFromObjectWaitList();

			while(pWaitingTask != NULLPTR){
				// Remove all remaining tasks from this object's wait list and add them to the ready list.

				if(pWaitingTask->RemoveTaskFromWaitListsWithException(pSyncObject) == DBOS_RUNNING_TASK_PRIORITY_LOWERED){
					// Priority of pCurrentTask has lowered (due to priority inheritance)
					// Updated priority is used for decisions below so nothing to do here.

					// How might execution end up here?
					// Task 1 acquires mutex 1.
					// Task 2 (a higher priority task) is set waiting on mutex 1 and on a signal. Task 1 inherits priority of task 2.
					// Task 3 (an EVEN higher priority task) is set waiting on the same signal.
					// Task 1 signals the signal.
					// ALL tasks waiting on the signal are made ready to run and removed from all object wait lists.
					// Task 2 is removed from wait list of signal AND mutex -> priority of task 1 drops back down.

					COVERAGE_PRINT("Test Point 2")

#if DBOS_TESTSENABLED == 1U
					DBOS_Test_LogEvent('*');
#endif
				}

				AddTaskToReadyListPriorityTail(pWaitingTask, pSyncObject);

				pWaitingTask = pSyncObject->FromFrontRemoveTaskFromObjectWaitList();
			}

		}

		if(pFirstPriorityWaitingTask->GetPriority() >= m_pCurrentTask->GetPriority()){
			if(SwitchToTaskAtHeadOfWaitList == TRUE){
				// This will run the first task that was in this object's wait list even if there were tasks of the same priority ALREADY in the ready list.

				// The first task that was in this object's wait list has already been added to the ready list above, remove it from the list and run it.
				m_pReadyTaskList->RemoveTaskFromQueue(pFirstPriorityWaitingTask->GetpReadyLink());

				*pTaskToSwitchTo = pFirstPriorityWaitingTask;

				ASSERT_FROM_OSCS(*pTaskToSwitchTo != NULLPTR)

				Result = DBOS_OBJECT_SIGNALLED_TASK_SWITCH_REQUIRED;
			}
			else{
				Result = DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH;
			}
		}
		else{
			Result = DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH;
		}

	}
	else{
		Result = DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING;
	}

	return Result;
}

void dBOS_KERNEL::AddTaskToReadyListPriorityTail(DBOS_TASK_P_t const pTask, DBOS_SYNCBASE_CONST_P_t const pSyncObject){

	pTask->SetReadyToRunState(pSyncObject);

	m_pReadyTaskList->AddTaskToQueue(pTask->GetpReadyLink());

	return;
}

void dBOS_KERNEL::AddTaskToReadyListPriorityTail(DBOS_TASK_P_t const pTask){

	pTask->SetReadyToRunState();

	m_pReadyTaskList->AddTaskToQueue(pTask->GetpReadyLink());

	return;
}

void dBOS_KERNEL::RunNextReadyTask(){
	// Implements immediate context switch to next highest priority task waiting to run.
	// Current task should already have been put into waiting state.

	dBOS_LISTNODE<dBOS_TASK> const * pLink;

	DBOS_TASK_P_t pNextTask;

	ASSERT_FROM_OSCS(m_pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(m_pCurrentTask->GetState() != DBOS_TASK_RUNNING)

	pLink = m_pReadyTaskList->DeQNextTaskFromQueue();

	ASSERT_FROM_OSCS(pLink != NULLPTR)

	pNextTask = pLink->GetItem();

	ASSERT_FROM_OSCS(pNextTask != NULLPTR)

	pNextTask->SetRunningState(); // going from ready to run to running state.

	m_pCurrentTask = pNextTask;

	DBOS_Port_SwCntxtSw();	// Immediate context switch

	return;
}

void dBOS_KERNEL::RunTask(DBOS_TASK_P_t const pTask){
	// Implements immediate context switch to pTask
	// Current task should already have been put into waiting state or into ready to run queue.

	ASSERT_FROM_OSCS(pTask != NULLPTR)

	ASSERT_FROM_OSCS(m_pCurrentTask != NULLPTR)

	pTask->SetRunningState();

	m_pCurrentTask = pTask;

	DBOS_Port_SwCntxtSw();	// Immediate context switch

	return;
}

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
