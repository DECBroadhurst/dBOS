/**
 * @file dBOS_Base.cpp
 * @author David Broadhurst
 * @date 15/01/2015
 *
 * \brief Implementation of dBOS Base Class.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Base.hpp"
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
dBOS_BASE::dBOS_BASE(DBOS_ID_t const ID) :
		m_ID(ID){

}

dBOS_BASE::~dBOS_BASE(void){

}

DBOS_ID_t dBOS_BASE::GetID(void) const{

	return m_ID;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_BASE::PrintID(void) const{

#if (DBOS_USE_OBJECTNAMES == 1U)
	DBOS_Intprintf("%s", m_ID);
#else
	DBOS_Intprintf("%u", m_ID);
#endif

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
