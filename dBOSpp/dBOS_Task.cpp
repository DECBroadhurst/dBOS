/**
 * @file dBOS_Task.cpp
 * @author David Broadhurst
 * @date 14/01/2015
 *
 * \brief Implementation of dBOS Task.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Task.hpp"
#include "dBOS_Sync.hpp"
#include "dBOS_Mutex.hpp"
#include "dBOS_Assert.hpp"
#include "dBOS_Port.hpp"
#include "dBOS_PortFunctions.hpp"
#include "dBOS_Kernel.hpp"

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
dBOS_KERNEL * dBOS_TASK::m_pK = NULLPTR;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
dBOS_TASK::dBOS_TASK(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const UniqueSyncID, DBOS_TASK_PRIORITY_t const Priority, DBOS_PNTEQUIV_t const TaskStackSize, DBOS_STK_t const * const pStackSpace,
		DBOS_TASKENTRY_FUNCTION_PNT_t const pTaskFunction, void const * const pTaskEntryData, dBOS_TASKINTERFACE_P_t const pInterface) :
		dBOS_BASE(ID), m_TaskTimer(ID, UniqueSyncID), m_pInterface(pInterface), m_State(DBOS_TASK_STARTUP), m_MutexList(), m_PresetPriority(Priority), m_CurrentPriority(Priority), m_pWaitLinks(NULLPTR), m_pWaitObjects(
		NULLPTR), m_pSignalledObject(NULLPTR), m_NumberOfObjectsWaitingOn(0U), m_ReadyLink(this), m_StackPointer(NULLPTR), m_TaskStackSize(TaskStackSize), m_pStackSpace(pStackSpace){

	m_pK = dBOS_KERNEL::GetInstance();

	m_StackPointer = DBOS_Port_InitTaskStack(m_pStackSpace, m_TaskStackSize, pTaskFunction, pTaskEntryData);

}

dBOS_TASK::~dBOS_TASK(void){

	m_StackPointer = NULLPTR;
	m_pSignalledObject = NULLPTR;
	m_pWaitLinks = NULLPTR;
	m_pWaitObjects = NULLPTR;

}

DBOS_TASK_PRIORITY_t dBOS_TASK::GetPriority(void) const{

	return m_CurrentPriority;
}

DBOS_TASK_STATE_t dBOS_TASK::GetState(void) const{

	return m_State;
}

DBOS_SYNCBASE_CONST_P_t dBOS_TASK::GetSignalledObject(void) const{

	return m_pSignalledObject;
}

DBOS_STKPTR_t const * dBOS_TASK::GetStackPointerAddress(void) const{

	return &m_StackPointer;
}

DBOS_RESULT_t dBOS_TASK::RaiseTaskPriority(DBOS_TASK_PRIORITY_t const NewPriorityIn){

	DBOS_RESULT_t Result;

	DBOS_TASK_P_t pCurrentTask;

	DBOS_SYNCBASE_CONST_P_t pWaitObject;
	DBOS_SYNCBASE_CONST_P_t pStartObject;

	DBOS_TASK_PRIORITY_t NewPriority;

	ASSERT_FROM_OSCS(NewPriorityIn != INVALID_PRIORITY_LEVEL)

	pCurrentTask = this;

	NewPriority = NewPriorityIn;

	ASSERT_FROM_OSCS(NewPriority >= pCurrentTask->m_CurrentPriority)

	if(NewPriority > pCurrentTask->m_CurrentPriority){
		// If the mutex has a higher priority than its owner.

		if(pCurrentTask->m_State == DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS){
			// If the owner is waiting on object(s).

			pWaitObject = pCurrentTask->RequeueTaskInWaitListsAndUpdatePriority(NewPriority);
			// pWaitObject will be a pointer to the mutex the task is waiting on OR equal NULLPTR if none of the objects the task is waiting on is a mutex WITH an owner.

			if(pWaitObject != NULLPTR){
				// pWaitObject should be the only Mutex the task is waiting on.

				pStartObject = pWaitObject;

				do{
					// pWaitObject should be the only Mutex the task is waiting on.
					pCurrentTask = pWaitObject->GetOwnerTask();

					ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)
					// There are task(s) waiting on pWaitObject therefore it must have an owner task.

					NewPriority = pWaitObject->GetPriority();

					// There are task(s) waiting on pMutex so Priority must be valid.
					ASSERT_FROM_OSCS(NewPriority != INVALID_PRIORITY_LEVEL)

					if(NewPriority > pCurrentTask->m_CurrentPriority){
						// If the mutex has a higher priority than its owner.
						if(pCurrentTask->m_State == DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS){
							// If the owner is waiting on object(s).
							pWaitObject = pCurrentTask->RequeueTaskInWaitListsAndUpdatePriority(NewPriority);
							// pWaitObject will be a pointer to the mutex the task is waiting on OR equal NULLPTR if none of the objects the task is waiting on is a mutex WITH an owner.
						}
						else{
							pWaitObject = NULLPTR;
						}
					}
					else{
						pWaitObject = NULLPTR;
					}

				}while((pWaitObject != NULLPTR) && (pWaitObject != pStartObject));

				if(pWaitObject != NULLPTR){
					ASSERT_FROM_OSCS(pWaitObject == pStartObject)
					// TODO Handle Deadlock
				}

			}

		}

	}

	if(NewPriority > pCurrentTask->m_CurrentPriority){
		// Priority of the Mutex is higher than the task that owns it.

		switch(pCurrentTask->m_State){
		case DBOS_TASK_RUNNING:

#if DBOS_COVERAGEPRINTOUTPUT == 1U
			COVERAGE_PRINT("Running Task RAISE Priority")
			DBOS_Intprintf(" - New: %u Current: %u Task: ", NewPriority, pCurrentTask->m_CurrentPriority);
			pCurrentTask->PrintID();
#endif

#if DBOS_TESTSENABLED == 1U
			DBOS_Test_LogEvent('!');
#endif

			pCurrentTask->m_CurrentPriority = NewPriority;

			Result = DBOS_RUNNING_TASK_PRIORITY_RAISED;
			break;
		case DBOS_TASK_READY_TO_RUN:
			// Re-queue task in ready list.

#if DBOS_COVERAGEPRINTOUTPUT == 1U
			COVERAGE_PRINT("Ready to Run Task RAISE Priority")
			DBOS_Intprintf(" - New: %u Current: %u Task: ", NewPriority, pCurrentTask->m_CurrentPriority);
			pCurrentTask->PrintID();
#endif

#if DBOS_TESTSENABLED == 1U
			DBOS_Test_LogEvent('@');
#endif

			m_pK->RequeueTaskInReadyList(&pCurrentTask->m_ReadyLink, NewPriority);

			pCurrentTask->m_CurrentPriority = NewPriority; // Change priority after call to ReQueue above.

			Result = DBOS_TASK_READY_TASK_PRIORITY_RAISED;
			break;
		case DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS:
			Result = DBOS_INTERNAL_ERROR;
			ASSERT_ERROR_FROM_OSCS()
			// THIS STATE SHOULD HAVE BEEN TAKEN CARE OF ABOVE.
			break;
		case DBOS_TASK_STARTUP:
		default:
			Result = DBOS_INTERNAL_ERROR;
			ASSERT_ERROR_FROM_OSCS()
			break;
		}

	}
	else{
		Result = DBOS_DONE;
	}

	return Result;
}

DBOS_RESULT_t dBOS_TASK::LowerTaskPriority(DBOS_TASK_PRIORITY_t const NewPriorityIn){

	DBOS_RESULT_t Result;

	DBOS_TASK_P_t pCurrentTask;

	DBOS_SYNCBASE_CONST_P_t pWaitObject;
	DBOS_SYNCBASE_CONST_P_t pStartObject;

	DBOS_TASK_PRIORITY_t NewPriority;

	ASSERT_FROM_OSCS(NewPriorityIn != INVALID_PRIORITY_LEVEL)

	pCurrentTask = this;

	NewPriority = NewPriorityIn;

	ASSERT_FROM_OSCS(NewPriority <= pCurrentTask->m_CurrentPriority)

	if(NewPriority < pCurrentTask->m_CurrentPriority){

		if(pCurrentTask->m_State == DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS){
			// If the owner is waiting on object(s).
			pWaitObject = pCurrentTask->RequeueTaskInWaitListsAndUpdatePriority(NewPriority);
			// pWaitObject will be a pointer to the mutex the task is waiting on OR equal NULLPTR if none of the objects the task is waiting on is a mutex WITH an owner.

			if(pWaitObject != NULLPTR){
				// pWaitObject should be the only Mutex the task is waiting on.

				pStartObject = pWaitObject;

				do{
					// pWaitObject should be the only Mutex the task is waiting on.
					pCurrentTask = pWaitObject->GetOwnerTask();

					ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)
					// There are task(s) waiting on pMutex therefore it must have an owner task.

					NewPriority = pCurrentTask->CalcTaskPriorityFromHeldMutexObjects();

					ASSERT_FROM_OSCS(NewPriority <= pCurrentTask->m_CurrentPriority)

					if(NewPriority < pCurrentTask->m_CurrentPriority){
						// The tasks priority needs to be lowered.
						if(pCurrentTask->m_State == DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS){
							// If the owner is waiting on object(s).
							pWaitObject = pCurrentTask->RequeueTaskInWaitListsAndUpdatePriority(NewPriority);
							// pWaitObject will be a pointer to the mutex the task is waiting on OR equal NULL if none of the objects the task is waiting on is a mutex.
						}
						else{
							pWaitObject = NULLPTR;
						}
					}
					else{
						pWaitObject = NULLPTR;
					}

				}while((pWaitObject != NULLPTR) && (pWaitObject != pStartObject));

				if(pWaitObject != NULLPTR){
					ASSERT_FROM_OSCS(pWaitObject == pStartObject)
					// TODO Handle Deadlock
				}

			}

		}

	}

	if(NewPriority < pCurrentTask->m_CurrentPriority){

		switch(pCurrentTask->m_State){
		case DBOS_TASK_RUNNING:

#if DBOS_COVERAGEPRINTOUTPUT == 1U
			COVERAGE_PRINT("Running Task LOWER Priority")
			DBOS_Intprintf(" - New: %u Current: %u Task: ", NewPriority, pCurrentTask->m_CurrentPriority);
			pCurrentTask->PrintID();
#endif

#if DBOS_TESTSENABLED == 1U
			DBOS_Test_LogEvent('#');
#endif

			pCurrentTask->m_CurrentPriority = NewPriority;

			Result = DBOS_RUNNING_TASK_PRIORITY_LOWERED;
			break;
		case DBOS_TASK_READY_TO_RUN:
			// Re-queue task in ready list.

#if DBOS_COVERAGEPRINTOUTPUT == 1U
			COVERAGE_PRINT("Ready to Run Task LOWER Priority")
			DBOS_Intprintf(" - New: %u Current: %u Task: ", NewPriority, pCurrentTask->m_CurrentPriority);
			pCurrentTask->PrintID();
#endif

#if DBOS_TESTSENABLED == 1U
			DBOS_Test_LogEvent('$');
#endif

			m_pK->RequeueTaskInReadyList(&pCurrentTask->m_ReadyLink, NewPriority);

			pCurrentTask->m_CurrentPriority = NewPriority; // Change priority after call to ReQueue above.

			Result = DBOS_TASK_READY_TASK_PRIORITY_LOWERED;
			break;
		case DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS:
			Result = DBOS_INTERNAL_ERROR;
			ASSERT_ERROR_FROM_OSCS()
			// THIS STATE SHOULD HAVE BEEN TAKEN CARE OF ABOVE.
			break;
		case DBOS_TASK_STARTUP:
		default:
			Result = DBOS_INTERNAL_ERROR;
			ASSERT_ERROR_FROM_OSCS()
			break;
		}

	}
	else{
		Result = DBOS_DONE;
	}

	return Result;
}

DBOS_TASK_PRIORITY_t dBOS_TASK::CalcTaskPriorityFromHeldMutexObjects(void) const{

	DBOS_TASK_PRIORITY_t NewPriority;
	DBOS_TASK_PRIORITY_t MutexPriority;

	dBOS_LISTNODE<dBOS_MUTEX> const * pMNCurrent;

	NewPriority = 0U;

	pMNCurrent = m_MutexList.GetHead();

	while(pMNCurrent != NULLPTR){

		MutexPriority = pMNCurrent->GetItem()->GetPriority();

		if(MutexPriority != INVALID_PRIORITY_LEVEL){ // Will be INVALID_PRIORITY_LEVEL if no tasks waiting on the Mutex.
			if(MutexPriority > NewPriority){
				NewPriority = MutexPriority;
			}
		}

		pMNCurrent = m_MutexList.GetNext(pMNCurrent);
	}

	if(NewPriority < m_PresetPriority){
		NewPriority = m_PresetPriority;
	}

	return NewPriority;
}

DBOS_SYNCBASE_CONST_P_t dBOS_TASK::RequeueTaskInWaitListsAndUpdatePriority(DBOS_TASK_PRIORITY_t const NewPriority){

	DBOS_SYNCBASE_CONST_P_t Result;

	UNATIVECPU_t Index;

	Result = NULLPTR;

	ASSERT_FROM_OSCS(m_NumberOfObjectsWaitingOn != 0U)
	ASSERT_FROM_OSCS(m_pWaitObjects != NULLPTR)
	ASSERT_FROM_OSCS(m_pWaitLinks != NULLPTR)

	DBOS_SYNCBASE_P_t const * pPnt = m_pWaitObjects;
	dBOS_LISTNODE<dBOS_TASK> * pWait = m_pWaitLinks;

	for(Index = 0U; Index < m_NumberOfObjectsWaitingOn ; Index++){

		DBOS_SYNCBASE_P_t const pWaitObject = *pPnt;

		if(pWaitObject != NULLPTR){
			// Priority of pTask has been changed so re-queue it in the wait list of the object(s) it is waiting on.
			pWaitObject->RequeueTaskInObjectWaitList(pWait, NewPriority);

			if(pWaitObject->GetOwnerTask() != NULLPTR){
				if(Result == NULLPTR){
					Result = pWaitObject;
				}
				else{
					ASSERT_ERROR_FROM_OSCS()
					// Should only be one object with an owner (if waiting on multiple objects only one can be a mutex).
				}
			}

		}

		pPnt++;
		pWait++;
	}

	m_CurrentPriority = NewPriority;

	return Result;
}

DBOS_RESULT_t dBOS_TASK::RemoveTaskFromWaitListsWithException(DBOS_SYNCBASE_CONST_P_t const pSyncObject){
	// Will not remove task from waitlist of object pSyncObject.

	DBOS_RESULT_t Result;

	UNATIVECPU_t Index;

	ASSERT_FROM_OSCS(m_NumberOfObjectsWaitingOn != 0U)
	ASSERT_FROM_OSCS(m_pWaitObjects != NULLPTR)
	ASSERT_FROM_OSCS(m_pWaitLinks != NULLPTR)

	Result = DBOS_DONE;

	DBOS_SYNCBASE_P_t const * pPnt = m_pWaitObjects;
	dBOS_LISTNODE<dBOS_TASK> * pWait = m_pWaitLinks;

	for(Index = 0U; Index < m_NumberOfObjectsWaitingOn ; Index++){

		DBOS_SYNCBASE_P_t const pWaitObject = *pPnt;

		if((pWaitObject != NULLPTR) && (pWaitObject != pSyncObject)){

			if(pWaitObject->RemoveTaskFromObjectWaitList(pWait) == DBOS_RUNNING_TASK_PRIORITY_LOWERED){

				Result = DBOS_RUNNING_TASK_PRIORITY_LOWERED;
			}

		}

		pPnt++;
		pWait++;
	}

	return Result;
}

void dBOS_TASK::AddMutex(dBOS_LISTNODE<dBOS_MUTEX> * const pLink){

	m_MutexList.AddItemToFront(pLink);

	return;
}

DBOS_MUTEX_CONST_P_t dBOS_TASK::GetLastAddedMutex(void) const{

	DBOS_MUTEX_CONST_P_t Result;

	dBOS_LISTNODE<dBOS_MUTEX> const * pHead;

	pHead = m_MutexList.GetHead();

	if(pHead == NULLPTR){
		Result = NULLPTR;
	}
	else{
		Result = pHead->GetItem();
	}

	return Result;
}

DBOS_RESULT_t dBOS_TASK::RemoveMutexAndUpdatePriority(){

	DBOS_RESULT_t Result;

	DBOS_TASK_PRIORITY_t NewPriority;

	dBOS_LISTNODE<dBOS_MUTEX> const * pHead;

	pHead = m_MutexList.RemoveHead();

	ASSERT_FROM_OSCS(pHead != NULLPTR)

	NewPriority = this->CalcTaskPriorityFromHeldMutexObjects();

	ASSERT_FROM_OSCS(NewPriority <= m_CurrentPriority)

	if(NewPriority < m_CurrentPriority){
		Result = this->LowerTaskPriority(NewPriority);

		ASSERT_FROM_OSCS(Result == DBOS_RUNNING_TASK_PRIORITY_LOWERED)
		// The running task has called LowerTaskPriority.
	}
	else{
		Result = DBOS_DONE;
	}

	return Result;
}

void dBOS_TASK::ChangedSignalledObjectOfRunningTask(DBOS_SYNCBASE_CONST_P_t const pSyncObject){

	ASSERT_FROM_OSCS(m_State == DBOS_TASK_RUNNING)
	ASSERT_FROM_OSCS(pSyncObject != NULLPTR)

	m_pSignalledObject = pSyncObject;

	return;
}

void dBOS_TASK::SetStartUpState(void){

	ASSERT_FROM_OSCS(m_State == DBOS_TASK_STARTUP)
	ASSERT_FROM_OSCS(m_pSignalledObject == NULLPTR)
	ASSERT_FROM_OSCS(m_NumberOfObjectsWaitingOn == 0U)
	ASSERT_FROM_OSCS(m_pWaitObjects == NULLPTR)
	ASSERT_FROM_OSCS(m_pWaitLinks == NULLPTR)

	return;
}

void dBOS_TASK::SetRunningState(DBOS_SYNCBASE_CONST_P_t const pSyncObject){

	ASSERT_FROM_OSCS(m_State != DBOS_TASK_RUNNING)
	ASSERT_FROM_OSCS(m_State != DBOS_TASK_READY_TO_RUN)
	// If in ready to run state then Sync object shouldn't need to change, use method below instead.
	ASSERT_FROM_OSCS(pSyncObject != NULLPTR)

	m_State = DBOS_TASK_RUNNING;
	m_pSignalledObject = pSyncObject;
	m_NumberOfObjectsWaitingOn = 0U;
	m_pWaitObjects = NULLPTR;
	m_pWaitLinks = NULLPTR;

	return;
}

void dBOS_TASK::SetRunningState(void){

	ASSERT_FROM_OSCS(m_State == DBOS_TASK_READY_TO_RUN)
	// must be switching from Ready to Run since this function does not change the signalled object.

	m_State = DBOS_TASK_RUNNING;
	m_NumberOfObjectsWaitingOn = 0U;
	m_pWaitObjects = NULLPTR;
	m_pWaitLinks = NULLPTR;

	return;
}

void dBOS_TASK::SetRunningStateFromStartup(void){

	ASSERT_FROM_OSCS(m_State == DBOS_TASK_STARTUP)
	// must be switching from Startup State.

	m_State = DBOS_TASK_RUNNING;
	m_NumberOfObjectsWaitingOn = 0U;
	m_pWaitObjects = NULLPTR;
	m_pWaitLinks = NULLPTR;

	return;
}

void dBOS_TASK::SetReadyToRunState(DBOS_SYNCBASE_CONST_P_t const pSyncObject){

	ASSERT_FROM_OSCS(m_State != DBOS_TASK_READY_TO_RUN)
	ASSERT_FROM_OSCS(m_State != DBOS_TASK_RUNNING)
	// If in the running state then SyncObject shouldn't need to change, use method below instead.
	ASSERT_FROM_OSCS(pSyncObject != NULLPTR)

	m_State = DBOS_TASK_READY_TO_RUN;
	m_pSignalledObject = pSyncObject;
	m_NumberOfObjectsWaitingOn = 0U;
	m_pWaitObjects = NULLPTR;
	m_pWaitLinks = NULLPTR;

	return;
}

void dBOS_TASK::SetReadyToRunState(void){

	ASSERT_FROM_OSCS(m_State == DBOS_TASK_RUNNING)
	// must be switching from Running State since this function does not change the signalled object.

	m_State = DBOS_TASK_READY_TO_RUN;
	m_NumberOfObjectsWaitingOn = 0U;
	m_pWaitObjects = NULLPTR;
	m_pWaitLinks = NULLPTR;

	return;
}

void dBOS_TASK::SetReadyToRunStateFromStartup(){

	ASSERT_FROM_OSCS(m_State == DBOS_TASK_STARTUP)
	// must be switching from Startup State.

	m_State = DBOS_TASK_READY_TO_RUN;
	m_NumberOfObjectsWaitingOn = 0U;
	m_pWaitObjects = NULLPTR;
	m_pWaitLinks = NULLPTR;

	return;
}

void dBOS_TASK::SetWaitingOnMultipleObjectsWithTimeOut(DBOS_SYNCBASE_P_t * const pObjects, dBOS_LISTNODE<dBOS_TASK> * const pWaitLinks, UNATIVECPU_t const NumOfObjects){

	UNATIVECPU_t Index;
	UNATIVECPU_t Count;

	ASSERT_FROM_OSCS(pObjects != NULLPTR)
	ASSERT_FROM_OSCS(m_State != DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS)

	Count = 0U;

	DBOS_SYNCBASE_P_t const * pPnt = pObjects;
	dBOS_LISTNODE<dBOS_TASK> * pWait = pWaitLinks;

	for(Index = 0U; Index < NumOfObjects ; Index++){

		DBOS_SYNCBASE_P_t const pWaitObject = *pPnt;

		if(pWaitObject != NULLPTR){
			pWaitObject->InsertTaskIntoObjectWaitList(pWait); // TODO deal with return value.
			Count++;
		}

		pPnt++;
		pWait++;
	}

	ASSERT_FROM_OSCS(Count != 0U)

	m_State = DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS;
	m_pSignalledObject = NULLPTR;
	m_NumberOfObjectsWaitingOn = Count;
	m_pWaitObjects = pObjects;
	m_pWaitLinks = pWaitLinks;

	return;
}

dBOS_LISTNODE<dBOS_TASK> * dBOS_TASK::GetpReadyLink(void){

	return &m_ReadyLink;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
void dBOS_TASK::PrintDetails(void) const{

	this->PrintTypeAndID();

	DBOS_Intprintf("\n  ");
	this->PrintState();

	DBOS_Intprintf("\n  Current Priority: %u", m_CurrentPriority);

	DBOS_Intprintf("\n  Preset Priority: %u", m_PresetPriority);

	DBOS_Intprintf("\n  Free Stack Space (Words): %u", DBOS_Port_CountFreeStackSpaces(m_pStackSpace, m_TaskStackSize));

	DBOS_Intprintf("\n  Signalled Object: ");
	if(m_pSignalledObject != NULLPTR){
		m_pSignalledObject->PrintTypeAndID();
	}
	else{
		DBOS_Intprintf("NONE");
	}

	// Disable timer ISR when accessing timer.
	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	DBOS_Intprintf("\n  Timer Count: %i", m_TaskTimer.GetTime());

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Intprintf("\n  Number Of Objects Waiting On: %u", m_NumberOfObjectsWaitingOn);

	dBOS_LISTNODE<dBOS_MUTEX> const * pMNCurrent;

	pMNCurrent = m_MutexList.GetHead();

	if(pMNCurrent != NULLPTR){
		DBOS_Intprintf("\n  Owned Mutexes: ");

		pMNCurrent->GetItem()->PrintID();

		pMNCurrent = m_MutexList.GetNext(pMNCurrent);

		while(pMNCurrent != NULLPTR){

			DBOS_Intprintf(", ");

			pMNCurrent->GetItem()->PrintID();

			pMNCurrent = m_MutexList.GetNext(pMNCurrent);
		}

	}
	else{
		DBOS_Intprintf("\n  Owned Mutexes: NONE");
	}

	return;
}

void dBOS_TASK::PrintTypeAndID(void) const{

	DBOS_Intprintf("Task: ");
	this->PrintID();

	return;
}

void dBOS_TASK::PrintFreeStackSpace(void) const{

	DBOS_Intprintf("\n");
	this->PrintTypeAndID();
	DBOS_Intprintf(" Free Stack Space (Words): %u", DBOS_Port_CountFreeStackSpaces(m_pStackSpace, m_TaskStackSize));

	return;
}

DBOS_SYNCBASE_CONST_P_t dBOS_TASK::PrintState(void) const{

	DBOS_SYNCBASE_CONST_P_t pResult;

	DBOS_Intprintf("State: ");

	switch(m_State){
	case DBOS_TASK_STARTUP:

		DBOS_Intprintf("Start Up");
		pResult = NULLPTR;

		break;
	case DBOS_TASK_RUNNING:

		DBOS_Intprintf("Running");
		pResult = NULLPTR;

		break;
	case DBOS_TASK_READY_TO_RUN:

		DBOS_Intprintf("Ready To Run");
		pResult = NULLPTR;

		break;
	case DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS:

		DBOS_Intprintf("Waiting On Objects:");
		pResult = PrintObjectsTaskWaitingOn();

		break;
	default:
		ASSERT_ERROR_FROM_OSCS()
		pResult = NULLPTR;
		break;
	}

	return pResult;
}

DBOS_SYNCBASE_CONST_P_t dBOS_TASK::PrintObjectsTaskWaitingOn(void) const{

	DBOS_SYNCBASE_CONST_P_t pResult;

	UNATIVECPU_t Index;

	pResult = NULLPTR;

	ASSERT_FROM_OSCS(m_pWaitObjects != NULLPTR)
	ASSERT_FROM_OSCS(m_NumberOfObjectsWaitingOn != 0U)

	DBOS_SYNCBASE_P_t const * pPnt = m_pWaitObjects;

	for(Index = 0U; Index < m_NumberOfObjectsWaitingOn ; Index++){

		DBOS_SYNCBASE_CONST_P_t const pWaitObject = *pPnt;

		if(pWaitObject != NULLPTR){

			DBOS_Intprintf("\n    ");
			pWaitObject->PrintTypeAndID();

			if(pWaitObject->GetOwnerTask() != NULLPTR){
				if(pResult == NULLPTR){
					pResult = pWaitObject;
				}
				else{
					ASSERT_ERROR_FROM_OSCS()
					// Already found a MUTEX object, there should only be one.
				}
			}

		}

		pPnt++;
	}

	return pResult;
}

#endif

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
