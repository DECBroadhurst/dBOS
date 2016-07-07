/**
 * @file dBOS_Flag.cpp
 * @author David Broadhurst
 * @date 24/01/2015
 *
 * \brief Implementation of Flag synchronization object.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Flag.hpp"
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

dBOS_FLAG::dBOS_FLAG(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType) :
		dBOS_SYNC(ID, SyncID, WaitListType), m_SignalData(SIGNALDATANONE), m_Flag(0U){

}

dBOS_FLAG::dBOS_FLAG(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask) :
		dBOS_SYNC(ID, SyncID, WaitListType, Mask), m_SignalData(SIGNALDATANONE), m_Flag(0U){

}

dBOS_FLAG::~dBOS_FLAG(){

}

SIGNALLED_DATA_t dBOS_FLAG::GetSignalData(void) const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return m_SignalData;
}

DBOS_TASK_P_t dBOS_FLAG::GetOwnerTask(void) const{

	return NULLPTR;
}

DBOS_RESULT_t dBOS_FLAG::InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(m_Flag > 0U){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_FLAG::FROMISR_InitialWaitCheck() const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(m_Flag > 0U){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_FLAG::PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;

	return Result;
}

DBOS_RESULT_t dBOS_FLAG::GiveObject(DBOS_TASK_P_t const pRxTask){

	(void) pRxTask; // Suppress unused parameter compiler warning.

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_FLAG::TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)
	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	*pSignalData = m_SignalData;

	m_Flag = 0U;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_FLAG::FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	*pSignalData = m_SignalData;

	m_Flag = 0U;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_FLAG::SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_SignalData = SignalData;

	m_Flag = 1U;

	if(SignalMode == SIGNAL_ONE){
		Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
	}
	else{
		Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
	}

	return Result;
}

DBOS_RESULT_t dBOS_FLAG::FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_SignalData = SignalData;

	m_Flag = 1U;

	if(SignalMode == SIGNAL_ONE){
		Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
	}
	else{
		Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
	}

	return Result;
}

UNATIVECPU_t dBOS_FLAG::GetFlagState(void) const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return m_Flag;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_FLAG::PrintDetails() const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	this->PrintTypeAndID();

	DBOS_Intprintf("\n  Current Flag: %i", m_Flag);

	dBOS_SYNC::PrintDetails();

	return;
}

void dBOS_FLAG::PrintTypeAndID() const{

	DBOS_Intprintf("Flag: ");
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
