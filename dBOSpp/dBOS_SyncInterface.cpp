/**
 * @file dBOS_SyncInterface.cpp
 * @author David Broadhurst
 * @date 19/02/2015
 *
 * \brief Implementation of Interfaces to Sync Objects.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_SyncInterface.hpp"

#include "dBOS_Flag.hpp"
#include "dBOS_Mutex.hpp"
#include "dBOS_Register.hpp"
#include "dBOS_Semaphore.hpp"
#include "dBOS_Signal.hpp"
#include "dBOS_SyncTimer.hpp"

#include "dBOS_Port.hpp"
#include "dBOS_PortFunctions.hpp"

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
dBOS_SYNCINTERFACE::dBOS_SYNCINTERFACE(){

}

dBOS_SYNCINTERFACE::~dBOS_SYNCINTERFACE(){

}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_SYNCINTERFACE::PrintDetails(void) const{

	DBOS_SYNCBASE_CONST_P_t pSync;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	pSync = this->GetConstSyncObject();

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(pSync->GetISRMask());	// Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	pSync->PrintDetails();

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(pSync->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return;
}

void dBOS_SYNCINTERFACE::PrintTypeAndID(void) const{

	DBOS_SYNCBASE_CONST_P_t pSync;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	pSync = this->GetConstSyncObject();

	pSync->PrintTypeAndID();

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return;
}

void dBOS_SYNCINTERFACE::PrintWaitList(void) const{

	DBOS_SYNCBASE_CONST_P_t pSync;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	pSync = this->GetConstSyncObject();

	pSync->PrintWaitList();

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return;
}

#endif

dBOS_TASKINTERFACE_CONST_P_t dBOS_SYNCINTERFACE::GetTaskAtWaitListPosition(UNATIVECPU_t Position) const{

	dBOS_TASKINTERFACE_CONST_P_t Result;

	DBOS_SYNCBASE_CONST_P_t pSync;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	pSync = this->GetConstSyncObject();

	Result = pSync->GetTaskAtWaitListPosition(Position);

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return Result;
}

dBOS_FLAGINTERFACE::dBOS_FLAGINTERFACE(DBOS_FLAG_P_t const pImpl) :
		dBOS_SYNCINTERFACE(), m_pImpl(pImpl){

	ASSERT_FROM_OSCS(pImpl != NULLPTR)
}

dBOS_FLAGINTERFACE::~dBOS_FLAGINTERFACE(){

	delete m_pImpl;
}

UNATIVECPU_t dBOS_FLAGINTERFACE::GetFlagState(void) const{

	UNATIVECPU_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask());	// Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = m_pImpl->GetFlagState();

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return Result;
}

DBOS_SYNCBASE_CONST_P_t dBOS_FLAGINTERFACE::GetConstSyncObject(void) const{

	return m_pImpl;
}

DBOS_SYNCBASE_P_t dBOS_FLAGINTERFACE::GetSyncObject(void){

	return m_pImpl;
}

dBOS_MUTEXINTERFACE::dBOS_MUTEXINTERFACE(DBOS_MUTEX_P_t const pImpl) :
		dBOS_SYNCINTERFACE(), m_pImpl(pImpl){

	ASSERT_FROM_OSCS(pImpl != NULLPTR)
}

SNATIVECPU_t dBOS_MUTEXINTERFACE::GetCount(void) const{

	SNATIVECPU_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = m_pImpl->GetCount();

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return Result;
}

dBOS_MUTEXINTERFACE::~dBOS_MUTEXINTERFACE(){

	delete m_pImpl;
}

DBOS_SYNCBASE_CONST_P_t dBOS_MUTEXINTERFACE::GetConstSyncObject(void) const{

	return m_pImpl;
}

DBOS_SYNCBASE_P_t dBOS_MUTEXINTERFACE::GetSyncObject(void){

	return m_pImpl;
}

dBOS_REGISTERINTERFACE::dBOS_REGISTERINTERFACE(DBOS_REGISTER_P_t const pImpl) :
		dBOS_SYNCINTERFACE(), m_pImpl(pImpl){

	ASSERT_FROM_OSCS(pImpl != NULLPTR)
}

dBOS_REGISTERINTERFACE::~dBOS_REGISTERINTERFACE(){

	delete m_pImpl;
}

DBOS_RESULT_t dBOS_REGISTERINTERFACE::SetRegisterANDMask(SIGNALLED_DATA_t const Mask){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = m_pImpl->SetRegisterANDMask(Mask);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return Result;
}

DBOS_RESULT_t dBOS_REGISTERINTERFACE::SetRegisterORMask(SIGNALLED_DATA_t const Mask){

	DBOS_RESULT_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = m_pImpl->SetRegisterORMask(Mask);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return Result;
}

SIGNALLED_DATA_t dBOS_REGISTERINTERFACE::GetRegisterState(void) const{

	SIGNALLED_DATA_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = m_pImpl->GetRegisterState();

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return Result;
}

DBOS_SYNCBASE_CONST_P_t dBOS_REGISTERINTERFACE::GetConstSyncObject(void) const{

	return m_pImpl;
}

DBOS_SYNCBASE_P_t dBOS_REGISTERINTERFACE::GetSyncObject(void){

	return m_pImpl;
}

dBOS_SEMAPHOREINTERFACE::dBOS_SEMAPHOREINTERFACE(DBOS_SEMAPHORE_P_t const pImpl) :
		dBOS_SYNCINTERFACE(), m_pImpl(pImpl){

	ASSERT_FROM_OSCS(pImpl != NULLPTR)
}

dBOS_SEMAPHOREINTERFACE::~dBOS_SEMAPHOREINTERFACE(){

	delete m_pImpl;
}

SNATIVECPU_t dBOS_SEMAPHOREINTERFACE::GetCount(void) const{

	SIGNALLED_DATA_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = m_pImpl->GetCount();

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return Result;
}

DBOS_SYNCBASE_CONST_P_t dBOS_SEMAPHOREINTERFACE::GetConstSyncObject(void) const{

	return m_pImpl;
}

DBOS_SYNCBASE_P_t dBOS_SEMAPHOREINTERFACE::GetSyncObject(void){

	return m_pImpl;
}

dBOS_SIGNALINTERFACE::dBOS_SIGNALINTERFACE(DBOS_SIGNAL_P_t const pImpl) :
		dBOS_SYNCINTERFACE(), m_pImpl(pImpl){

	ASSERT_FROM_OSCS(pImpl != NULLPTR)
}

dBOS_SIGNALINTERFACE::~dBOS_SIGNALINTERFACE(){

	delete m_pImpl;
}

DBOS_SYNCBASE_CONST_P_t dBOS_SIGNALINTERFACE::GetConstSyncObject(void) const{

	return m_pImpl;
}

DBOS_SYNCBASE_P_t dBOS_SIGNALINTERFACE::GetSyncObject(void){

	return m_pImpl;
}

dBOS_TIMERINTERFACE::dBOS_TIMERINTERFACE(DBOS_TIMER_P_t const pImpl) :
		dBOS_SYNCINTERFACE(), m_pImpl(pImpl){

	ASSERT_FROM_OSCS(pImpl != NULLPTR)
}

dBOS_TIMERINTERFACE::~dBOS_TIMERINTERFACE(){

	delete m_pImpl;
}

void dBOS_TIMERINTERFACE::Pause(void){

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	m_pImpl->Pause();

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return;
}

void dBOS_TIMERINTERFACE::Run(void){

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	m_pImpl->Run();

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return;

}

void dBOS_TIMERINTERFACE::Reset(void){

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	m_pImpl->Reset();

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return;
}

void dBOS_TIMERINTERFACE::ResetAndPause(void){

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	m_pImpl->ResetAndPause();

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return;
}

void dBOS_TIMERINTERFACE::SetTime(DBOS_TIMECOUNT_t const Time){

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	m_pImpl->SetTime(Time);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return;

}

DBOS_TIMECOUNT_t dBOS_TIMERINTERFACE::GetTime(void) const{

	DBOS_TIMECOUNT_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	Result = m_pImpl->GetTime();

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return Result;
}

void dBOS_TIMERINTERFACE::Configure(INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime){

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask()); // Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	m_pImpl->Configure(Flags, ReloadTime, InitialTime);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS Critical Section

	return;
}

DBOS_SYNCBASE_CONST_P_t dBOS_TIMERINTERFACE::GetConstSyncObject(void) const{

	return m_pImpl;
}

DBOS_SYNCBASE_P_t dBOS_TIMERINTERFACE::GetSyncObject(void){

	return m_pImpl;
}

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
