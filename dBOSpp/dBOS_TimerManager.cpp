/**
 * @file dBOS_TimerManager.cpp
 * @author David Broadhurst
 * @date 2/03/2015
 *
 * \brief Implementation of dBOS Timer Manager.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_TimerManager.hpp"
#include "dBOS_Timers.hpp"
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
dBOS_TIMERMANAGER * dBOS_TIMERMANAGER::m_pInstance = NULLPTR;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
DBOS_RESULT_t dBOS_TIMERMANAGER::Init(void){ // To be called before OS starts.

	DBOS_RESULT_t Result;

	m_pInstance = new dBOS_TIMERMANAGER();

	if(m_pInstance == NULLPTR){
		Result = DBOS_OUT_OF_MEMORY;
	}
	else{
		Result = DBOS_DONE;
	}

	return Result;
}

dBOS_TIMERMANAGER * dBOS_TIMERMANAGER::GetInstance(void){

	ASSERT_FROM_OSCS(m_pInstance != NULLPTR)

	return m_pInstance;
}

void dBOS_TIMERMANAGER::AddTimer(dBOS_LISTNODE<dBOS_TIMERBASE> * const pNode){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER); // Enter ISR critical section

	m_TimerList.AddItemToFront(pNode);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

void dBOS_TIMERMANAGER::RemoveTimer(dBOS_LISTNODE<dBOS_TIMERBASE> * const pNode){

	INTERRUPT_MASK_t PrevISRMask;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	m_TimerList.RemoveItem(pNode);

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return;
}

void dBOS_TIMERMANAGER::ISR(void){
	// CALLED FROM TIMER ISR

	dBOS_LISTNODE<dBOS_TIMERBASE> const * pCurrent;

	dBOS_LISTNODE<dBOS_TIMERBASE> const * pNext;

	m_EllpasedTime++;

	pCurrent = m_TimerList.GetHead();

	while(pCurrent != NULLPTR){

		pNext = m_TimerList.GetNext(pCurrent);
		// Get next timer here as call to RunFromISR below  might remove the current timer from the list.

		pCurrent->GetItem()->RunFromISR();

		pCurrent = pNext;
	}

	return;
}

DBOS_TIMECOUNT_t dBOS_TIMERMANAGER::GetEllapsedTime(void) const{

	INTERRUPT_MASK_t PrevISRMask;

	DBOS_TIMECOUNT_t Result;

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSTIMER);	// Enter ISR critical section

	Result = m_EllpasedTime;

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(ISRMASK_OSTIMER)) == TRUE)
	// Check ISR Mask Was Still Raised

	return Result;
}

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
dBOS_TIMERMANAGER::dBOS_TIMERMANAGER() :
		m_EllpasedTime(0), m_TimerList(){

}

dBOS_TIMERMANAGER::~dBOS_TIMERMANAGER(){

}

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
