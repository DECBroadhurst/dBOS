/**
 * @file dBOS_Semaphore.cpp
 * @author David Broadhurst
 * @date 24/01/2015
 *
 * \brief Implementation of semaphore synchronization object.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Semaphore.hpp"
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
dBOS_SEMAPHORE::dBOS_SEMAPHORE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, SNATIVECPU_t const InitialCount, SNATIVECPU_t const MaxCount, DBOS_WAITLIST_SORT_t const WaitListType) :
		dBOS_SYNC(ID, SyncID, WaitListType), m_Count(InitialCount), m_MaxCount(MaxCount), m_SignalData(SIGNALDATANONE){

}

dBOS_SEMAPHORE::dBOS_SEMAPHORE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, SNATIVECPU_t const InitialCount, SNATIVECPU_t const MaxCount, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask) :
		dBOS_SYNC(ID, SyncID, WaitListType, Mask), m_Count(InitialCount), m_MaxCount(MaxCount), m_SignalData(SIGNALDATANONE){

}

dBOS_SEMAPHORE::~dBOS_SEMAPHORE(){

}

SIGNALLED_DATA_t dBOS_SEMAPHORE::GetSignalData(void) const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return m_SignalData;
}

DBOS_TASK_P_t dBOS_SEMAPHORE::GetOwnerTask(void) const{

	return NULLPTR;
}

DBOS_RESULT_t dBOS_SEMAPHORE::InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(m_Count > 0){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_SEMAPHORE::FROMISR_InitialWaitCheck() const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(m_Count > 0){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_SEMAPHORE::PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(m_Count > 0){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_SEMAPHORE::GiveObject(DBOS_TASK_P_t const pRxTask){

	(void) pRxTask; // Suppress unused parameter compiler warning.

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_SEMAPHORE::TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)
	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	ASSERT_FROM_OSCS(m_Count > 0)

	if((this->GetPriority() != INVALID_PRIORITY_LEVEL) && (this->GetWaitListOrderType() == dBOS_TASKQUEUE_ORDER_PRIORITY)){
		ASSERT_FROM_OSCS(pCurrentTask->GetPriority() >= this->GetPriority())
	}

	m_Count--;

	*pSignalData = m_SignalData;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_SEMAPHORE::FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	ASSERT_FROM_OSCS(m_Count > 0)

	m_Count--;

	*pSignalData = m_SignalData;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_SEMAPHORE::SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_SignalData = SignalData;

	if(m_Count < m_MaxCount){
		m_Count++;

		if(SignalMode == SIGNAL_AUTO){
			if(m_Count == 1){
				Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
			}
			else{
				Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
			}
		}
		else if(SignalMode == SIGNAL_ALL){
			Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
		}
		else{
			Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
		}
	}
	else{
		if(SignalMode == SIGNAL_ONE){
			Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
		}
		else{
			Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
		}
	}

	return Result;
}

DBOS_RESULT_t dBOS_SEMAPHORE::FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_SignalData = SignalData;

	if(m_Count < m_MaxCount){
		m_Count++;

		if(SignalMode == SIGNAL_AUTO){
			if(m_Count == 1){
				Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
			}
			else{
				Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
			}
		}
		else if(SignalMode == SIGNAL_ALL){
			Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
		}
		else{
			Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
		}
	}
	else{
		if(SignalMode == SIGNAL_ONE){
			Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
		}
		else{
			Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
		}
	}

	return Result;
}

SNATIVECPU_t dBOS_SEMAPHORE::GetCount(void) const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return m_Count;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_SEMAPHORE::PrintDetails() const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	this->PrintTypeAndID();

	DBOS_Intprintf("\n  Current Count: %i", m_Count);
	DBOS_Intprintf("\n  Maximum Count: %i", m_MaxCount);

	dBOS_SYNC::PrintDetails();

	return;
}

void dBOS_SEMAPHORE::PrintTypeAndID() const{

	DBOS_Intprintf("Semaphore: ");
	this->PrintID();

	return;
}

#endif

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
