/**
 * @file dBOS_SyncTimer.cpp
 * @author David Broadhurst
 * @date 28/02/2015
 *
 * \brief Implementation of synchronization timer object.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_SyncTimer.hpp"
#include "dBOS_TimerManager.hpp"
#include "dBOS_Port.hpp"
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
dBOS_SYNCTIMER::dBOS_SYNCTIMER(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime, DBOS_WAITLIST_SORT_t const WaitListType) :
		dBOS_SYNC(ID, SyncID, WaitListType, ISRMASK_OSTIMER), m_Timer(Flags, ReloadTime, InitialTime, this){

}

dBOS_SYNCTIMER::~dBOS_SYNCTIMER(){

}

SIGNALLED_DATA_t dBOS_SYNCTIMER::GetSignalData(void) const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	return SIGNALDATANONE;
}

DBOS_TASK_P_t dBOS_SYNCTIMER::GetOwnerTask(void) const{

	return NULLPTR;
}

DBOS_RESULT_t dBOS_SYNCTIMER::InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(m_Timer.GetTime() == 0){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_SYNCTIMER::FROMISR_InitialWaitCheck() const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(m_Timer.GetTime() == 0){
		Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;
	}
	else{
		Result = DBOS_SYNC_OBJECT_WAIT_REQUIRED;
	}

	return Result;
}

DBOS_RESULT_t dBOS_SYNCTIMER::PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const{

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_SYNC_OBJECT_NO_WAIT_REQD;

	return Result;
}

DBOS_RESULT_t dBOS_SYNCTIMER::GiveObject(DBOS_TASK_P_t const pRxTask){

	(void) pRxTask; // Suppress unused parameter compiler warning.

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_SYNCTIMER::TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)
	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	*pSignalData = SIGNALDATANONE;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_SYNCTIMER::FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData){

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pSignalData != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	*pSignalData = SIGNALDATANONE;

	Result = DBOS_DONE;

	return Result;
}

DBOS_RESULT_t dBOS_SYNCTIMER::SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask){

	(void) SignalData; // Suppress unused parameter compiler warning.
	(void) SignalMode;

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pCurrentTask != NULLPTR)

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(SignalMode == SIGNAL_ONE){
		Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
	}
	else{
		Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
	}

	return Result;
}

DBOS_RESULT_t dBOS_SYNCTIMER::FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode){

	(void) SignalData; // Suppress unused parameter compiler warning.
	(void) SignalMode;

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	if(SignalMode == SIGNAL_ONE){
		Result = DBOS_OBJECT_SIGNALLED_SIGNALONETASK;
	}
	else{
		Result = DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS;
	}

	return Result;
}

void dBOS_SYNCTIMER::Pause(void){

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_Timer.Pause();

	return;
}

void dBOS_SYNCTIMER::Run(void){

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_Timer.Run();

	return;
}

void dBOS_SYNCTIMER::Reset(void){

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_Timer.Reset();

	return;
}

void dBOS_SYNCTIMER::ResetAndPause(void){

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_Timer.ResetAndPause();

	return;
}

void dBOS_SYNCTIMER::SetTime(DBOS_TIMECOUNT_t const Time){

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_Timer.SetTime(Time);

	return;
}

DBOS_TIMECOUNT_t dBOS_SYNCTIMER::GetTime(void) const{

	DBOS_TIMECOUNT_t Result;

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	Result = m_Timer.GetTime();

	return Result;
}

void dBOS_SYNCTIMER::Configure(INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime){

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	m_Timer.Configure(Flags, ReloadTime, InitialTime);

	return;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
void dBOS_SYNCTIMER::PrintDetails(void) const{

	ASSERT_FROM_OSCS(dBOS_SYNC::CheckISRDisabled() == TRUE)

	this->PrintTypeAndID();

	m_Timer.PrintDetails();

	dBOS_SYNC::PrintDetails();

	return;
}

void dBOS_SYNCTIMER::PrintTypeAndID(void) const{

	DBOS_Intprintf("Sync Timer: ");
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
