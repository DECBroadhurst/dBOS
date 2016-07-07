/**
 * @file dBOS_HSMEvent.cpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Implementation of dBOS Hierarchical State Machine Event.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSMEvent.hpp"

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
dBOS_HSMEVENTDATA::dBOS_HSMEVENTDATA() :
		Uint32(0U){

}

dBOS_HSMEVENTDATA::dBOS_HSMEVENTDATA(const dBOS_HSMEVENTDATA &D) :
		Uint32(D.Uint32){

}

dBOS_HSMEVENTDATA & dBOS_HSMEVENTDATA::operator=(const dBOS_HSMEVENTDATA & D){

	Uint32 = D.Uint32;

	return *this;
}

dBOS_HSMEVENTDATA::~dBOS_HSMEVENTDATA(){

}

dBOS_HSMEVENT::dBOS_HSMEVENT() :
		m_pProp(NULLPTR), m_Data(){

}

dBOS_HSMEVENT::dBOS_HSMEVENT(pdBOS_HSMEVENTPROP_CONST_t const pProp, dBOS_HSMEVENTDATA const Data) :
		m_pProp(pProp), m_Data(Data){

}

dBOS_HSMEVENT::dBOS_HSMEVENT(const dBOS_HSMEVENT &E) :
		m_pProp(E.m_pProp), m_Data(E.m_Data){

}

dBOS_HSMEVENT & dBOS_HSMEVENT::operator=(const dBOS_HSMEVENT & E){

	m_pProp = E.m_pProp;
	m_Data = E.m_Data;

	return *this;
}

dBOS_HSMEVENT::~dBOS_HSMEVENT(){

}

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
