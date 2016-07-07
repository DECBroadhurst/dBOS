/**
 * @file dBOS_Timers.cpp
 * @author David Broadhurst
 * @date 27/03/2015
 *
 * \brief Implementation of dBOS Timers.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Timers.hpp"
#include "dBOS_TimerManager.hpp"
#include "dBOS_Port.hpp"
#include "dBOS_PortFunctions.hpp"
#include "dBOS_Kernel.hpp"
#include "dBOS_Sync.hpp"

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
dBOS_TIMERMANAGER * dBOS_TIMERBASE::m_pTimerManager = NULLPTR;
dBOS_KERNEL * dBOS_TIMERBASE::m_pK = NULLPTR;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
dBOS_TIMERBASE::dBOS_TIMERBASE(){

	m_pTimerManager = dBOS_TIMERMANAGER::GetInstance();

	m_pK = dBOS_KERNEL::GetInstance();

	ASSERT_FROM_OSCS(m_pTimerManager != NULLPTR)
	ASSERT_FROM_OSCS(m_pK != NULLPTR)

}

dBOS_TIMERBASE::~dBOS_TIMERBASE(){

}

dBOS_TIMERCOUNTDOWN::dBOS_TIMERCOUNTDOWN(DBOS_SYNCBASE_P_t const pSyncObject) :
		dBOS_TIMERBASE(), m_Node(this), m_State(TIMERSTOPPED), m_CountTime(0), m_pSyncObject(pSyncObject){

}

dBOS_TIMERCOUNTDOWN::~dBOS_TIMERCOUNTDOWN(){

}

void dBOS_TIMERCOUNTDOWN::RunFromISR(void){

	if(m_CountTime > 0){
		m_CountTime--;
	}

	if(m_CountTime == 0){

		if(m_pSyncObject != NULLPTR){
			// Signal object assigned to this timer.

			DBOS_RESULT_t SignalResult;

			INTERRUPT_MASK_t PrevISRMask;
			INTERRUPT_MASK_t ISRMask;
			INTERRUPT_MASK_t PrevCSMask;

			PrevCSMask = DBOS_Port_EnterCritical_FromISR();	// Enter OS critical section

			ISRMask = m_pSyncObject->GetISRMask();

			PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);		// Enter ISR critical section, not strictly required if
																				// only one ISR accesses this object.

			ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
			// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

			SignalResult = m_pSyncObject->FROMISR_SignalObject(SIGNALDATANONE, SIGNAL_AUTO);

			INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
			ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
			// Check ISR Mask Was Still Raised

			switch(SignalResult){
			case DBOS_OBJECT_SIGNALLED_SIGNALONETASK:
				m_pK->SetISRSignaledFlag(m_pSyncObject->GetUniqueSyncID());
				break;
			case DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS:
				m_pK->SetISRBroadcastFlag(m_pSyncObject->GetUniqueSyncID());
				break;
			case DBOS_OBJECT_SIGNAL_NO_TASKS:

				break;
			case DBOS_SYNC_OBJECT_INVALID_OBJECT_TYPE:

				break;
			default:
				ASSERT_ERROR_FROM_OSCS()
				break;
			}

			DBOS_Port_ExitCritical_FromISR(PrevCSMask);		// Exit OS critical section

		}

		// Stop Timer.
		if(m_State == TIMERRUNNING){
			m_pTimerManager->RemoveTimer(&m_Node);
			m_State = TIMERSTOPPED;
		}

	}

	return;
}

void dBOS_TIMERCOUNTDOWN::Pause(void){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	if(m_State == TIMERRUNNING){
		m_pTimerManager->RemoveTimer(&m_Node);
		m_State = TIMERSTOPPED;
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

void dBOS_TIMERCOUNTDOWN::Run(void){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	if(m_State == TIMERSTOPPED){
		m_pTimerManager->AddTimer(&m_Node);
		m_State = TIMERRUNNING;
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

void dBOS_TIMERCOUNTDOWN::Reset(void){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	m_CountTime = 0U;

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

void dBOS_TIMERCOUNTDOWN::ResetAndPause(void){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	m_CountTime = 0U;

	if(m_State == TIMERRUNNING){
		m_pTimerManager->RemoveTimer(&m_Node);
		m_State = TIMERSTOPPED;
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

void dBOS_TIMERCOUNTDOWN::SetTime(DBOS_TIMECOUNT_t const Time){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	m_CountTime = Time;

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

DBOS_TIMECOUNT_t dBOS_TIMERCOUNTDOWN::GetTime(void) const{

	DBOS_TIMECOUNT_t Result;

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	Result = m_CountTime;

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return Result;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_TIMERCOUNTDOWN::PrintDetails(void) const{

	DBOS_Intprintf("\n  Time: %i", m_CountTime);

	switch(m_State){
	case TIMERRUNNING:
		DBOS_Intprintf("\n  State: Running");
		break;
	case TIMERSTOPPED:
		DBOS_Intprintf("\n  State: Stopped");
		break;
	default:
		break;
	}

	DBOS_Intprintf("\n  Sync Object: ");
	if(m_pSyncObject != NULLPTR){
		m_pSyncObject->PrintTypeAndID();
	}
	else{
		DBOS_Intprintf("NONE");
	}

	return;
}

#endif

dBOS_TIMERFULLFEATURE::dBOS_TIMERFULLFEATURE(INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime, DBOS_SYNCBASE_P_t const pSyncObject) :
		dBOS_TIMERBASE(), m_Node(this), m_State(TIMERSTOPPED), m_CountTime(InitialTime), m_ReloadTime(ReloadTime), m_Flags(Flags), m_pSyncObject(pSyncObject){

}

dBOS_TIMERFULLFEATURE::~dBOS_TIMERFULLFEATURE(){

}

void dBOS_TIMERFULLFEATURE::RunFromISR(void){

	if((m_Flags & TIMER_COUNTDOWN_FLAG) == TIMER_COUNTDOWN_FLAG){

		if((m_CountTime > 0) || ((m_Flags & TIMER_ALLOW_NEGATIVE) == TIMER_ALLOW_NEGATIVE)){
			m_CountTime--;
		}

		if(m_CountTime == 0){

			if(m_pSyncObject != NULLPTR){
				// Signal object assigned to this timer.

				DBOS_RESULT_t SignalResult;

				INTERRUPT_MASK_t PrevISRMask;
				INTERRUPT_MASK_t ISRMask;
				INTERRUPT_MASK_t PrevCSMask;

				PrevCSMask = DBOS_Port_EnterCritical_FromISR();	// Enter OS critical section

				ISRMask = m_pSyncObject->GetISRMask();

				PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMask);		// Enter ISR critical section, not strictly required if
																					// only one ISR accesses this object.

				ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
				// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

				SignalResult = m_pSyncObject->FROMISR_SignalObject(SIGNALDATANONE, SIGNAL_AUTO);

				INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
				ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMask)) == TRUE)
				// Check ISR Mask Was Still Raised

				switch(SignalResult){
				case DBOS_OBJECT_SIGNALLED_SIGNALONETASK:
					m_pK->SetISRSignaledFlag(m_pSyncObject->GetUniqueSyncID());
					break;
				case DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS:
					m_pK->SetISRBroadcastFlag(m_pSyncObject->GetUniqueSyncID());
					break;
				case DBOS_OBJECT_SIGNAL_NO_TASKS:

					break;
				case DBOS_SYNC_OBJECT_INVALID_OBJECT_TYPE:

					break;
				default:
					ASSERT_ERROR_FROM_OSCS()
					break;
				}

				DBOS_Port_ExitCritical_FromISR(PrevCSMask);		// Exit OS critical section

			}

			if((m_Flags & TIMER_RELOAD_FLAG) == TIMER_RELOAD_FLAG){
				m_CountTime = m_ReloadTime;
			}
			else if((m_Flags & TIMER_ALLOW_NEGATIVE) == TIMER_ALLOW_NEGATIVE){

			}
			else{
				// Stop Timer.
				if(m_State == TIMERRUNNING){
					m_pTimerManager->RemoveTimer(&m_Node);
					m_State = TIMERSTOPPED;
				}
			}

		}

	}
	else if((m_Flags & TIMER_COUNTUP_FLAG) == TIMER_COUNTUP_FLAG){
		m_CountTime++;
	}
	else{
		// Stop Timer.
		if(m_State == TIMERRUNNING){
			m_pTimerManager->RemoveTimer(&m_Node);
			m_State = TIMERSTOPPED;
		}
	}

	return;
}

void dBOS_TIMERFULLFEATURE::Pause(void){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	if(m_State == TIMERRUNNING){
		m_pTimerManager->RemoveTimer(&m_Node);
		m_State = TIMERSTOPPED;
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

void dBOS_TIMERFULLFEATURE::Run(void){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	if(m_State == TIMERSTOPPED){
		m_pTimerManager->AddTimer(&m_Node);
		m_State = TIMERRUNNING;
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

void dBOS_TIMERFULLFEATURE::Reset(void){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	m_CountTime = m_ReloadTime;

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

void dBOS_TIMERFULLFEATURE::ResetAndPause(void){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	m_CountTime = m_ReloadTime;

	if(m_State == TIMERRUNNING){
		m_pTimerManager->RemoveTimer(&m_Node);
		m_State = TIMERSTOPPED;
	}

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

void dBOS_TIMERFULLFEATURE::SetTime(DBOS_TIMECOUNT_t const Time){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	m_CountTime = Time;

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

DBOS_TIMECOUNT_t dBOS_TIMERFULLFEATURE::GetTime(void) const{

	DBOS_TIMECOUNT_t Result;

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	Result = m_CountTime;

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return Result;
}

void dBOS_TIMERFULLFEATURE::Configure(INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	m_Flags = Flags;
	m_ReloadTime = ReloadTime;
	m_CountTime = InitialTime;

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_TIMERFULLFEATURE::PrintDetails(void) const{

	UNATIVECPU_t Count = 0U;

	DBOS_Intprintf("\n  Time: %i", m_CountTime);
	DBOS_Intprintf("\n  Reload Time: %i", m_ReloadTime);

	switch(m_State){
	case TIMERRUNNING:
		DBOS_Intprintf("\n  State: Running");
		break;
	case TIMERSTOPPED:
		DBOS_Intprintf("\n  State: Stopped");
		break;
	default:
		break;
	}

	DBOS_Intprintf("\n  Flags: ");

	if((m_Flags & TIMER_COUNTUP_FLAG) == TIMER_COUNTUP_FLAG){
		DBOS_Intprintf("Count Up");
		Count++;
	}

	if((m_Flags & TIMER_COUNTDOWN_FLAG) == TIMER_COUNTDOWN_FLAG){
		if(Count > 0U){
			DBOS_Intprintf(", ");
		}

		DBOS_Intprintf("Count Down");
		Count++;
	}

	if((m_Flags & TIMER_RELOAD_FLAG) == TIMER_RELOAD_FLAG){
		if(Count > 0U){
			DBOS_Intprintf(", ");
		}

		DBOS_Intprintf("Reload");
		Count++;
	}

	if((m_Flags & TIMER_ALLOW_NEGATIVE) == TIMER_ALLOW_NEGATIVE){
		if(Count > 0U){
			DBOS_Intprintf(", ");
		}

		DBOS_Intprintf("Allow Negative");
	}

	DBOS_Intprintf("\n  Sync Object: ");
	if(m_pSyncObject != NULLPTR){
		m_pSyncObject->PrintTypeAndID();
	}
	else{
		DBOS_Intprintf("NONE");
	}

	return;
}

#endif
// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
