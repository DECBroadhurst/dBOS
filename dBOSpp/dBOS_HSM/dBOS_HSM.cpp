/**
 * @file dBOS_HSM.cpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Implementation of dBOS Hierarchical State Machine.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSM.hpp"
#include "dBOS_DebugPrint.hpp"
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
dBOS_HSM::dBOS_HSM(char const * const pName, dBOS_HSM_DEBUG_MODE_t const DebugMode) :
		m_pStates(NULLPTR), m_pExtended(NULLPTR), m_pCurrentState(NULLPTR), m_pName(pName), m_DebugMode(DebugMode){

}

dBOS_HSM::~dBOS_HSM(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSM::ProcessEvent(pdBOS_HSMEVENT_CONST_t const pEvent){

	DBOSASSERT(pEvent != NULLPTR)

	DBOSASSERT(pEvent->m_pProp->m_ID != INVALIDEVENT)

	pDBOS_HSMSTATE_CONST_t pNextState = NULLPTR;

	pDBOS_HSMSTATE_CONST_t pCurrentHandlerState;

	dBOS_HSM_HANDLER_RETURN_t HandlerResult;

	pCurrentHandlerState = m_pCurrentState;

	HandlerResult = pCurrentHandlerState->Event(m_pStates, m_pExtended, pEvent, &pNextState);

	if((HandlerResult == dBOS_HSM_EVENT_NOTHANDLED) || (HandlerResult == dBOS_HSM_EVENT_HANDLED_PASS_TO_PARENT)){

		if(HandlerResult == dBOS_HSM_EVENT_HANDLED_PASS_TO_PARENT){
			if(m_DebugMode == HSM_DEBUG_ALL_EVENTS){
				DBOS_printf("\nHSM: %s Current State: %s Process Event: %s", m_pName, m_pCurrentState->m_pName, pEvent->m_pProp->m_pName);
				DBOS_printf(" Handled By State: %s Passing Event to Parent State.", pCurrentHandlerState->m_pName);
			}
		}

		pCurrentHandlerState = pCurrentHandlerState->m_pParent;
	}

	while(((HandlerResult == dBOS_HSM_EVENT_NOTHANDLED) || (HandlerResult == dBOS_HSM_EVENT_HANDLED_PASS_TO_PARENT)) && (pCurrentHandlerState != NULLPTR)){

		HandlerResult = pCurrentHandlerState->Event(m_pStates, m_pExtended, pEvent, &pNextState);

		if((HandlerResult == dBOS_HSM_EVENT_NOTHANDLED) || (HandlerResult == dBOS_HSM_EVENT_HANDLED_PASS_TO_PARENT)){

			if(HandlerResult == dBOS_HSM_EVENT_HANDLED_PASS_TO_PARENT){
				if(m_DebugMode == HSM_DEBUG_ALL_EVENTS){
					DBOS_printf("\nHSM: %s Current State: %s Process Event: %s", m_pName, m_pCurrentState->m_pName, pEvent->m_pProp->m_pName);
					DBOS_printf(" Handled By State: %s Passing Event to Parent State.", pCurrentHandlerState->m_pName);
				}
			}

			pCurrentHandlerState = pCurrentHandlerState->m_pParent;
		}
	}

	switch(HandlerResult){
	case dBOS_HSM_EVENT_HANDLED:

		if(m_DebugMode == HSM_DEBUG_ALL_EVENTS){
			DBOS_printf("\nHSM: %s Current State: %s Process Event: %s", m_pName, m_pCurrentState->m_pName, pEvent->m_pProp->m_pName);
			DBOS_printf(" Handled By State: %s", pCurrentHandlerState->m_pName);
		}

		break;
	case dBOS_HSM_EVENT_NOTHANDLED:
	case dBOS_HSM_EVENT_HANDLED_PASS_TO_PARENT: // deliberate fall through.

		DBOSASSERT(pCurrentHandlerState == NULLPTR)

		DBOS_printf("\nHSM: %s Current State: %s Process Event: %s EVENT NOT HANDLED", m_pName, m_pCurrentState->m_pName, pEvent->m_pProp->m_pName);

		break;
	case dBOS_HSM_TRANSISTION:
		DBOSASSERT(pNextState != NULLPTR)

		if(pNextState != m_pCurrentState){

			while(pNextState != NULLPTR){
				if((m_DebugMode == HSM_DEBUG_TRANSISTIONS_ONLY) || (m_DebugMode == HSM_DEBUG_ALL_EVENTS)){
					DBOS_printf("\nHSM: %s Current State: %s Process Event: %s", m_pName, m_pCurrentState->m_pName, pEvent->m_pProp->m_pName);
					DBOS_printf(" Handled By State: %s TRANSISTION TO STATE: %s,", pCurrentHandlerState->m_pName, pNextState->m_pName);
				}

				pNextState = Transistion(m_pCurrentState, pNextState);
			}
		}

		break;
	default:
		DBOSASSERTERROR()
		break;
	}

	return HandlerResult;
}

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS
void dBOS_HSM::Start(pDBOS_HSMSTATE_CONST_t const pInitialState){

	DBOSASSERT(m_pStates != NULLPTR)
	DBOSASSERT(m_pStates->pTopState != NULLPTR)
	DBOSASSERT(pInitialState != NULLPTR)

	pDBOS_HSMSTATE_CONST_t pNextState = NULLPTR;

	if((m_DebugMode == HSM_DEBUG_TRANSISTIONS_ONLY) || (m_DebugMode == HSM_DEBUG_ALL_EVENTS)){
		DBOS_printf("\nHSM: Startup: %s", m_pName);
	}

	m_pCurrentState = m_pStates->pTopState;

	if((m_DebugMode == HSM_DEBUG_TRANSISTIONS_ONLY) || (m_DebugMode == HSM_DEBUG_ALL_EVENTS)){
		DBOS_printf("\n ");
		DBOS_printf("State Entry: %s", m_pStates->pTopState->m_pName);
	}

	m_pStates->pTopState->Entry(m_pExtended);

	pNextState = pInitialState;

	while(pNextState != NULLPTR){
		pNextState = Transistion(m_pCurrentState, pNextState);
	}

	return;
}

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
/**
 * \brief Performs a state transition from Source state to Destination state making state exit, entry and check entry calls.
 *
 * @param[in] pSource Pointer to source state.
 * @param[in] pDest Pointer to destination state.
 * @retval NULLPTR Transition complete.
 * @retval pNextState Pointer to state to transition to (if another transition is required).
 */
pDBOS_HSMSTATE_CONST_t dBOS_HSM::Transistion(pDBOS_HSMSTATE_CONST_t const pSource, pDBOS_HSMSTATE_CONST_t const pDest){

	dBOS_HSM_HANDLER_RETURN_t EntryChecksResult;

	pDBOS_HSMSTATE_CONST_t pIntSourceState = pSource;

	pDBOS_HSMSTATE_CONST_t pIntDestinationState = pDest;

	pDBOS_HSMSTATE_CONST_t pNextState = NULLPTR;

	DBOSASSERT(pIntSourceState != NULLPTR)
	DBOSASSERT(pIntDestinationState != NULLPTR)

	const_cast<pDBOS_HSMSTATE_t>(pIntDestinationState)->m_pDestNext = NULLPTR; // Cast allow write.

	while(pIntSourceState->m_Level > pIntDestinationState->m_Level){

		if((m_DebugMode == HSM_DEBUG_TRANSISTIONS_ONLY) || (m_DebugMode == HSM_DEBUG_ALL_EVENTS)){
			DBOS_printf("\n ");

			UNATIVECPU_t i;
			for(i = pIntSourceState->m_Level; i > 0U ; i--){
				DBOS_printf("  ");
			}

			DBOS_printf("State Exit: %s", pIntSourceState->m_pName);
		}

		DBOSASSERT(m_pCurrentState == pIntSourceState)
		// Assert pCurrentState variable is correct, the current state is the one we are about to exit.
		pIntSourceState->Exit(m_pExtended);
		pIntSourceState = pIntSourceState->m_pParent;

		DBOSASSERT(pIntSourceState != NULLPTR)

		m_pCurrentState = pIntSourceState; // After state exit the current state is the parent state of the state just exited.
	}

	while(pIntDestinationState->m_Level > pIntSourceState->m_Level){

		DBOSASSERT(pIntDestinationState->m_pParent != NULLPTR)

		const_cast<pDBOS_HSMSTATE_t>(pIntDestinationState->m_pParent)->m_pDestNext = pIntDestinationState; // This saves the path required for entry actions in pDestNext.  // Cast allow write.
		pIntDestinationState = pIntDestinationState->m_pParent;
	}

	DBOSASSERT(pIntDestinationState->m_Level == pIntSourceState->m_Level)

	while(pIntDestinationState != pIntSourceState){

		if((m_DebugMode == HSM_DEBUG_TRANSISTIONS_ONLY) || (m_DebugMode == HSM_DEBUG_ALL_EVENTS)){
			DBOS_printf("\n ");

			UNATIVECPU_t i;
			for(i = pIntSourceState->m_Level; i > 0U ; i--){
				DBOS_printf("  ");
			}

			DBOS_printf("State Exit: %s", pIntSourceState->m_pName);
		}

		DBOSASSERT(m_pCurrentState == pIntSourceState)
		// Assert pCurrentState variable is correct, the current state is the one we are about to exit.
		pIntSourceState->Exit(m_pExtended);
		pIntSourceState = pIntSourceState->m_pParent;

		DBOSASSERT(pIntSourceState != NULLPTR)

		m_pCurrentState = pIntSourceState; // After state exit the current state is the parent state of the state just exited.

		DBOSASSERT(pIntDestinationState->m_pParent != NULLPTR)

		const_cast<pDBOS_HSMSTATE_t>(pIntDestinationState->m_pParent)->m_pDestNext = pIntDestinationState; // This saves the path required for entry actions in member pDestNext.  // Cast allow write.
		pIntDestinationState = pIntDestinationState->m_pParent;

		DBOSASSERT(pIntDestinationState->m_Level == pIntSourceState->m_Level)
	}

	DBOSASSERT(m_pCurrentState == pIntDestinationState)

	// pIntDestinationState and pIntSourceState both point to the same state which is the least common ancestor state of the Source and Destination states.
	// All exit actions complete at this point.

	pDBOS_HSMSTATE_CONST_t pPrevDestinationState = NULLPTR;

	while((pNextState == NULLPTR) && (pIntDestinationState != pDest)){

		pPrevDestinationState = pIntDestinationState;
		pIntDestinationState = pIntDestinationState->m_pDestNext;
		const_cast<pDBOS_HSMSTATE_t>(pPrevDestinationState)->m_pDestNext = NULLPTR; // Finished with this, clear it to unused state.  // Cast allow write.

		DBOSASSERT(pIntDestinationState != NULLPTR)

		DBOSASSERT(pIntDestinationState->m_pParent == m_pCurrentState)

		EntryChecksResult = pIntDestinationState->EntryChecks(m_pStates, m_pExtended, &pNextState);

		if(EntryChecksResult == dBOS_HSM_ENTRY_CHECKS_PASSED){

			DBOSASSERT(pNextState == NULLPTR)

			if((m_DebugMode == HSM_DEBUG_TRANSISTIONS_ONLY) || (m_DebugMode == HSM_DEBUG_ALL_EVENTS)){
				DBOS_printf("\n ");

				UNATIVECPU_t i;
				for(i = pIntDestinationState->m_Level; i > 0U ; i--){
					DBOS_printf("  ");
				}

				DBOS_printf("State Entry: %s", pIntDestinationState->m_pName);
			}

			m_pCurrentState = pIntDestinationState;
			pIntDestinationState->Entry(m_pExtended);
		}
		else{

			DBOSASSERT(EntryChecksResult == dBOS_HSM_TRANSISTION)
			DBOSASSERT(pNextState != NULLPTR)

			if((m_DebugMode == HSM_DEBUG_TRANSISTIONS_ONLY) || (m_DebugMode == HSM_DEBUG_ALL_EVENTS)){
				DBOS_printf("\n ");

				UNATIVECPU_t i;
				for(i = pIntDestinationState->m_Level; i > 0U ; i--){
					DBOS_printf("  ");
				}

				DBOS_printf("State: %s - Entry Checks FAILED", pIntDestinationState->m_pName);
			}
		}

	}

	if(pNextState != NULLPTR){
		// While loop might have finished above without pIntDestinationState equal to pDest.
		// Complete the loop here to set pDestNext member of HSM_STATE_t back to NULLPTR
		while(pIntDestinationState != pDest){
			pPrevDestinationState = pIntDestinationState;
			pIntDestinationState = pIntDestinationState->m_pDestNext;
			const_cast<pDBOS_HSMSTATE_t>(pPrevDestinationState)->m_pDestNext = NULLPTR; // Finished with this, clear it to unused state.  // Cast allow write.

			DBOSASSERT(pIntDestinationState != NULLPTR)
		}

	}

	DBOSASSERT(pIntDestinationState->m_pDestNext == NULLPTR)

	return pNextState;
}

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
