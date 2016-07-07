/**
 * @file dBOS_Mutex.cpp
 * @author David Broadhurst
 * @date 17/01/2015
 *
 * \brief Implementation of Mutex synchronization object.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Mutex.hpp"
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
dBOS_MUTEX::dBOS_MUTEX(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType) :
		dBOS_SYNC(ID, SyncID, WaitListType), m_Count(1), m_pOwner(NULLPTR), m_Link(this), m_SignalData(SIGNALDATANONE){

}

dBOS_MUTEX::~dBOS_MUTEX(){

	m_pOwner = NULLPTR;
}

SIGNALLED_DATA_t dBOS_MUTEX::GetSignalData(void) const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return m_SignalData;
}

DBOS_TASK_P_t dBOS_MUTEX::GetOwnerTask(void) const{

	return m_pOwner;
}

DBOS_RESULT_t dBOS_MUTEX::FROMISR_InitialWaitCheck() const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return DBOS_SYNC_OBJECT_INVALID_OBJECT_TYPE;
}

DBOS_RESULT_t dBOS_MUTEX::FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_SYNC_OBJECT_INVALID_OBJECT_TYPE;

	return Result;
}

DBOS_RESULT_t dBOS_MUTEX::FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode){

	(void) SignalData; // Suppress unused parameter compiler warning.
	(void) SignalMode;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return DBOS_SYNC_OBJECT_INVALID_OBJECT_TYPE;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
void dBOS_MUTEX::PrintDetails() const{

	DBOS_SYNCBASE_CONST_P_t pStartObject;
	DBOS_SYNCBASE_CONST_P_t pWaitObject;
	DBOS_TASK_CONST_P_t pOwnerTask;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	this->PrintTypeAndID();

	DBOS_Intprintf("\n  Count: %i", m_Count);

	dBOS_SYNC::PrintDetails();

	DBOS_Intprintf("\n  Signal Data: %u", m_SignalData);

	pStartObject = this;
	pOwnerTask = m_pOwner;

	if(pOwnerTask != NULLPTR){

		DBOS_Intprintf("\n  Owner Task: ");
		pOwnerTask->PrintID();

		if(pOwnerTask->GetState() == DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS){

			pWaitObject = pOwnerTask->PrintState();
			// Print State will will return a pointer to the object in the task's waitlist that has an owner task.
			// Otherwise returns NULLPTR

			while((pWaitObject != NULLPTR) && (pStartObject != pWaitObject)){
				// pWaitObject is the Mutex the owner task is waiting on.

				DBOS_Intprintf("\n    ");

				pOwnerTask = pWaitObject->GetOwnerTask();

				ASSERT_FROM_OSCS(pOwnerTask != NULLPTR)
				// There are tasks waiting on the Mutex therefore it must have an owner task.

				DBOS_Intprintf("\n  Owner Task: ");
				pOwnerTask->PrintID();

				if(pOwnerTask->GetState() == DBOS_TASK_WAITING_ON_ONE_OF_N_OBJECTS){
					DBOS_Intprintf(" ");
					pWaitObject = pOwnerTask->PrintState();
					// Print State will will return a pointer to the object in the task's waitlist that has an owner task.
					// Otherwise returns NULLPTR
				}
				else{
					pWaitObject = NULLPTR;
				}

			}

			if(pStartObject == pWaitObject){
				DBOS_Intprintf(" -> DEADLOCK");
			}
			else{
				DBOS_Intprintf(" ");
				pWaitObject = pOwnerTask->PrintState();
				// Print State will will return a pointer to the object in the task's waitlist that has an owner task.
				// Otherwise returns NULLPTR

				ASSERT_FROM_OSCS(pWaitObject == NULLPTR)
				// Case where a mutex is one of the objects being waited on should have been handled above.
			}

		}
		else{
			DBOS_Intprintf(" ");
			pWaitObject = pOwnerTask->PrintState();
			// Print State will will return a pointer to the object in the task's waitlist that has an owner task.
			// Otherwise returns NULLPTR

			ASSERT_FROM_OSCS(pWaitObject == NULLPTR)
		}
	}
	else{
		DBOS_Intprintf("\n  Owner: None");
	}

	return;
}

void dBOS_MUTEX::PrintTypeAndID() const{

	DBOS_Intprintf("Mutex: ");
	this->PrintID();

	return;
}
#endif

SNATIVECPU_t dBOS_MUTEX::GetCount(void) const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return m_Count;
}

/**
 * \brief Inserts a Task into an object's wait list.
 *
 * \note This includes <b>raising</b> the priority of the object, <b>raising</b> the priority of the owner task if required. Followed by full priority inheritance.
 *
 * @see DBOS_Task_RaiseTaskPriority
 *
 * @param[in] pTaskWaitLink Pointer to dBOS_WAITLINK of Task
 * @retval DBOS_DONE Task added to object's waitlist.
 * @retval DBOS_TASK_RUNNING_TASK_PRIORITY_RAISED
 * @retval DBOS_TASK_READY_TASK_PRIORITY_RAISED
 */
DBOS_RESULT_t dBOS_MUTEX::InsertTaskIntoObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink){

	DBOS_RESULT_t Result;

	Result = dBOS_SYNC::InsertTaskIntoObjectWaitList(pTaskWaitLink);

	ASSERT_FROM_OSCS(Result == DBOS_DONE)

	// Priority Inheritance
	// Task has been added to Mutex Wait List, priority of mutex might have been raised.

	ASSERT_FROM_OSCS(m_pOwner != NULLPTR)
	// There are Tasks waiting on the mutex, it must have an owner.

	if(dBOS_SYNC::GetPriority() > m_pOwner->GetPriority()){
		Result = m_pOwner->RaiseTaskPriority(dBOS_SYNC::GetPriority());
	}
	else{
		Result = DBOS_DONE;
	}

	return Result;
}

/**
 * \brief Removes the task at the front of the object's waitlist front the waitlist.
 *
 * @retval NULLPTR The object's waitlist was empty.
 * @retval Pointer to the task that was at the front of the object's waitlist.
 */
DBOS_TASK_P_t dBOS_MUTEX::FromFrontRemoveTaskFromObjectWaitList(void){

	DBOS_TASK_P_t pResult;

	pResult = dBOS_SYNC::FromFrontRemoveTaskFromObjectWaitList();

	ASSERT_FROM_OSCS(m_pOwner == NULLPTR)
	// Mutex shouldn't have an owner at this point.
	// The previous owner has signalled (released) it and the task just removed from the front of the wait list hasn't taken it yet.

	return pResult;
}

/**
 * \brief Removes a task from an object's waitlist.
 *
 * \note This includes <b>lowering</b> the priority of the object, <b>lowering</b> the priority of the owner task if required. Followed by full priority inheritance.
 *
 * @see DBOS_Task_CalcTaskPriorityFromHeldMutexObjects TODO
 * @see DBOS_Task_RaiseTaskPriority TODO
 *
 * @param[in] pTaskWaitLink Pointer to dBOS_WAITLINK of Task
 * @retval DBOS_DONE Task removed from the object's waitlist.
 * @retval DBOS_TASK_RUNNING_TASK_PRIORITY_LOWERED
 * @retval DBOS_TASK_READY_TASK_PRIORITY_LOWERED
 */
DBOS_RESULT_t dBOS_MUTEX::RemoveTaskFromObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink){

	DBOS_RESULT_t Result;

	DBOS_TASK_PRIORITY_t NewPriority;

	Result = dBOS_SYNC::RemoveTaskFromObjectWaitList(pTaskWaitLink);

	// Priority Inheritance
	// Task was removed from the Mutex wait list so priority of Mutex might have lowered.

	ASSERT_FROM_OSCS(m_pOwner != NULLPTR)
	// There are task(s) waiting on the Mutex therefore it must have an owner task.

	NewPriority = m_pOwner->CalcTaskPriorityFromHeldMutexObjects();

	if(NewPriority < m_pOwner->GetPriority()){
		Result = m_pOwner->LowerTaskPriority(NewPriority);
	}
	else{
		Result = DBOS_DONE;
	}

	return Result;
}

dBOS_NONRECURSIVEMUTEX::dBOS_NONRECURSIVEMUTEX(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType) :
		dBOS_MUTEX(ID, SyncID, WaitListType){

}

dBOS_NONRECURSIVEMUTEX::~dBOS_NONRECURSIVEMUTEX(){

}

DBOS_RESULT_t dBOS_NONRECURSIVEMUTEX::InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	DBOS_MUTEX_CONST_P_t const pLastAddedMutex = pCurrentTask->GetLastAddedMutex();

	if(m_pOwner != NULLPTR){
		ASSERT_FROM_OSCS(m_Count <= 0)

		if(m_pOwner == pCurrentTask){
			Result = DBOS_MUTEX_ERROR_NOT_A_RECURSIVE_MUTEX;
		}
		else{
			// Has an owner and it's not the current task.
			if(pLastAddedMutex != NULLPTR){
				Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
			}
			else{
				Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
			}
		}
	}
	else{
		// Has no owner.
		ASSERT_FROM_OSCS(m_Count == 1)
		ASSERT_FROM_OSCS(m_Link.CheckUnlinked() == TRUE)

		if(pLastAddedMutex != NULLPTR){
			Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
		}
		else{
			Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
		}
	}

	return Result;
}

DBOS_RESULT_t dBOS_NONRECURSIVEMUTEX::PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(m_pOwner == pCurrentTask){ // Task already owns the mutex (mutex has been given to the current task)
		ASSERT_FROM_OSCS(m_Count == 0)
		ASSERT_FROM_OSCS(pCurrentTask->GetLastAddedMutex() == this)

		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else if(m_Count > 0){
		// Mutex is available.
		ASSERT_FROM_OSCS(m_Link.CheckUnlinked() == TRUE)
		ASSERT_FROM_OSCS(m_pOwner == NULLPTR)
		ASSERT_FROM_OSCS(m_Count == 1)

		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		// Mutex is not available.
		ASSERT_FROM_OSCS(m_pOwner != NULLPTR)
		ASSERT_FROM_OSCS(m_pOwner != pCurrentTask)
		ASSERT_FROM_OSCS(m_Count == 0)

		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_NONRECURSIVEMUTEX::GiveObject(DBOS_TASK_P_t const pRxTask){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pRxTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	ASSERT_FROM_OSCS(m_Link.CheckUnlinked() == TRUE)
	ASSERT_FROM_OSCS(m_pOwner == NULLPTR)
	ASSERT_FROM_OSCS(m_Count == 1)

	if((this->GetPriority() != INVALID_PRIORITY_LEVEL) && (this->GetWaitListOrderType() == dBOS_TASKQUEUE_ORDER_PRIORITY)){
		ASSERT_FROM_OSCS(pRxTask->GetPriority() >= this->GetPriority())
		// When the wait list is priority based then the
		// The task the mutex is been given to should a have priority higher or equal to the highest priority task waiting to acquire the mutex.
	}

	pRxTask->AddMutex(&m_Link);

	m_pOwner = pRxTask;
	m_Count--;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_NONRECURSIVEMUTEX::TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask){
	// Note: all clear to take the object has already been given by InitialWaitCheck or PostWaitCheck
	// NOTE: Mutex may already have been given to the current task.

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)
	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	ASSERT_FROM_OSCS((m_pOwner == NULLPTR) || (m_pOwner == pCurrentTask))

	if(m_pOwner == NULLPTR){
		ASSERT_FROM_OSCS(m_Link.CheckUnlinked() == TRUE)
		ASSERT_FROM_OSCS(m_Count == 1)
	}

	*pSignalData = m_SignalData;

	if((this->GetPriority() != INVALID_PRIORITY_LEVEL) && (this->GetWaitListOrderType() == dBOS_TASKQUEUE_ORDER_PRIORITY)){
		ASSERT_FROM_OSCS(pCurrentTask->GetPriority() >= this->GetPriority())
		// When the wait list is priority based then the
		// task acquiring the mutex should have priority higher or equal to highest priority task waiting to acquire the mutex.
	}

	if(m_pOwner == NULLPTR){
		pCurrentTask->AddMutex(&m_Link);

		m_pOwner = pCurrentTask;
		m_Count--;
	}

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_NONRECURSIVEMUTEX::SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask){

	(void) SignalMode; // Suppress unused parameter compiler warning.

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(pCurrentTask != m_pOwner){
		// No mutexes acquired

		if(m_pOwner == NULLPTR){
			Result = DBOS_MUTEX_ERROR_HAS_NO_OWNER;
		}
		else{
			Result = DBOS_MUTEX_ERROR_THIS_TASK_NOT_THE_OWNER;
		}
	}
	else{

		DBOS_MUTEX_CONST_P_t const pLastAddedMutex = pCurrentTask->GetLastAddedMutex();

		ASSERT_FROM_OSCS(pLastAddedMutex != NULLPTR)

		if(pLastAddedMutex != this){
			Result = DBOS_MUTEX_ERROR_IS_NOT_LAST_MUTEX_ACQUIRED;
		}
		else{
			m_SignalData = SignalData;

			pCurrentTask->RemoveMutexAndUpdatePriority(); // TODO Should not be ignoring return value here.

			ASSERT_FROM_OSCS(m_Link.CheckUnlinked() == TRUE)

			m_pOwner = NULLPTR;

			m_Count++;

			ASSERT_FROM_OSCS(m_Count == 1)

			Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
		}
	}

	return Result;
}

dBOS_RECURSIVEMUTEX::dBOS_RECURSIVEMUTEX(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType) :
		dBOS_MUTEX(ID, SyncID, WaitListType), m_pRxTask(NULLPTR){

}

dBOS_RECURSIVEMUTEX::~dBOS_RECURSIVEMUTEX(){

}

DBOS_RESULT_t dBOS_RECURSIVEMUTEX::InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	DBOS_MUTEX_CONST_P_t const pLastAddedMutex = pCurrentTask->GetLastAddedMutex();

	ASSERT_FROM_OSCS(m_pRxTask == NULLPTR)

	if(m_pOwner != NULLPTR){
		ASSERT_FROM_OSCS(m_Count <= 0)

		if(m_pOwner == pCurrentTask){
			if(pLastAddedMutex != this){
				Result = DBOS_MUTEX_ERROR_IS_NOT_LAST_MUTEX_ACQUIRED;
			}
			else{
				// Current task is the owner and this was the last mutex the task acquired.
				Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
			}
		}
		else{
			// Has an owner and it's not the current task.
			if(pLastAddedMutex != NULLPTR){
				Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
			}
			else{
				Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
			}
		}
	}
	else{
		// Has no owner.
		ASSERT_FROM_OSCS(m_Count == 1)
		ASSERT_FROM_OSCS(m_Link.CheckUnlinked() == TRUE)

		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}

	return Result;
}

DBOS_RESULT_t dBOS_RECURSIVEMUTEX::PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(m_Count > 0){
		// Mutex is available.
		ASSERT_FROM_OSCS(m_Link.CheckUnlinked() == TRUE)
		ASSERT_FROM_OSCS(m_pOwner == NULLPTR)
		ASSERT_FROM_OSCS(m_Count == 1)

		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else if(m_pOwner == pCurrentTask){
		// Mutex already owns this task.

		ASSERT_FROM_OSCS(m_Count < 1)
		ASSERT_FROM_OSCS(pCurrentTask->GetLastAddedMutex() == this)
		// Mutex must be the most recent one added to the list of owned mutexes

		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		// Mutex is not available.

		ASSERT_FROM_OSCS(m_pOwner != NULLPTR)

		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_RECURSIVEMUTEX::GiveObject(DBOS_TASK_P_t const pRxTask){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pRxTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	ASSERT_FROM_OSCS(m_Link.CheckUnlinked() == TRUE)
	ASSERT_FROM_OSCS(m_pOwner == NULLPTR)
	ASSERT_FROM_OSCS(m_pRxTask == NULLPTR)
	ASSERT_FROM_OSCS(m_Count == 1)

	if((this->GetPriority() != INVALID_PRIORITY_LEVEL) && (this->GetWaitListOrderType() == dBOS_TASKQUEUE_ORDER_PRIORITY)){
		ASSERT_FROM_OSCS(pRxTask->GetPriority() >= this->GetPriority())
		// When the wait list is priority based then the
		// task the mutex is been given to should a have priority higher or equal to the highest priority task waiting to acquire the mutex.
	}

	pRxTask->AddMutex(&m_Link);

	m_pRxTask = pRxTask;
	m_pOwner = pRxTask;
	m_Count--;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_RECURSIVEMUTEX::TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask){
	// Note all clear to take the object has already been given by InitialWaitCheck or PostWaitCheck
	// NOTE: Mutex may already have been given to the current task.

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)
	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	ASSERT_FROM_OSCS(m_Count <= 1)

	if(m_Count == 1){
		ASSERT_FROM_OSCS(m_Link.CheckUnlinked() == TRUE)
		ASSERT_FROM_OSCS(m_pOwner == NULLPTR)
		ASSERT_FROM_OSCS(m_pRxTask == NULLPTR)

		*pSignalData = m_SignalData;

		if((this->GetPriority() != INVALID_PRIORITY_LEVEL) && (this->GetWaitListOrderType() == dBOS_TASKQUEUE_ORDER_PRIORITY)){
			ASSERT_FROM_OSCS(pCurrentTask->GetPriority() >= this->GetPriority())
			// When the wait list is priority based then the
			// task acquiring the mutex should have priority higher or equal to highest priority task waiting to acquire the mutex.
		}

		pCurrentTask->AddMutex(&m_Link);

		m_pOwner = pCurrentTask;
		m_Count--;

		Result = DBOS_DONE;
	}
	else{
		ASSERT_FROM_OSCS(m_pOwner == pCurrentTask)
		ASSERT_FROM_OSCS(pCurrentTask->GetLastAddedMutex() == this)

		*pSignalData = m_SignalData;

		if(m_pRxTask == m_pOwner){
			m_pRxTask = NULLPTR; // If mutex has already been given.
		}
		else{
			m_Count--;
		}

		Result = DBOS_DONE;
	}

	return Result;
}

DBOS_RESULT_t dBOS_RECURSIVEMUTEX::SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask){

	(void) SignalMode; // Suppress unused parameter compiler warning.

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(pCurrentTask != m_pOwner){
		// No mutexes acquired

		if(m_pOwner == NULLPTR){
			Result = DBOS_MUTEX_ERROR_HAS_NO_OWNER;
		}
		else{
			Result = DBOS_MUTEX_ERROR_THIS_TASK_NOT_THE_OWNER;
		}
	}
	else{

		DBOS_MUTEX_CONST_P_t const pLastAddedMutex = pCurrentTask->GetLastAddedMutex();

		ASSERT_FROM_OSCS(pLastAddedMutex != NULLPTR)

		if(pLastAddedMutex != this){
			Result = DBOS_MUTEX_ERROR_IS_NOT_LAST_MUTEX_ACQUIRED;
		}
		else{
			m_SignalData = SignalData;

			m_Count++;

			if(m_Count > 0){
				ASSERT_FROM_OSCS(m_Count == 1)

				pCurrentTask->RemoveMutexAndUpdatePriority(); // TODO Should not be ignoring return value here.

				ASSERT_FROM_OSCS(m_Link.CheckUnlinked() == TRUE)

				m_pOwner = NULLPTR;

				Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
			}
			else{
				Result = DBOS_OBJECT_SIGNAL_NO_TASKS;
			}

		}
	}

	return Result;
}

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
