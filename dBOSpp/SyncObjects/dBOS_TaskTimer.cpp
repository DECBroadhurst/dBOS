/**
 * @file dBOS_TaskTimer.cpp
 * @author David Broadhurst
 * @date 12/04/2015
 *
 * \brief Implementation of task timer synchronization object.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_TaskTimer.hpp"
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
dBOS_TASKTIMER::dBOS_TASKTIMER(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID) :
		dBOS_BASICSYNC(ID, SyncID, ISRMASK_OSTIMER), m_Timer(this){

}

dBOS_TASKTIMER::~dBOS_TASKTIMER(){

}

SIGNALLED_DATA_t dBOS_TASKTIMER::GetSignalData(void) const{

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	return SIGNALDATANONE;
}

DBOS_TASK_P_t dBOS_TASKTIMER::GetOwnerTask(void) const{

	return NULLPTR;
}

DBOS_RESULT_t dBOS_TASKTIMER::InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	if(m_Timer.GetTime() == 0){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_TASKTIMER::FROMISR_InitialWaitCheck() const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	if(m_Timer.GetTime() == 0){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_TASKTIMER::PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;

	return Result;
}

DBOS_RESULT_t dBOS_TASKTIMER::GiveObject(DBOS_TASK_P_t const pRxTask){

	(void) pRxTask; // Suppress unused parameter compiler warning.

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_TASKTIMER::TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)
	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	*pSignalData = SIGNALDATANONE;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_TASKTIMER::FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	*pSignalData = SIGNALDATANONE;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_TASKTIMER::SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask){

	(void) SignalData; // Suppress unused parameter compiler warning.
	(void) SignalMode;

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;

	return Result;
}

DBOS_RESULT_t dBOS_TASKTIMER::FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode){

	(void) SignalData; // Suppress unused parameter compiler warning.
	(void) SignalMode;

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;

	return Result;
}

void dBOS_TASKTIMER::Pause(void){

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	m_Timer.Pause();

	return;
}

void dBOS_TASKTIMER::Run(void){

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	m_Timer.Run();

	return;
}

void dBOS_TASKTIMER::Reset(void){

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	m_Timer.Reset();

	return;
}

void dBOS_TASKTIMER::ResetAndPause(void){

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	m_Timer.ResetAndPause();

	return;
}

void dBOS_TASKTIMER::SetTime(DBOS_TIMECOUNT_t const Time){

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	m_Timer.SetTime(Time);

	return;
}

DBOS_TIMECOUNT_t dBOS_TASKTIMER::GetTime(void) const{

	DBOS_TIMECOUNT_t Result;

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	Result = m_Timer.GetTime();

	return Result;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_TASKTIMER::PrintDetails(void) const{

	ASSERT_FROM_OSCS(dBOS_BASICSYNC::CheckISRDisabled() == TRUE)

	this->PrintTypeAndID();

	m_Timer.PrintDetails();

	dBOS_BASICSYNC::PrintDetails();
}

void dBOS_TASKTIMER::PrintTypeAndID(void) const{

	DBOS_Intprintf("Task Timer: ");
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
