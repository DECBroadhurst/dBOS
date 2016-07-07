/**
 * @file dBOS_QueueInterface.hpp
 * @author David Broadhurst
 * @date 15/11/2015
 *
 * \brief Definition of dBOS Queue Interface.
 *
 */

// -------------------------------------------------------------------------------------------------
#ifndef DBOS_QUEUEINTERFACE_H_
#define DBOS_QUEUEINTERFACE_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_SyncInterface.hpp"
#include "dBOS_KernelInterface.hpp"

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
class dBOS_QUEUEINTERFACE {

public:

	dBOS_QUEUEINTERFACE(DBOS_ID_t const ID, UNATIVECPU_t const Size, UNATIVECPU_t const ItemSize);
	dBOS_QUEUEINTERFACE(DBOS_ID_t const ID, UNATIVECPU_t const Size, UNATIVECPU_t const ItemSize, INTERRUPT_MASK_t const Mask);

	virtual ~dBOS_QUEUEINTERFACE();

	dBOS_QUEUEINTERFACE() = delete;
	dBOS_QUEUEINTERFACE(dBOS_QUEUEINTERFACE const &) = delete;
	dBOS_QUEUEINTERFACE& operator=(dBOS_QUEUEINTERFACE const &) = delete;

	UNATIVECPU_t GetLevel(void) const;

	dBOS_SEMAPHOREINTERFACE_P_t GetEmptyCountSemaphore(void);
	dBOS_SEMAPHOREINTERFACE_P_t GetFullCountSemaphore(void);

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails() const;
	virtual void PrintTypeAndID() const;
#endif

protected:

private:

	friend class dBOS_KERNELINTERFACE;

	class dBOS_QUEUESEMAPHORE * m_pSemaphoreEmptyCount;
	class dBOS_QUEUESEMAPHORE * m_pSemaphoreFullCount;
	dBOS_SEMAPHOREINTERFACE_P_t m_pSemInterfaceEmptyCount;
	dBOS_SEMAPHOREINTERFACE_P_t m_pSemInterfaceFullCount;
	DBOS_QUEUE_P_t m_pImpl;

	static class dBOS_KERNELINTERFACE * m_pKernelInterface;

};

} // namespace

#endif /* DBOS_QUEUEINTERFACE_H_ */
