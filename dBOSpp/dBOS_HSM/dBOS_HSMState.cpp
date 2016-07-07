/**
 * @file dBOS_HSMState.cpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Implementation of dBOS Hierarchical State Machine State.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSMState.hpp"
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
dBOS_HSMSTATE::dBOS_HSMSTATE(char const * const pName) :
		m_pParent(NULLPTR), m_pName(pName), m_Level(0U), m_pDestNext(NULLPTR){

}

dBOS_HSMSTATE::dBOS_HSMSTATE(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName) :
		m_pParent(pParent), m_pName(pName), m_Level(pParent->m_Level + 1U), m_pDestNext(NULLPTR){

	DBOSASSERT(m_pParent != NULLPTR)
}

dBOS_HSMSTATE::~dBOS_HSMSTATE(){

}

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
