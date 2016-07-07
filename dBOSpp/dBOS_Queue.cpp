/**
 * @file dBOS_Queue.cpp
 * @author David Broadhurst
 * @date 15/11/2015
 *
 * \brief Implementation of dBOS Queue.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Queue.hpp"
#include "dBOS_Port.hpp"
#include "dBOS_DebugPrint.hpp"
#include "dBOS_Assert.hpp"
#include <string.h>

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
dBOS_QUEUE::dBOS_QUEUE(DBOS_ID_t const ID, UNATIVECPU_t const Size, UNATIVECPU_t const ItemSize) :
		dBOS_BASE(ID), m_Size(Size), m_ItemSize(ItemSize), m_ISRMask(ISRMASK_NONE), m_QueueSpace(NULLPTR), m_Level(0U), m_Wpnt(0U), m_Rpnt(0U){

	m_QueueSpace = new INT8U[Size * ItemSize];
}

dBOS_QUEUE::dBOS_QUEUE(DBOS_ID_t const ID, UNATIVECPU_t const Size, UNATIVECPU_t const ItemSize, INTERRUPT_MASK_t const Mask) :
		dBOS_BASE(ID), m_Size(Size), m_ItemSize(ItemSize), m_ISRMask(Mask), m_QueueSpace(NULLPTR), m_Level(0U), m_Wpnt(0U), m_Rpnt(0U){

	m_QueueSpace = new INT8U[Size * ItemSize];
}

dBOS_QUEUE::~dBOS_QUEUE(void){

}

DBOS_RESULT_t dBOS_QUEUE::WriteToBack(void const * const pData){

	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(m_ISRMask) == TRUE)

	DBOS_RESULT_t Result;

	if(m_Level < m_Size){

		INT8U * const pnt = m_QueueSpace + (m_Wpnt * m_ItemSize);

		memcpy(pnt, pData, m_ItemSize);

		m_Level++;

		m_Wpnt++;
		if(m_Wpnt >= m_Size){
			m_Wpnt = 0U;
		}

		Result = DBOS_Q_WRITTEN;
	}
	else{
		Result = DBOS_Q_WRITE_FAIL_FULL;
	}

	return Result;
}

DBOS_RESULT_t dBOS_QUEUE::WriteToFront(void const * const pData){

	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(m_ISRMask) == TRUE)

	DBOS_RESULT_t Result;

	if(m_Level < m_Size){

		if(m_Rpnt == 0U){
			m_Rpnt = m_Size - 1U;
		}
		else{
			m_Rpnt--;
		}

		INT8U * const pnt = m_QueueSpace + (m_Rpnt * m_ItemSize);

		memcpy(pnt, pData, m_ItemSize);

		m_Level++;

		Result = DBOS_Q_WRITTEN;
	}
	else{
		Result = DBOS_Q_WRITE_FAIL_FULL;
	}

	return Result;
}

DBOS_RESULT_t dBOS_QUEUE::ReadFromFront(void * const pData){

	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(m_ISRMask) == TRUE)

	DBOS_RESULT_t Result;

	if(m_Level > 0U){

		INT8U * const pnt = m_QueueSpace + (m_Rpnt * m_ItemSize);

		memcpy(pData, pnt, m_ItemSize);

		m_Level--;

		m_Rpnt++;
		if(m_Rpnt >= m_Size){
			m_Rpnt = 0U;
		}

		Result = DBOS_Q_READ;
	}
	else{
		Result = DBOS_Q_READ_FAIL_EMPTY;
	}

	return Result;
}

DBOS_RESULT_t dBOS_QUEUE::ReadFromBack(void * const pData){

	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(m_ISRMask) == TRUE)

	DBOS_RESULT_t Result;

	if(m_Level > 0U){

		if(m_Wpnt == 0U){
			m_Wpnt = m_Size - 1;
		}
		else{
			m_Wpnt--;
		}

		INT8U * const pnt = m_QueueSpace + (m_Wpnt * m_ItemSize);

		memcpy(pData, pnt, m_ItemSize);

		m_Level--;

		Result = DBOS_Q_READ;
	}
	else{
		Result = DBOS_Q_READ_FAIL_EMPTY;
	}

	return Result;
}

UNATIVECPU_t dBOS_QUEUE::GetLevel(void) const{

	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(m_ISRMask) == TRUE)

	return m_Level;
}

INTERRUPT_MASK_t dBOS_QUEUE::GetISRMask(void) const{

	return m_ISRMask;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
void dBOS_QUEUE::PrintDetails() const{

	this->PrintTypeAndID();

	DBOS_Intprintf("\n  Depth: %i", m_Size);
	DBOS_Intprintf("\n  Current Level: %i", m_Level);
	DBOS_Intprintf("\n  Item Size: %i Bytes", m_ItemSize);
}

void dBOS_QUEUE::PrintTypeAndID() const{

	DBOS_Intprintf("Queue: ");
	this->PrintID();
}
#endif

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
