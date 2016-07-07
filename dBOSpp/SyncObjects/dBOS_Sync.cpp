/**
 * @file dBOS_Sync.cpp
 * @author David Broadhurst
 * @date 15/01/2015
 *
 * \brief Implementation of synchronization object base classes.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Sync.hpp"
#include "dBOS_Task.hpp"
#include "dBOS_List.hpp"
#include "dBOS_Assert.hpp"
#include "dBOS_Port.hpp"

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

dBOS_SYNCBASE::dBOS_SYNCBASE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID) :
		dBOS_BASE(ID), m_InterruptMask(ISRMASK_NONE), m_SyncID(SyncID){

}

dBOS_SYNCBASE::dBOS_SYNCBASE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, INTERRUPT_MASK_t const Mask) :
		dBOS_BASE(ID), m_InterruptMask(Mask), m_SyncID(SyncID){

}

dBOS_SYNCBASE::~dBOS_SYNCBASE(){

}

DBOS_UNIQUESYNCID_t dBOS_SYNCBASE::GetUniqueSyncID(void) const{

	return m_SyncID;
}

INTERRUPT_MASK_t dBOS_SYNCBASE::GetISRMask(void) const{

	return m_InterruptMask;
}

DBOS_RESULT_t dBOS_SYNCBASE::TakeObjectAfterWaitCheck(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask){

	return this->TakeObject(pSignalData, pCurrentTask);
}

BOOL dBOS_SYNCBASE::CheckISRDisabled(void) const{

	return DBOS_Port_CheckCurrentMaskLevel(m_InterruptMask);
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_SYNCBASE::PrintDetails(void) const{

	DBOS_Intprintf("\n  Sync ID: %u", m_SyncID);
	DBOS_Intprintf("\n  ISR Mask: %X", m_InterruptMask);
}

#endif

dBOS_SYNC::dBOS_SYNC(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType) :
		dBOS_SYNCBASE(ID, SyncID), m_pWaitList(NULLPTR), m_Priority(INVALID_PRIORITY_LEVEL){

	if(WaitListType == PRIORITYLIST){
		m_pWaitList = new dBOS_TASKPRIORITYLINKEDQUEUE();
	}
	else{
		m_pWaitList = new dBOS_TASKFIFOLINKEDQUEUE();
	}

	ASSERT_FROM_OSCS(m_pWaitList != NULLPTR)

}

dBOS_SYNC::dBOS_SYNC(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask) :
		dBOS_SYNCBASE(ID, SyncID, Mask), m_pWaitList(NULLPTR), m_Priority(INVALID_PRIORITY_LEVEL){

	if(WaitListType == PRIORITYLIST){
		m_pWaitList = new dBOS_TASKPRIORITYLINKEDQUEUE();
	}
	else{
		m_pWaitList = new dBOS_TASKFIFOLINKEDQUEUE();
	}

	ASSERT_FROM_OSCS(m_pWaitList != NULLPTR)
}

dBOS_SYNC::~dBOS_SYNC(){

}

DBOS_TASK_PRIORITY_t dBOS_SYNC::GetPriority(void) const{

	return m_Priority;
}

DBOS_RESULT_t dBOS_SYNC::RequeueTaskInObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink, DBOS_TASK_PRIORITY_t const NewPriority){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pTaskWaitLink != NULLPTR)
	ASSERT_FROM_OSCS(pTaskWaitLink->GetItem() != NULLPTR)

	Result = m_pWaitList->RequeueTaskInQueue(pTaskWaitLink, NewPriority);

	dBOS_LISTNODE<dBOS_TASK> const * const pHead = m_pWaitList->GetNextTaskFromQueue();

	ASSERT_FROM_OSCS(pHead != NULLPTR)

	DBOS_TASK_CONST_P_t const pTask = pHead->GetItem();

	if(pTaskWaitLink->GetItem() == pTask){
		// If the task is now at the front of the wait list.
		m_Priority = NewPriority;
		// The sync object inherits the new priority.
		// CurrentPriority of pTaskWaitLink->Base.pTask to be updated to NewPriority by caller.
	}

	return Result;
}

DBOS_RESULT_t dBOS_SYNC::InsertTaskIntoObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pTaskWaitLink != NULLPTR)
	ASSERT_FROM_OSCS(pTaskWaitLink->GetItem() != NULLPTR)

	m_pWaitList->AddTaskToQueue(pTaskWaitLink);

	dBOS_LISTNODE<dBOS_TASK> const * const pHead = m_pWaitList->GetNextTaskFromQueue();

	ASSERT_FROM_OSCS(pHead != NULLPTR)

	DBOS_TASK_CONST_P_t const pTask = pHead->GetItem();

	ASSERT_FROM_OSCS(pTask != NULLPTR)

	m_Priority = pTask->GetPriority(); 	// Sync Object inherits priority of highest priority task
	// which is the first task in the list.
	Result = DBOS_DONE;

	return Result;
}

BOOL dBOS_SYNC::TaskWouldBeFirstInObjectWaitList(dBOS_LISTNODE<dBOS_TASK> const * const pTaskWaitLink) const{

	BOOL Result;

	ASSERT_FROM_OSCS(pTaskWaitLink != NULLPTR)
	ASSERT_FROM_OSCS(pTaskWaitLink->GetItem() != NULLPTR)

	Result = m_pWaitList->TaskWouldBeFirstInQueue(pTaskWaitLink);

	return Result;
}

DBOS_TASK_P_t dBOS_SYNC::FromFrontRemoveTaskFromObjectWaitList(void){

	DBOS_TASK_P_t pResult;

	dBOS_LISTNODE<dBOS_TASK> const * pTaskWaitLink;

	pTaskWaitLink = m_pWaitList->DeQNextTaskFromQueue();

	if(pTaskWaitLink == NULLPTR){
		// List was already empty.
		ASSERT_FROM_OSCS(m_Priority == INVALID_PRIORITY_LEVEL)

		pResult = NULLPTR;
	}
	else{

		dBOS_LISTNODE<dBOS_TASK> const * const pHead = m_pWaitList->GetNextTaskFromQueue();

		if(pHead != NULLPTR){
			DBOS_TASK_CONST_P_t const pTask = pHead->GetItem();

			ASSERT_FROM_OSCS(pTask != NULLPTR)

			m_Priority = pTask->GetPriority();	// Sync Object inherits priority of highest priority task
		}												// which is the first task in the list.
		else{
			// List is now empty.
			m_Priority = INVALID_PRIORITY_LEVEL;
		}

		pResult = pTaskWaitLink->GetItem();

		ASSERT_FROM_OSCS(pResult != NULLPTR)
	}

	return pResult;
}

DBOS_RESULT_t dBOS_SYNC::RemoveTaskFromObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pTaskWaitLink != NULLPTR)
	ASSERT_FROM_OSCS(pTaskWaitLink->GetItem() != NULLPTR)

	m_pWaitList->RemoveTaskFromQueue(pTaskWaitLink);

	dBOS_LISTNODE<dBOS_TASK> const * const pHead = m_pWaitList->GetNextTaskFromQueue();

	if(pHead != NULLPTR){
		DBOS_TASK_CONST_P_t const pTask = pHead->GetItem();

		ASSERT_FROM_OSCS(pTask != NULLPTR)

		m_Priority = pTask->GetPriority();	// Sync Object inherits priority of highest priority task
											// which is the first task in the list.
	}
	else{
		// List is now empty.
		m_Priority = INVALID_PRIORITY_LEVEL;
	}

	Result = DBOS_DONE;

	return Result;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_SYNC::PrintDetails(void) const{

	dBOS_SYNCBASE::PrintDetails();

	if(m_Priority == INVALID_PRIORITY_LEVEL){
		DBOS_Intprintf("\n  Priority: Not Assigned");
	}
	else{
		DBOS_Intprintf("\n  Priority: %u", m_Priority);
	}

	DBOS_Intprintf("\n  Wait List: ");
	m_pWaitList->PrintQueue();

}

void dBOS_SYNC::PrintWaitList(void) const{

	m_pWaitList->PrintQueue();
}

#endif

dBOS_TASKINTERFACE_CONST_P_t dBOS_SYNC::GetTaskAtWaitListPosition(UNATIVECPU_t const Position) const{

	dBOS_TASKINTERFACE_CONST_P_t Result;

	DBOS_TASK_CONST_P_t pTask;

	pTask = m_pWaitList->GetTaskAtPosition(Position);

	if(pTask == NULLPTR){
		Result = NULLPTR;
	}
	else{
		Result = pTask->m_pInterface;
	}

	return Result;
}

dBOS_TASKQUEUE_ORDER_t dBOS_SYNC::GetWaitListOrderType(void) const{

	return m_pWaitList->GetOrderType();
}

dBOS_BASICSYNC::dBOS_BASICSYNC(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID) :
		dBOS_SYNCBASE(ID, SyncID), m_pTask(NULLPTR){

}

dBOS_BASICSYNC::dBOS_BASICSYNC(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, INTERRUPT_MASK_t const Mask) :
		dBOS_SYNCBASE(ID, SyncID, Mask), m_pTask(NULLPTR){

}

dBOS_BASICSYNC::~dBOS_BASICSYNC(){

	m_pTask = NULLPTR;
}

DBOS_TASK_PRIORITY_t dBOS_BASICSYNC::GetPriority(void) const{

	DBOS_TASK_PRIORITY_t Result;

	if(m_pTask != NULLPTR){
		Result = m_pTask->GetPriority();
	}
	else{
		Result = INVALID_PRIORITY_LEVEL;
	}

	return Result;
}

DBOS_RESULT_t dBOS_BASICSYNC::RequeueTaskInObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink, DBOS_TASK_PRIORITY_t const NewPriority){

	(void) NewPriority; // Suppress compiler unused parameter warning.

	ASSERT_FROM_OSCS(pTaskWaitLink != NULLPTR)

	DBOS_TASK_CONST_P_t const pTask = pTaskWaitLink->GetItem();

	ASSERT_FROM_OSCS(pTask == m_pTask)

	return DBOS_DONE;
}

DBOS_RESULT_t dBOS_BASICSYNC::InsertTaskIntoObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink){

	ASSERT_FROM_OSCS(pTaskWaitLink != NULLPTR)
	ASSERT_FROM_OSCS(m_pTask == NULLPTR)

	DBOS_TASK_P_t const pTask = pTaskWaitLink->GetItem();

	m_pTask = pTask;

	return DBOS_DONE;
}

BOOL dBOS_BASICSYNC::TaskWouldBeFirstInObjectWaitList(dBOS_LISTNODE<dBOS_TASK> const * const pTaskWaitLink) const{

	BOOL Result;

	ASSERT_FROM_OSCS(pTaskWaitLink != NULLPTR)
	ASSERT_FROM_OSCS(pTaskWaitLink->GetItem() != NULLPTR)

	if(m_pTask == NULLPTR){
		Result = TRUE;
	}
	else{
		Result = FALSE;
	}

	return Result;
}

DBOS_TASK_P_t dBOS_BASICSYNC::FromFrontRemoveTaskFromObjectWaitList(void){

	DBOS_TASK_P_t Result;

	Result = m_pTask;

	m_pTask = NULLPTR;

	return Result;
}

DBOS_RESULT_t dBOS_BASICSYNC::RemoveTaskFromObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink){

	ASSERT_FROM_OSCS(pTaskWaitLink != NULLPTR)

	DBOS_TASK_CONST_P_t const pTask = pTaskWaitLink->GetItem();

	ASSERT_FROM_OSCS(pTask == m_pTask)

	m_pTask = NULLPTR;

	return DBOS_DONE;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_BASICSYNC::PrintDetails(void) const{

	dBOS_SYNCBASE::PrintDetails();

	this->PrintWaitList();

	return;
}

void dBOS_BASICSYNC::PrintWaitList(void) const{

	if(m_pTask != NULLPTR){
		DBOS_Intprintf("\n  Wait List: ");
		m_pTask->PrintID();
	}
	else{
		DBOS_Intprintf("\n  Wait List: No Tasks Waiting");
	}

	return;
}

#endif

dBOS_TASKINTERFACE_CONST_P_t dBOS_BASICSYNC::GetTaskAtWaitListPosition(UNATIVECPU_t const Position) const{

	dBOS_TASKINTERFACE_CONST_P_t Result;

	if(Position == 0U){
		if(m_pTask == NULLPTR){
			Result = NULLPTR;
		}
		else{
			Result = m_pTask->m_pInterface;
		}
	}
	else{
		Result = NULLPTR;
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
