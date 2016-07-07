/**
 * @file dBOS_KernelInterface.cpp
 * @author David Broadhurst
 * @date 5/03/2015
 *
 * \brief Implementation of dBOS Kernel Interface.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_KernelInterface.hpp"
#include "dBOS_Kernel.hpp"
#include "dBOS_Port.hpp"
#include "dBOS_PortFunctions.hpp"
#include "dBOS_Assert.hpp"
#include "dBOS_Sync.hpp"
#include "dBOS_TimerManager.hpp"
#include "dBOS_Task.hpp"

#include "dBOS_Flag.hpp"
#include "dBOS_Mutex.hpp"
#include "dBOS_Register.hpp"
#include "dBOS_Semaphore.hpp"
#include "dBOS_QueueSemaphore.hpp"
#include "dBOS_Signal.hpp"
#include "dBOS_SyncTimer.hpp"
#include "dBOS_Queue.hpp"
#include "dBOS_QueueInterface.hpp"

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
dBOS_KERNELINTERFACE * dBOS_KERNELINTERFACE::m_pInstance = NULLPTR;
dBOS_KERNEL * dBOS_KERNELINTERFACE::m_pImpl = NULLPTR;
dBOS_TIMERMANAGER const * dBOS_KERNELINTERFACE::m_pTimerManager = NULLPTR;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
DBOS_RESULT_t dBOS_KERNELINTERFACE::Init(void){

	DBOS_RESULT_t Result;

	DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_ALL); // Disable all interrupts at start up.

	m_pInstance = new dBOS_KERNELINTERFACE();

	if(m_pInstance != NULLPTR){

		Result = dBOS_KERNEL::Init();

		if(Result == DBOS_DONE){
			m_pImpl = dBOS_KERNEL::GetInstance();

			m_pTimerManager = dBOS_TIMERMANAGER::GetInstance();
		}
	}
	else{
		Result = DBOS_OUT_OF_MEMORY;
	}

	return Result;
}

dBOS_KERNELINTERFACE * dBOS_KERNELINTERFACE::GetInstance(void){

	ASSERT_FROM_OSCS(m_pInstance != NULLPTR)

	return m_pInstance;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Start(DBOS_ID_t const ID, DBOS_PNTEQUIV_t const TaskStackSize, DBOS_TASKENTRY_FUNCTION_PNT_t const pTaskFunction){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(ISRMASK_OSCRITICAL) == TRUE)

	Result = m_pImpl->Start(ID, TaskStackSize, pTaskFunction);

	ASSERT_ERROR_FROM_OSCS()

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::IdleTaskStarted(void){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(ISRMASK_NONE); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_ALL)) == TRUE)
	// Check ISR Mask Was Still Raised

	Result = Yield();

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Sleep(DBOS_TIMECOUNT_t const Time){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = m_pImpl->CurrentTaskWait(Time);

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Yield(){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = m_pImpl->YieldCurrentTask();

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return Result;
}

DBOS_TASK_PRIORITY_t dBOS_KERNELINTERFACE::GetCurrentTaskCurrentPriority() const{

	DBOS_TASK_PRIORITY_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = m_pImpl->GetCurrentTaskCurrentPriority();

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return Result;
}

DBOS_TIMECOUNT_t dBOS_KERNELINTERFACE::GetElapsedTime() const{

	DBOS_TIMECOUNT_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = m_pTimerManager->GetEllapsedTime();

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Object_Take(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t * const pSignalData){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t ISRMask;
	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	DBOS_SYNCBASE_P_t const pSync = pObject->GetSyncObject();

	ASSERT_FROM_OSCS(pSync != NULLPTR)

	ISRMask = pSync->GetISRMask();

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = m_pImpl->CurrentTaskTakeObject(pSync, pSignalData);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Object_TakeFROMISR(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t * const pSignalData){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t ISRMask;
	INTERRUPT_MASK_t PrevCSMask;
	INTERRUPT_MASK_t PrevISRMask;

	PrevCSMask = DBOS_Port_EnterCritical_FromISR();	// Enter OS critical section

	DBOS_SYNCBASE_P_t const pSync = pObject->GetSyncObject();

	ASSERT_FROM_OSCS(pSync != NULLPTR)

	ISRMask = pSync->GetISRMask();

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section, not strictly required if
																	// only one ISR accesses this object.

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = pSync->FROMISR_InitialWaitCheck();

	switch(Result){
	case DBOS_SYNC_OBJECT_NO_WAIT_REQD:
		Result = pSync->FROMISR_TakeObject(pSignalData);
		break;
	case DBOS_SYNC_OBJECT_WAIT_REQUIRED:
		Result = DBOS_OBJECT_NOT_AVAILABLE;
		break;
	case DBOS_SYNC_OBJECT_INVALID_OBJECT_TYPE:

		break;
	default:

		break;
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical_FromISR(PrevCSMask);	// Exit OS critical section

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Object_Wait(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t * const pSignalData, DBOS_TIMECOUNT_t const Time){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t ISRMask;
	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	DBOS_SYNCBASE_P_t const pSync = pObject->GetSyncObject();

	ASSERT_FROM_OSCS(pSync != NULLPTR)

	if(Time == TIMEWAITFOREVER){
		// Timer is not going to be used.
		ISRMask = pSync->GetISRMask();
	}
	else{
		ISRMask = m_pImpl->GetHighestISRMask(ISRMASK_OSTIMER, &pSync, 1U);
	}

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = m_pImpl->CheckAndWaitCurrentTaskOnObject(pSync, pSignalData, Time, ISRMask, &PrevISRMask, NULLPTR);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Object_WaitCV(dBOS_SYNCINTERFACE * const pObject, dBOS_MUTEXINTERFACE * const pMutex, SIGNALLED_DATA_t * const pSignalDataOut, DBOS_TIMECOUNT_t const Time){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t ISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	DBOS_SYNCBASE_P_t const pSync = pObject->GetSyncObject();
	DBOS_SYNCBASE_P_t const pSyncMutex = (static_cast<dBOS_SYNCINTERFACE_P_t>(pMutex))->GetSyncObject();

	ASSERT_FROM_OSCS(pSync != NULLPTR)
	ASSERT_FROM_OSCS(pSyncMutex != NULLPTR)

	if(Time == TIMEWAITFOREVER){
		// Timer is not going to be used.
		ISRMask = pSync->GetISRMask();
	}
	else{
		ISRMask = m_pImpl->GetHighestISRMask(ISRMASK_OSTIMER, &pSync, 1U);
	}

	Result = m_pImpl->WaitOnCV(pSync, pSyncMutex, pSignalDataOut, Time, ISRMask);

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Object_WaitMultiple(dBOS_SYNCINTERFACE * const pObjects[], UNATIVECPU_t const NumOfObjects, DBOS_TIMECOUNT_t const Time, SNATIVECPU_t * const pSignalledObjectIndex, SIGNALLED_DATA_t * const pSignalData){

	DBOS_RESULT_t Result;

	UNATIVECPU_t Index;

	INTERRUPT_MASK_t ISRMask;
	INTERRUPT_MASK_t PrevISRMask;

	DBOS_SYNCBASE_P_t pSync[NumOfObjects] = { NULLPTR };

	if(NumOfObjects > 0U){

		Result = DBOS_DONE;

		for(Index = 0U; Index < NumOfObjects ; Index++){
			if(pObjects[Index] == NULLPTR){
				Result = DBOS_ERROR_INVALID_HANDLE;
			}
			else{
				pSync[Index] = pObjects[Index]->GetSyncObject();
			}
		}

		if(Result == DBOS_DONE){

			INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

			if(Time == TIMEWAITFOREVER){
				// Timer is not going to be used.
				ISRMask = m_pImpl->GetHighestISRMask(ISRMASK_NONE, pSync, NumOfObjects);
			}
			else{
				ISRMask = m_pImpl->GetHighestISRMask(ISRMASK_OSTIMER, pSync, NumOfObjects);
			}

			PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

			ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
			// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

			Result = m_pImpl->CheckAndWaitCurrentTaskOnMultipleObjects(pSync, NumOfObjects, Time, pSignalledObjectIndex, pSignalData, ISRMask, &PrevISRMask);

			INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
			ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
			// Check ISR Mask Was Still Raised

			DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

		}

	}
	else{
		*pSignalledObjectIndex = -1;
		Result = DBOS_ERROR_INVALID_ARGUMENT;
	}

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Object_Signal(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t const SignalledData, DBOS_SIGNAL_MODE_t const SignalMode, BOOL const SwitchToTaskAtHeadOfWaitList){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t ISRMask;
	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	DBOS_SYNCBASE_P_t const pSync = pObject->GetSyncObject();

	ASSERT_FROM_OSCS(pSync != NULLPTR)

	ISRMask = pSync->GetISRMask();

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = m_pImpl->CurrentTaskSignalObject(pSync, SignalledData, SignalMode, SwitchToTaskAtHeadOfWaitList, ISRMask, &PrevISRMask);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Object_SignalFROMISR(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t const SignalledData, DBOS_SIGNAL_MODE_t const SignalMode){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t ISRMask;
	INTERRUPT_MASK_t PrevISRMask;
	INTERRUPT_MASK_t PrevCSMask;

	PrevCSMask = DBOS_Port_EnterCritical_FromISR();	// Enter OS critical section

	DBOS_SYNCBASE_P_t const pSync = pObject->GetSyncObject();

	ASSERT_FROM_OSCS(pSync != NULLPTR)

	ISRMask = pSync->GetISRMask();

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);		// Enter ISR critical section, not strictly required if
																		// only one ISR accesses this object.

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = pSync->FROMISR_SignalObject(SignalledData, SignalMode);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
	// Check ISR Mask Was Still Raised

	switch(Result){
	case DBOS_OBJECT_SIGNALLED_SIGNALONETASK:
		m_pImpl->SetISRSignaledFlag(pSync->GetUniqueSyncID());
		Result = DBOS_DONE;
		break;
	case DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS:
		m_pImpl->SetISRBroadcastFlag(pSync->GetUniqueSyncID());
		Result = DBOS_DONE;
		break;
	case DBOS_OBJECT_SIGNAL_NO_TASKS:
		Result = DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET;
		break;
	case DBOS_SYNC_OBJECT_INVALID_OBJECT_TYPE:

		break;
	default:
		ASSERT_ERROR_FROM_OSCS()
		break;
	}

	DBOS_Port_ExitCritical_FromISR(PrevCSMask);	// Exit OS critical section

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Object_SignalAndWait(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t const SignalledDataIn, DBOS_SIGNAL_MODE_t const SignalMode, BOOL const SwitchToTaskAtHeadOfWaitList,
		SIGNALLED_DATA_t * const pSignalDataOut, DBOS_TIMECOUNT_t const Time){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t ISRMask;
	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	DBOS_SYNCBASE_P_t const pSync = pObject->GetSyncObject();

	ASSERT_FROM_OSCS(pSync != NULLPTR)

	if(Time == TIMEWAITFOREVER){
		// Timer is not going to be used.
		ISRMask = pSync->GetISRMask();
	}
	else{
		ISRMask = m_pImpl->GetHighestISRMask(ISRMASK_OSTIMER, &pSync, 1U);
	}

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

	Result = m_pImpl->SignalAndWaitOnObject(pSync, SignalledDataIn, SignalMode, SwitchToTaskAtHeadOfWaitList, pSignalDataOut, Time, ISRMask, &PrevISRMask);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Queue_WriteToBack(dBOS_QUEUEINTERFACE_P_t const pQueue, void const * const pData, DBOS_TIMECOUNT_t const Time){

	DBOS_RESULT_t Result;

	Result = Queue_Write(pQueue, pData, Time, TRUE);

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Queue_WriteToFront(dBOS_QUEUEINTERFACE_P_t const pQueue, void const * const pData, DBOS_TIMECOUNT_t const Time){

	DBOS_RESULT_t Result;

	Result = Queue_Write(pQueue, pData, Time, FALSE);

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Queue_WriteMultipleToBack(dBOS_QUEUEINTERFACE_P_t const * const pQueue, UNATIVECPU_t const NumberOfQueues, void const * const pData, DBOS_TIMECOUNT_t const Time){

	DBOS_RESULT_t Result;

	Result = Queue_WriteMultiple(pQueue, NumberOfQueues, pData, Time, TRUE);

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Queue_WriteMultipleToFront(dBOS_QUEUEINTERFACE_P_t const * const pQueue, UNATIVECPU_t const NumberOfQueues, void const * const pData, DBOS_TIMECOUNT_t const Time){

	DBOS_RESULT_t Result;

	Result = Queue_WriteMultiple(pQueue, NumberOfQueues, pData, Time, FALSE);

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Queue_ReadFromFront(dBOS_QUEUEINTERFACE_P_t const pQueue, void * const pData, DBOS_TIMECOUNT_t const Time){

	DBOS_RESULT_t Result;

	Result = Queue_Read(pQueue, pData, Time, TRUE);

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Queue_ReadFromBack(dBOS_QUEUEINTERFACE_P_t const pQueue, void * const pData, DBOS_TIMECOUNT_t const Time){

	DBOS_RESULT_t Result;

	Result = Queue_Read(pQueue, pData, Time, FALSE);

	return Result;
}

dBOS_FLAGINTERFACE_P_t dBOS_KERNELINTERFACE::CreateFlag(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType){

	dBOS_FLAGINTERFACE_P_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = new dBOS_FLAGINTERFACE(new dBOS_FLAG(ID, m_pImpl->GetNextSyncObjectID(), WaitListType));

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_FLAGINTERFACE_P_t dBOS_KERNELINTERFACE::CreateFlag(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask){

	dBOS_FLAGINTERFACE_P_t Result;

	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(Mask); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = new dBOS_FLAGINTERFACE(new dBOS_FLAG(ID, m_pImpl->GetNextSyncObjectID(), WaitListType, Mask));

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(Mask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_MUTEXINTERFACE_P_t dBOS_KERNELINTERFACE::CreateNonRecursiveMutex(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType){

	dBOS_MUTEXINTERFACE_P_t Result;

	DBOS_MUTEX_P_t pMutex;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	pMutex = new dBOS_NONRECURSIVEMUTEX(ID, m_pImpl->GetNextSyncObjectID(), WaitListType);

	Result = new dBOS_MUTEXINTERFACE(pMutex);

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_MUTEXINTERFACE_P_t dBOS_KERNELINTERFACE::CreateRecursiveMutex(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType){

	dBOS_MUTEXINTERFACE_P_t Result;

	DBOS_MUTEX_P_t pMutex;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	pMutex = new dBOS_RECURSIVEMUTEX(ID, m_pImpl->GetNextSyncObjectID(), WaitListType);

	Result = new dBOS_MUTEXINTERFACE(pMutex);

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_REGISTERINTERFACE_P_t dBOS_KERNELINTERFACE::CreateRegister(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType){

	dBOS_REGISTERINTERFACE_P_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = new dBOS_REGISTERINTERFACE(new dBOS_REGISTER(ID, m_pImpl->GetNextSyncObjectID(), WaitListType));

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_REGISTERINTERFACE_P_t dBOS_KERNELINTERFACE::CreateRegister(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask){

	dBOS_REGISTERINTERFACE_P_t Result;

	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(Mask); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = new dBOS_REGISTERINTERFACE(new dBOS_REGISTER(ID, m_pImpl->GetNextSyncObjectID(), WaitListType, Mask));

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(Mask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_SEMAPHOREINTERFACE_P_t dBOS_KERNELINTERFACE::CreateSemaphore(DBOS_ID_t const ID, SNATIVECPU_t const InitialCount, SNATIVECPU_t const MaxCount, DBOS_WAITLIST_SORT_t const WaitListType){

	dBOS_SEMAPHOREINTERFACE_P_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = new dBOS_SEMAPHOREINTERFACE(new dBOS_SEMAPHORE(ID, m_pImpl->GetNextSyncObjectID(), InitialCount, MaxCount, WaitListType));

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_SEMAPHOREINTERFACE_P_t dBOS_KERNELINTERFACE::CreateSemaphore(DBOS_ID_t const ID, SNATIVECPU_t const InitialCount, SNATIVECPU_t const MaxCount, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask){

	dBOS_SEMAPHOREINTERFACE_P_t Result;

	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(Mask); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = new dBOS_SEMAPHOREINTERFACE(new dBOS_SEMAPHORE(ID, m_pImpl->GetNextSyncObjectID(), InitialCount, MaxCount, WaitListType, Mask));

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(Mask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_SIGNALINTERFACE_P_t dBOS_KERNELINTERFACE::CreateSignal(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType){

	dBOS_SIGNALINTERFACE_P_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = new dBOS_SIGNALINTERFACE(new dBOS_SIGNAL(ID, m_pImpl->GetNextSyncObjectID(), WaitListType));

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_SIGNALINTERFACE_P_t dBOS_KERNELINTERFACE::CreateSignal(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask){

	dBOS_SIGNALINTERFACE_P_t Result;

	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(Mask); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = new dBOS_SIGNALINTERFACE(new dBOS_SIGNAL(ID, m_pImpl->GetNextSyncObjectID(), WaitListType, Mask));

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(Mask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_QUEUEINTERFACE_P_t dBOS_KERNELINTERFACE::CreateQueue(DBOS_ID_t const ID, UNATIVECPU_t const size, UNATIVECPU_t const ItemSize){

	dBOS_QUEUEINTERFACE_P_t Result;

	Result = new dBOS_QUEUEINTERFACE(ID, size, ItemSize);

	return Result;
}

dBOS_QUEUEINTERFACE_P_t dBOS_KERNELINTERFACE::CreateQueue(DBOS_ID_t const ID, UNATIVECPU_t const size, UNATIVECPU_t const ItemSize, INTERRUPT_MASK_t const Mask){

	dBOS_QUEUEINTERFACE_P_t Result;

	Result = new dBOS_QUEUEINTERFACE(ID, size, ItemSize, Mask);

	return Result;
}

dBOS_TIMERINTERFACE_P_t dBOS_KERNELINTERFACE::CreateTimer(DBOS_ID_t const ID, INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime, DBOS_WAITLIST_SORT_t const WaitListType){

	dBOS_TIMERINTERFACE_P_t Result;

	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = new dBOS_TIMERINTERFACE(new dBOS_SYNCTIMER(ID, m_pImpl->GetNextSyncObjectID(), Flags, ReloadTime, InitialTime, WaitListType));

	if(Result != NULLPTR){
		m_pImpl->RegisterSyncObject((static_cast<dBOS_SYNCINTERFACE_P_t>(Result))->GetSyncObject());
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_TASKINTERFACE_P_t dBOS_KERNELINTERFACE::CreateTask(DBOS_ID_t const ID, DBOS_TASK_PRIORITY_t const Priority, DBOS_PNTEQUIV_t const TaskStackSize, DBOS_TASKENTRY_FUNCTION_PNT_t const pTaskFunction, void const * const pTaskEntryData){

	dBOS_TASKINTERFACE_P_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = new dBOS_TASKINTERFACE(ID, m_pImpl->GetNextSyncObjectID(), Priority, TaskStackSize, pTaskFunction, pTaskEntryData);

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return Result;
}

dBOS_TASKINTERFACE_CONST_P_t dBOS_KERNELINTERFACE::GetTaskAtReadyListPosition(UNATIVECPU_t Position) const{

	dBOS_TASKINTERFACE_CONST_P_t pResult;

	dBOS_TASK const * pTask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	pTask = m_pImpl->GetTaskAtReadyListPosition(Position);

	if(pTask == NULLPTR){
		pResult = NULLPTR;
	}
	else{
		pResult = pTask->m_pInterface;
	}

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return pResult;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_KERNELINTERFACE::PrintReadyTaskList(void) const{

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	m_pImpl->PrintReadyTaskList();

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

	return;
}

#endif

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
dBOS_KERNELINTERFACE::dBOS_KERNELINTERFACE(){

}

dBOS_KERNELINTERFACE::~dBOS_KERNELINTERFACE(){

}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Queue_WriteMultiple(dBOS_QUEUEINTERFACE_P_t const * const pQueue, UNATIVECPU_t const NumberOfQueues, void const * const pData, DBOS_TIMECOUNT_t const Time, BOOL const WriteToBack){

	DBOS_RESULT_t Result;

	UNATIVECPU_t Index;

	INTERRUPT_MASK_t ISRMask;
	INTERRUPT_MASK_t PrevISRMask;

	DBOS_SYNCBASE_P_t pSync[NumberOfQueues] = { NULLPTR };

	if(NumberOfQueues > 0U){

		Result = DBOS_DONE;

		for(Index = 0U; Index < NumberOfQueues ; Index++){
			if(pQueue[Index] == NULLPTR){
				Result = DBOS_ERROR_INVALID_HANDLE;
			}
			else{
				pSync[Index] = pQueue[Index]->m_pSemaphoreEmptyCount;
			}
		}

		if(Result == DBOS_DONE){

			INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

			if(Time == TIMEWAITFOREVER){
				// Timer is not going to be used.
				ISRMask = m_pImpl->GetHighestISRMask(ISRMASK_NONE, pSync, NumberOfQueues);
			}
			else{
				ISRMask = m_pImpl->GetHighestISRMask(ISRMASK_OSTIMER, pSync, NumberOfQueues);
			}

			PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

			ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
			// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

			UNATIVECPU_t QueuesWaitingOn;
			SIGNALLED_DATA_t SignalData;
			SNATIVECPU_t SignalledIndex;
			DBOS_RESULT_t TakeResult;
			DBOS_RESULT_t SignalResult;
			DBOS_RESULT_t QueueWriteResult;

			QueuesWaitingOn = NumberOfQueues;

			do{
				Result = m_pImpl->CheckAndWaitCurrentTaskOnMultipleObjects(pSync, NumberOfQueues, Time, &SignalledIndex, &SignalData, ISRMask, &PrevISRMask);

				switch(Result){
				case DBOS_OBJECT_WAIT_NO_WAIT:
				case DBOS_OBJECT_WAIT_WAITED:

					ASSERT_FROM_OSCS(SignalledIndex >= 0)
					ASSERT_FROM_OSCS(SignalledIndex < (SNATIVECPU_t )NumberOfQueues)

					TakeResult = pSync[SignalledIndex]->TakeObject(&SignalData, m_pImpl->GetCurrentTask()); // Queue Semaphores Are Not Taken Automatically After Waiting
					ASSERT_FROM_OSCS(TakeResult == DBOS_DONE)

					if(WriteToBack == TRUE){
						QueueWriteResult = pQueue[SignalledIndex]->m_pImpl->WriteToBack(pData);
					}
					else{
						QueueWriteResult = pQueue[SignalledIndex]->m_pImpl->WriteToFront(pData);
					}

					ASSERT_FROM_OSCS(QueueWriteResult == DBOS_Q_WRITTEN)

					SignalResult = m_pImpl->CurrentTaskSignalObject(pQueue[SignalledIndex]->m_pSemaphoreFullCount, 0U, SIGNAL_AUTO, FALSE, ISRMask, &PrevISRMask);

					ASSERT_FROM_OSCS((SignalResult == DBOS_OBJECT_SIGNALLED_TASK_SWITCHED) || (SignalResult == DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH) || (SignalResult == DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING))

					pSync[SignalledIndex] = NULLPTR;

					ASSERT_FROM_OSCS(QueuesWaitingOn > 0U)

					QueuesWaitingOn--;

					break;
				case DBOS_OBJECT_WAIT_WAITED_TIMEOUT:
				case DBOS_OBJECT_WAIT_TIMEOUT:

					break;
				default:

					break;
				}

			}while(((Result == DBOS_OBJECT_WAIT_NO_WAIT) || (Result == DBOS_OBJECT_WAIT_WAITED)) && (QueuesWaitingOn > 0U));

			INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
			ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
			// Check ISR Mask Was Still Raised

			DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

			switch(Result){
			case DBOS_OBJECT_WAIT_NO_WAIT:
			case DBOS_OBJECT_WAIT_WAITED:

				ASSERT_FROM_OSCS(QueuesWaitingOn == 0U)

				Result = DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN;
				break;
			case DBOS_OBJECT_WAIT_WAITED_TIMEOUT:
			case DBOS_OBJECT_WAIT_TIMEOUT:

				ASSERT_FROM_OSCS(QueuesWaitingOn > 0U)

				if(QueuesWaitingOn == NumberOfQueues){
					Result = DBOS_Q_WRITE_MULTIPLE_NONE_WRITTEN;
				}
				else{
					Result = DBOS_Q_WRITE_MULTIPLE_SOME_WRITTEN;
				}

				break;
			default:
				break;
			}

		}

	}
	else{
		Result = DBOS_ERROR_INVALID_ARGUMENT;
	}

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Queue_Write(dBOS_QUEUEINTERFACE_P_t const pQueue, void const * const pData, DBOS_TIMECOUNT_t const Time, BOOL const WriteToBack){

	DBOS_RESULT_t Result;
	DBOS_RESULT_t WaitResult;
	DBOS_RESULT_t TakeResult;

	DBOS_RESULT_t QueueWriteResult;

	INTERRUPT_MASK_t ISRMask;
	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	DBOS_SYNCBASE_P_t const pEmptySync = pQueue->m_pSemaphoreEmptyCount;
	DBOS_SYNCBASE_P_t const pFullSync = pQueue->m_pSemaphoreFullCount;

	ASSERT_FROM_OSCS(pEmptySync != NULLPTR)
	ASSERT_FROM_OSCS(pFullSync != NULLPTR)

	ASSERT_FROM_OSCS(pEmptySync->GetISRMask() == pFullSync->GetISRMask())

	if(Time == TIMEWAITFOREVER){
		// Timer is not going to be used.
		ISRMask = pEmptySync->GetISRMask();
	}
	else{
		ISRMask = m_pImpl->GetHighestISRMask(ISRMASK_OSTIMER, &pEmptySync, 1U);
	}

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	SIGNALLED_DATA_t SignalData = SIGNALDATANONE;

	WaitResult = m_pImpl->CheckAndWaitCurrentTaskOnObject(pEmptySync, &SignalData, Time, ISRMask, &PrevISRMask, NULLPTR);

	switch(WaitResult){
	case DBOS_OBJECT_WAIT_NO_WAIT:
	case DBOS_OBJECT_WAIT_WAITED: // deliberate fall through.

		TakeResult = pEmptySync->TakeObject(&SignalData, m_pImpl->GetCurrentTask()); // Queue Semaphores Are Not Taken Automatically After Waiting
		ASSERT_FROM_OSCS(TakeResult == DBOS_DONE)

		if(WriteToBack == TRUE){
			QueueWriteResult = pQueue->m_pImpl->WriteToBack(pData);
		}
		else{
			QueueWriteResult = pQueue->m_pImpl->WriteToFront(pData);
		}

		ASSERT_FROM_OSCS(QueueWriteResult == DBOS_Q_WRITTEN)

		Result = m_pImpl->CurrentTaskSignalObject(pFullSync, 0U, SIGNAL_AUTO, FALSE, ISRMask, &PrevISRMask);

		ASSERT_FROM_OSCS((Result == DBOS_OBJECT_SIGNALLED_TASK_SWITCHED) || (Result == DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH) || (Result == DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING))

		if(WaitResult == DBOS_OBJECT_WAIT_NO_WAIT){
			Result = DBOS_Q_WRITTEN;
		}
		else{
			Result = DBOS_Q_WRITTEN_WAITED;
		}

		break;
	case DBOS_OBJECT_WAIT_TIMEOUT:
		Result = DBOS_Q_WRITE_FAIL_FULL;
		break;
	case DBOS_OBJECT_WAIT_WAITED_TIMEOUT:
		Result = DBOS_Q_WRITE_FAIL_FULL_TIMEOUT;
		break;
	default:
		ASSERT_ERROR_FROM_OSCS()
		break;
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return Result;
}

DBOS_RESULT_t dBOS_KERNELINTERFACE::Queue_Read(dBOS_QUEUEINTERFACE_P_t const pQueue, void * const pData, DBOS_TIMECOUNT_t const Time, BOOL const ReadFromFront){

	DBOS_RESULT_t Result;
	DBOS_RESULT_t WaitResult;
	DBOS_RESULT_t TakeResult;

	DBOS_RESULT_t QueueReadResult;

	INTERRUPT_MASK_t ISRMask;
	INTERRUPT_MASK_t PrevISRMask;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	DBOS_SYNCBASE_P_t const pEmptySync = pQueue->m_pSemaphoreEmptyCount;
	DBOS_SYNCBASE_P_t const pFullSync = pQueue->m_pSemaphoreFullCount;

	ASSERT_FROM_OSCS(pEmptySync != NULLPTR)
	ASSERT_FROM_OSCS(pFullSync != NULLPTR)

	ASSERT_FROM_OSCS(pEmptySync->GetISRMask() == pFullSync->GetISRMask())

	if(Time == TIMEWAITFOREVER){
		// Timer is not going to be used.
		ISRMask = pEmptySync->GetISRMask();
	}
	else{
		ISRMask = m_pImpl->GetHighestISRMask(ISRMASK_OSTIMER, &pEmptySync, 1U);
	}

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);	// Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	SIGNALLED_DATA_t SignalData = SIGNALDATANONE;

	WaitResult = m_pImpl->CheckAndWaitCurrentTaskOnObject(pFullSync, &SignalData, Time, ISRMask, &PrevISRMask, NULLPTR);

	switch(WaitResult){
	case DBOS_OBJECT_WAIT_NO_WAIT:
	case DBOS_OBJECT_WAIT_WAITED: // deliberate fall through.

		TakeResult = pFullSync->TakeObject(&SignalData, m_pImpl->GetCurrentTask()); // Queue Semaphores Are Not Taken Automatically After Waiting
		ASSERT_FROM_OSCS(TakeResult == DBOS_DONE)

		if(ReadFromFront == TRUE){
			QueueReadResult = pQueue->m_pImpl->ReadFromFront(pData);
		}
		else{
			QueueReadResult = pQueue->m_pImpl->ReadFromBack(pData);
		}

		ASSERT_FROM_OSCS(QueueReadResult == DBOS_Q_READ)

		Result = m_pImpl->CurrentTaskSignalObject(pEmptySync, 0U, SIGNAL_AUTO, TRUE, ISRMask, &PrevISRMask);

		ASSERT_FROM_OSCS((Result == DBOS_OBJECT_SIGNALLED_TASK_SWITCHED) || (Result == DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH) || (Result == DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING))

		if(WaitResult == DBOS_OBJECT_WAIT_NO_WAIT){
			Result = DBOS_Q_READ;
		}
		else{
			Result = DBOS_Q_READ_WAITED;
		}

		break;
	case DBOS_OBJECT_WAIT_TIMEOUT:
		Result = DBOS_Q_READ_FAIL_EMPTY;
		break;
	case DBOS_OBJECT_WAIT_WAITED_TIMEOUT:
		Result = DBOS_Q_READ_FAIL_EMPTY_TIMEOUT;
		break;
	default:
		ASSERT_ERROR_FROM_OSCS()
		break;
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return Result;
}
// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
