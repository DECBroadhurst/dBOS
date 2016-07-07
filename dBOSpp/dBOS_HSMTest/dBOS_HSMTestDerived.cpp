/**
 * @file dBOS_HSMTestDerived.cpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Implementation of a dBOS HSM derived from dBOS_HSMTESTBASE for unit testing.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSMTestDerived.hpp"
#include "dBOS_HSMTestEvents.hpp"
#include "TestSys.hpp"

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
// ================================================================================================
dBOS_HSMTESTSTATE_S1_1::dBOS_HSMTESTSTATE_S1_1(char const * const pName) :
		dBOS_HSMTESTSTATE_S1(pName){

}

dBOS_HSMTESTSTATE_S1_1::dBOS_HSMTESTSTATE_S1_1(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMTESTSTATE_S1(pParent, pName){

}

dBOS_HSMTESTSTATE_S1_1::~dBOS_HSMTESTSTATE_S1_1(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S1_1::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	return dBOS_HSMTESTSTATE_S1::EntryChecks(pStates, pExtended, pNextState);
}

void dBOS_HSMTESTSTATE_S1_1::Entry(void * const pExtended) const{

	dBOS_HSMTESTSTATE_S1::Entry(pExtended);
}

void dBOS_HSMTESTSTATE_S1_1::Exit(void * const pExtended) const{

	dBOS_HSMTESTSTATE_S1::Exit(pExtended);
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S1_1::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	dBOS_HSMTESTDERIVEDSTATES const * const pDerivedStates = static_cast<dBOS_HSMTESTDERIVEDSTATES const *>(pStates);

	dBOS_HSM_HANDLER_RETURN_t Result;

	switch(*pEvent->m_pProp){
	case EVENT11:
		*pNextState = pDerivedStates->pS123;
		Result = dBOS_HSM_TRANSISTION;
		break;
	default:
		Result = dBOS_HSMTESTSTATE_S1::Event(pStates, pExtended, pEvent, pNextState); // Event not handled, let state this state is derived from handle it.
		break;
	}

	return Result;
}

// ================================================================================================
dBOS_HSMTESTSTATE_S123::dBOS_HSMTESTSTATE_S123(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S123::dBOS_HSMTESTSTATE_S123(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S123::~dBOS_HSMTESTSTATE_S123(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S123::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pStates;
	(void) pExtended;
	(void) pNextState;

	DBOS_Test_LogEvent('B');
	DBOS_Test_LogEvent('C');

	return dBOS_HSM_ENTRY_CHECKS_PASSED;
}

void dBOS_HSMTESTSTATE_S123::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('B');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S123::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('B');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S123::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pStates;
	(void) pExtended;
	(void) pNextState;

	dBOS_HSM_HANDLER_RETURN_t Result;

	switch(*pEvent->m_pProp){
	default:
		Result = dBOS_HSM_EVENT_NOTHANDLED;
		break;
	}

	return Result;
}

// ================================================================================================
dBOS_HSMTESTDERIVED::dBOS_HSMTESTDERIVED(char const * const pName, dBOS_HSM_DEBUG_MODE_t const DebugMode) :
		dBOS_HSMTESTBASE(pName, DebugMode){

}

dBOS_HSMTESTDERIVED::~dBOS_HSMTESTDERIVED(){

}

void dBOS_HSMTESTDERIVED::Init(void){

	if(m_pExtended == NULLPTR){ // If extended state variables struct hasn't been created by sub class.
		m_pExtended = NULLPTR;
	}

	dBOS_HSMTESTDERIVEDSTATES * pStates;

	if(m_pStates == NULLPTR){
		pStates = new dBOS_HSMTESTDERIVEDSTATES;
		m_pStates = pStates;
	}
	else{
		pStates = static_cast<dBOS_HSMTESTDERIVEDSTATES *>(m_pStates);
	}

	if(pStates->pTopState == NULLPTR){ // If state hasn't been created by sub class.
		pStates->pTopState = new dBOS_HSMTESTSTATE_S1_1("S1");
	}

	dBOS_HSMTESTBASE::Init(); // Create all other states, TopState will not be created, it was created above.

	if(pStates->pS123 == NULLPTR){ // If state hasn't been created by sub class.
		pStates->pS123 = new dBOS_HSMTESTSTATE_S123(pStates->pS12, "S123"); // Can now create S123, parent state S12 was created in dBOS_HSMTESTBASE::Init();
	}
}

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
