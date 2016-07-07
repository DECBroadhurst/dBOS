/**
 * @file dBOS_Register.cpp
 * @author David Broadhurst
 * @date 24/01/2015
 *
 * \brief Implementation of register synchronization object.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Register.hpp"
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

dBOS_REGISTER::dBOS_REGISTER(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType) :
		dBOS_SYNC(ID, SyncID, WaitListType), m_SignalData(SIGNALDATANONE), m_Reg(0U), m_ORMask(0U), m_ANDMask(0U){

}

dBOS_REGISTER::dBOS_REGISTER(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask) :
		dBOS_SYNC(ID, SyncID, WaitListType, Mask), m_SignalData(SIGNALDATANONE), m_Reg(0U), m_ORMask(0U), m_ANDMask(0U){

}

dBOS_REGISTER::~dBOS_REGISTER(){

}

SIGNALLED_DATA_t dBOS_REGISTER::GetSignalData(void) const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return m_SignalData;
}

DBOS_TASK_P_t dBOS_REGISTER::GetOwnerTask(void) const{

	return NULLPTR;
}

DBOS_RESULT_t dBOS_REGISTER::InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(((m_Reg & m_ORMask) != 0U) || (((m_Reg & m_ANDMask) == m_ANDMask) && (m_ANDMask != 0U))){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_REGISTER::FROMISR_InitialWaitCheck() const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(((m_Reg & m_ORMask) != 0U) || (((m_Reg & m_ANDMask) == m_ANDMask) && (m_ANDMask != 0U))){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_REGISTER::PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;

	return Result;
}

DBOS_RESULT_t dBOS_REGISTER::GiveObject(DBOS_TASK_P_t const pRxTask){

	(void) pRxTask; // Suppress unused parameter compiler warning.

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_REGISTER::TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)
	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	*pSignalData = m_SignalData;

	m_Reg = 0U;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_REGISTER::FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	*pSignalData = m_SignalData;

	m_Reg = 0U;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_REGISTER::SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_Reg |= SignalData;

	if(((m_Reg & m_ORMask) != 0U) || (((m_Reg & m_ANDMask) == m_ANDMask) && (m_ANDMask != 0U))){

		m_SignalData = m_Reg;	// Update SignalData when conditions met.

		if(SignalMode == SIGNAL_ONE){
			Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
		}
		else{
			Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
		}
	}
	else{
		Result = DBOS_OBJECT_SIGNAL_NO_TASKS;
	}

	return Result;
}

DBOS_RESULT_t dBOS_REGISTER::FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_Reg |= SignalData;

	if(((m_Reg & m_ORMask) != 0U) || (((m_Reg & m_ANDMask) == m_ANDMask) && (m_ANDMask != 0U))){

		m_SignalData = m_Reg;	// Update SignalData when conditions met.

		if(SignalMode == SIGNAL_ONE){
			Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
		}
		else{
			Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
		}
	}
	else{
		Result = DBOS_OBJECT_SIGNAL_NO_TASKS;
	}

	return Result;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_REGISTER::PrintDetails() const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	this->PrintTypeAndID();

	DBOS_Intprintf("\n  Current Register: %X", m_Reg);
	DBOS_Intprintf("\n  AND Mask: %X", m_ANDMask);
	DBOS_Intprintf("\n  OR Mask: %X", m_ORMask);

	dBOS_SYNC::PrintDetails();

	return;
}

void dBOS_REGISTER::PrintTypeAndID() const{

	DBOS_Intprintf("Register: ");
	this->PrintID();

	return;
}

#endif

DBOS_RESULT_t dBOS_REGISTER::SetRegisterANDMask(SIGNALLED_DATA_t const Mask){

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_ANDMask = Mask;

	return DBOS_DONE;
}

DBOS_RESULT_t dBOS_REGISTER::SetRegisterORMask(SIGNALLED_DATA_t const Mask){

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_ORMask = Mask;

	return DBOS_DONE;
}

SIGNALLED_DATA_t dBOS_REGISTER::GetRegisterState(void) const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return m_Reg;
}
// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
