/**
 * @file dBOS_Queue.hpp
 * @author David Broadhurst
 * @date 15/11/2015
 *
 * \brief Definition of dBOS Queue.
 *
 */

// -------------------------------------------------------------------------------------------------
#ifndef DBOS_QUEUE_H_
#define DBOS_QUEUE_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Base.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// FORWARD DECLERATIONS

// -------------------------------------------------------------------------------------------------
// DATA TYPES

// -------------------------------------------------------------------------------------------------
// CONSTANTS

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// CLASSES
class dBOS_QUEUE : public dBOS_BASE {

public:

	dBOS_QUEUE(DBOS_ID_t const ID, UNATIVECPU_t const Size, UNATIVECPU_t const ItemSize);
	dBOS_QUEUE(DBOS_ID_t const ID, UNATIVECPU_t const Size, UNATIVECPU_t const ItemSize, INTERRUPT_MASK_t const Mask);

	virtual ~dBOS_QUEUE();

	dBOS_QUEUE() = delete;
	dBOS_QUEUE(dBOS_QUEUE const &) = delete;
	dBOS_QUEUE& operator=(dBOS_QUEUE const &) = delete;

	DBOS_RESULT_t WriteToBack(void const * const pData);
	DBOS_RESULT_t WriteToFront(void const * const pData);
	DBOS_RESULT_t ReadFromFront(void * const pData);
	DBOS_RESULT_t ReadFromBack(void * const pData);

	UNATIVECPU_t GetLevel(void) const;

	INTERRUPT_MASK_t GetISRMask(void) const;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails() const;
	virtual void PrintTypeAndID() const;
#endif

protected:

private:

	UNATIVECPU_t const m_Size;
	UNATIVECPU_t const m_ItemSize;

	INTERRUPT_MASK_t const m_ISRMask;

	INT8U * m_QueueSpace;
	UNATIVECPU_t m_Level;
	UNATIVECPU_t m_Wpnt;
	UNATIVECPU_t m_Rpnt;

};

}// namespace

#endif /* DBOS_QUEUE_H_ */
