/**
 * @file dBOS_HSMTestBase.cpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Implementation of a dBOS HSM for unit testing.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSMTestBase.hpp"
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
dBOS_HSMTESTSTATE_S1::dBOS_HSMTESTSTATE_S1(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S1::dBOS_HSMTESTSTATE_S1(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S1::~dBOS_HSMTESTSTATE_S1(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S1::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pStates;
	(void) pExtended;
	(void) pNextState;

	DBOS_Test_LogEvent('1');
	DBOS_Test_LogEvent('C');

	return dBOS_HSM_ENTRY_CHECKS_PASSED;
}

void dBOS_HSMTESTSTATE_S1::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('1');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S1::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('1');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S1::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	dBOS_HSMTESTBASESTATES const * const pBaseStates = static_cast<dBOS_HSMTESTBASESTATES const *>(pStates);

	dBOS_HSM_HANDLER_RETURN_t Result;

	switch(*pEvent->m_pProp){
	case EVENT1:
		DBOS_Test_LogEvent('1');
		DBOS_Test_LogEvent('H');
		DBOS_Test_LogEvent('1');
		Result = dBOS_HSM_EVENT_HANDLED;
		break;
	case EVENT4:
		DBOS_Test_LogEvent('1');
		DBOS_Test_LogEvent('H');
		DBOS_Test_LogEvent('4');
		Result = dBOS_HSM_EVENT_HANDLED;
		break;
	case EVENT5:
		DBOS_Test_LogEvent('1');
		DBOS_Test_LogEvent('H');
		DBOS_Test_LogEvent('5');
		Result = dBOS_HSM_EVENT_HANDLED;
		break;
	case EVENT6:
		*pNextState = pBaseStates->pS1221;
		Result = dBOS_HSM_TRANSISTION;
		break;
	default:
		Result = dBOS_HSM_EVENT_NOTHANDLED;
		break;
	}

	return Result;
}

// ================================================================================================
dBOS_HSMTESTSTATE_S11::dBOS_HSMTESTSTATE_S11(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S11::dBOS_HSMTESTSTATE_S11(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S11::~dBOS_HSMTESTSTATE_S11(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S11::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	dBOS_HSMTESTBASESTATES const * const pBaseStates = static_cast<dBOS_HSMTESTBASESTATES const *>(pStates);
	(void) pExtended;

	DBOS_Test_LogEvent('2');
	DBOS_Test_LogEvent('C');

	*pNextState = pBaseStates->pS12;
	return dBOS_HSM_TRANSISTION;
}

void dBOS_HSMTESTSTATE_S11::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('2');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S11::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('2');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S11::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	(void) pStates;
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
dBOS_HSMTESTSTATE_S12::dBOS_HSMTESTSTATE_S12(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S12::dBOS_HSMTESTSTATE_S12(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S12::~dBOS_HSMTESTSTATE_S12(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S12::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pStates;
	(void) pExtended;
	(void) pNextState;

	DBOS_Test_LogEvent('3');
	DBOS_Test_LogEvent('C');

	return dBOS_HSM_ENTRY_CHECKS_PASSED;
}

void dBOS_HSMTESTSTATE_S12::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('3');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S12::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('3');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S12::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	(void) pStates;
	(void) pNextState;

	dBOS_HSM_HANDLER_RETURN_t Result;

	switch(*pEvent->m_pProp){
	case EVENT1:
		DBOS_Test_LogEvent('3');
		DBOS_Test_LogEvent('H');
		DBOS_Test_LogEvent('1');
		Result = dBOS_HSM_EVENT_HANDLED;
		break;
	case EVENT5:
		DBOS_Test_LogEvent('3');
		DBOS_Test_LogEvent('H');
		DBOS_Test_LogEvent('5');
		Result = dBOS_HSM_EVENT_HANDLED_PASS_TO_PARENT;
		break;
	default:
		Result = dBOS_HSM_EVENT_NOTHANDLED;
		break;
	}

	return Result;
}

// ================================================================================================
dBOS_HSMTESTSTATE_S121::dBOS_HSMTESTSTATE_S121(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S121::dBOS_HSMTESTSTATE_S121(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S121::~dBOS_HSMTESTSTATE_S121(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S121::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pStates;
	(void) pExtended;
	(void) pNextState;

	DBOS_Test_LogEvent('4');
	DBOS_Test_LogEvent('C');

	return dBOS_HSM_ENTRY_CHECKS_PASSED;
}

void dBOS_HSMTESTSTATE_S121::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('4');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S121::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('4');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S121::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	(void) pStates;
	(void) pNextState;

	dBOS_HSM_HANDLER_RETURN_t Result;

	switch(*pEvent->m_pProp){
	case EVENT1:
		DBOS_Test_LogEvent('4');
		DBOS_Test_LogEvent('H');
		DBOS_Test_LogEvent('1');
		Result = dBOS_HSM_EVENT_HANDLED;
		break;
	case EVENT5:
		DBOS_Test_LogEvent('4');
		DBOS_Test_LogEvent('H');
		DBOS_Test_LogEvent('5');
		Result = dBOS_HSM_EVENT_HANDLED_PASS_TO_PARENT;
		break;
	case EVENT6:
		DBOS_Test_LogEvent('4');
		DBOS_Test_LogEvent('H');
		DBOS_Test_LogEvent('6');
		Result = dBOS_HSM_EVENT_HANDLED_PASS_TO_PARENT;
		break;
	default:
		Result = dBOS_HSM_EVENT_NOTHANDLED;
		break;
	}

	return Result;
}

// ================================================================================================
dBOS_HSMTESTSTATE_S122::dBOS_HSMTESTSTATE_S122(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S122::dBOS_HSMTESTSTATE_S122(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S122::~dBOS_HSMTESTSTATE_S122(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S122::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pStates;
	(void) pExtended;
	(void) pNextState;

	DBOS_Test_LogEvent('5');
	DBOS_Test_LogEvent('C');

	return dBOS_HSM_ENTRY_CHECKS_PASSED;
}

void dBOS_HSMTESTSTATE_S122::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('5');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S122::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('5');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S122::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	dBOS_HSMTESTBASESTATES const * const pBaseStates = static_cast<dBOS_HSMTESTBASESTATES const *>(pStates);

	dBOS_HSM_HANDLER_RETURN_t Result;

	switch(*pEvent->m_pProp){
	case EVENT1:
		Result = dBOS_HSM_EVENT_HANDLED;
		break;
	case EVENT8:
		DBOS_TEST_INT32U(pEvent->m_Data.Uint32, 5U)

		*pNextState = pBaseStates->pS1221;
		Result = dBOS_HSM_TRANSISTION;
		break;
	default:
		Result = dBOS_HSM_EVENT_NOTHANDLED;
		break;
	}

	return Result;
}

// ================================================================================================
dBOS_HSMTESTSTATE_S1221::dBOS_HSMTESTSTATE_S1221(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S1221::dBOS_HSMTESTSTATE_S1221(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S1221::~dBOS_HSMTESTSTATE_S1221(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S1221::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pStates;
	(void) pExtended;
	(void) pNextState;

	DBOS_Test_LogEvent('6');
	DBOS_Test_LogEvent('C');

	return dBOS_HSM_ENTRY_CHECKS_PASSED;
}

void dBOS_HSMTESTSTATE_S1221::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('6');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S1221::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('6');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S1221::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	dBOS_HSMTESTBASESTATES const * const pBaseStates = static_cast<dBOS_HSMTESTBASESTATES const *>(pStates);

	dBOS_HSM_HANDLER_RETURN_t Result;

	switch(*pEvent->m_pProp){
	case EVENT1:
		DBOS_Test_LogEvent('6');
		DBOS_Test_LogEvent('H');
		DBOS_Test_LogEvent('1');
		Result = dBOS_HSM_EVENT_HANDLED;
		break;
	case EVENT2:
		*pNextState = pBaseStates->pS121;
		Result = dBOS_HSM_TRANSISTION;
		break;
	case EVENT7:
		*pNextState = pBaseStates->pS1321;
		Result = dBOS_HSM_TRANSISTION;
		break;
	case EVENT8:
		DBOS_TEST_INT32U(pEvent->m_Data.Uint32, 4U)

		*pNextState = pBaseStates->pS122;
		Result = dBOS_HSM_TRANSISTION;
		break;
	case EVENT9:
		DBOS_TEST_INT32U(pEvent->m_Data.Uint32, 6U)

		*pNextState = pBaseStates->pS1221;
		Result = dBOS_HSM_TRANSISTION;
		break;
	case EVENT10:

		*pNextState = pBaseStates->pS131;
		Result = dBOS_HSM_TRANSISTION;
		break;
	default:
		Result = dBOS_HSM_EVENT_NOTHANDLED;
		break;
	}

	return Result;
}

// ================================================================================================
dBOS_HSMTESTSTATE_S13::dBOS_HSMTESTSTATE_S13(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S13::dBOS_HSMTESTSTATE_S13(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S13::~dBOS_HSMTESTSTATE_S13(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S13::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pStates;
	(void) pExtended;
	(void) pNextState;

	DBOS_Test_LogEvent('7');
	DBOS_Test_LogEvent('C');

	return dBOS_HSM_ENTRY_CHECKS_PASSED;
}

void dBOS_HSMTESTSTATE_S13::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('7');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S13::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('7');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S13::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	dBOS_HSMTESTBASESTATES const * const pBaseStates = static_cast<dBOS_HSMTESTBASESTATES const *>(pStates);

	dBOS_HSM_HANDLER_RETURN_t Result;

	switch(*pEvent->m_pProp){
	case EVENT1:
		Result = dBOS_HSM_EVENT_HANDLED;
		break;
	case EVENT3:
		*pNextState = pBaseStates->pS1221;
		Result = dBOS_HSM_TRANSISTION;
		break;
	default:
		Result = dBOS_HSM_EVENT_NOTHANDLED;
		break;
	}

	return Result;
}

// ================================================================================================
dBOS_HSMTESTSTATE_S131::dBOS_HSMTESTSTATE_S131(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S131::dBOS_HSMTESTSTATE_S131(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S131::~dBOS_HSMTESTSTATE_S131(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S131::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pStates;
	(void) pExtended;
	(void) pNextState;

	DBOS_Test_LogEvent('8');
	DBOS_Test_LogEvent('C');

	return dBOS_HSM_ENTRY_CHECKS_PASSED;
}

void dBOS_HSMTESTSTATE_S131::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('8');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S131::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('8');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S131::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	(void) pStates;
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
dBOS_HSMTESTSTATE_S132::dBOS_HSMTESTSTATE_S132(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S132::dBOS_HSMTESTSTATE_S132(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S132::~dBOS_HSMTESTSTATE_S132(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S132::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	dBOS_HSMTESTBASESTATES const * const pBaseStates = static_cast<dBOS_HSMTESTBASESTATES const *>(pStates);

	(void) pExtended;
	(void) pNextState;

	DBOS_Test_LogEvent('9');
	DBOS_Test_LogEvent('C');

	*pNextState = pBaseStates->pS11;
	return dBOS_HSM_TRANSISTION;
}

void dBOS_HSMTESTSTATE_S132::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('9');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S132::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('9');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S132::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	(void) pStates;
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
dBOS_HSMTESTSTATE_S1321::dBOS_HSMTESTSTATE_S1321(char const * const pName) :
		dBOS_HSMSTATE(pName){

}

dBOS_HSMTESTSTATE_S1321::dBOS_HSMTESTSTATE_S1321(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		dBOS_HSMSTATE(pParent, pName){

}

dBOS_HSMTESTSTATE_S1321::~dBOS_HSMTESTSTATE_S1321(){

}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S1321::EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pStates;
	(void) pExtended;
	(void) pNextState;

	DBOS_Test_LogEvent('A');
	DBOS_Test_LogEvent('C');

	return dBOS_HSM_ENTRY_CHECKS_PASSED;
}

void dBOS_HSMTESTSTATE_S1321::Entry(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('A');
	DBOS_Test_LogEvent('E');
}

void dBOS_HSMTESTSTATE_S1321::Exit(void * const pExtended) const{

	(void) pExtended;

	DBOS_Test_LogEvent('A');
	DBOS_Test_LogEvent('X');
}

dBOS_HSM_HANDLER_RETURN_t dBOS_HSMTESTSTATE_S1321::Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const{

	(void) pExtended;
	(void) pStates;
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

// ================================================================================================
dBOS_HSMTESTBASE::dBOS_HSMTESTBASE(char const * const pName, dBOS_HSM_DEBUG_MODE_t const DebugMode) :
		dBOS_HSM(pName, DebugMode){

}

dBOS_HSMTESTBASE::~dBOS_HSMTESTBASE(){

}

void dBOS_HSMTESTBASE::Init(void){

	if(m_pExtended == NULLPTR){ // If extended state variables struct hasn't been created by sub class.
		m_pExtended = NULLPTR;
	}

	dBOS_HSMTESTBASESTATES * pBaseStates;

	if(m_pStates == NULLPTR){
		pBaseStates = new dBOS_HSMTESTBASESTATES;
		m_pStates = pBaseStates;
	}
	else{
		pBaseStates = static_cast<dBOS_HSMTESTBASESTATES *>(m_pStates);
	}

	if(pBaseStates->pTopState == NULLPTR){ // If state hasn't been created by sub class.
		pBaseStates->pTopState = new dBOS_HSMTESTSTATE_S1("S1");
	}

	if(pBaseStates->pS11 == NULLPTR){ // If state hasn't been created by sub class.
		pBaseStates->pS11 = new dBOS_HSMTESTSTATE_S11(pBaseStates->pTopState, "S11");
	}

	if(pBaseStates->pS12 == NULLPTR){ // If state hasn't been created by sub class.
		pBaseStates->pS12 = new dBOS_HSMTESTSTATE_S12(pBaseStates->pTopState, "S12");
	}

	if(pBaseStates->pS121 == NULLPTR){ // If state hasn't been created by sub class.
		pBaseStates->pS121 = new dBOS_HSMTESTSTATE_S121(pBaseStates->pS12, "S121");
	}

	if(pBaseStates->pS122 == NULLPTR){ // If state hasn't been created by sub class.
		pBaseStates->pS122 = new dBOS_HSMTESTSTATE_S122(pBaseStates->pS12, "S122");
	}

	if(pBaseStates->pS1221 == NULLPTR){ // If state hasn't been created by sub class.
		pBaseStates->pS1221 = new dBOS_HSMTESTSTATE_S1221(pBaseStates->pS122, "S1221");
	}

	if(pBaseStates->pS13 == NULLPTR){ // If state hasn't been created by sub class.
		pBaseStates->pS13 = new dBOS_HSMTESTSTATE_S13(pBaseStates->pTopState, "S13");
	}

	if(pBaseStates->pS131 == NULLPTR){ // If state hasn't been created by sub class.
		pBaseStates->pS131 = new dBOS_HSMTESTSTATE_S131(pBaseStates->pS13, "S131");
	}

	if(pBaseStates->pS132 == NULLPTR){ // If state hasn't been created by sub class.
		pBaseStates->pS132 = new dBOS_HSMTESTSTATE_S132(pBaseStates->pS13, "S132");
	}

	if(pBaseStates->pS1321 == NULLPTR){ // If state hasn't been created by sub class.
		pBaseStates->pS1321 = new dBOS_HSMTESTSTATE_S1321(pBaseStates->pS132, "S1321");
	}

	this->Start(pBaseStates->pS1221);

}

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
