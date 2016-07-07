/**
 * @file dBOS_QueueInterface.cpp
 * @author David Broadhurst
 * @date 15/11/2015
 *
 * \brief Implementation of dBOS Queue Interface.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_QueueInterface.hpp"
#include "dBOS_Queue.hpp"
#include "dBOS_Kernel.hpp"
#include "dBOS_Port.hpp"
#include "dBOS_PortFunctions.hpp"
#include "dBOS_QueueSemaphore.hpp"

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
#include "dBOS_DebugPrint.hpp"
#endif

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
dBOS_KERNELINTERFACE * dBOS_QUEUEINTERFACE::m_pKernelInterface = NULLPTR;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
dBOS_QUEUEINTERFACE::dBOS_QUEUEINTERFACE(DBOS_ID_t const ID, UNATIVECPU_t const Size, UNATIVECPU_t const ItemSize) :
		m_pSemaphoreEmptyCount(NULLPTR), m_pSemaphoreFullCount(NULLPTR), m_pSemInterfaceEmptyCount(NULLPTR), m_pSemInterfaceFullCount(NULLPTR), m_pImpl(NULLPTR){

	dBOS_KERNEL * const m_pK = dBOS_KERNEL::GetInstance();

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	m_pSemaphoreEmptyCount = new dBOS_QUEUESEMAPHORE(ID, m_pK->GetNextSyncObjectID(), Size, Size, FIRSTTOWAITFIRSTTORUNLIST);

	if(m_pSemaphoreEmptyCount != NULLPTR){
		m_pSemInterfaceEmptyCount = new dBOS_SEMAPHOREINTERFACE(m_pSemaphoreEmptyCount);

		if(m_pSemInterfaceEmptyCount != NULLPTR){
			m_pK->RegisterSyncObject(m_pSemaphoreEmptyCount);
		}
	}

	m_pSemaphoreFullCount = new dBOS_QUEUESEMAPHORE(ID, m_pK->GetNextSyncObjectID(), 0U, Size, PRIORITYLIST);

	if(m_pSemaphoreFullCount != NULLPTR){
		m_pSemInterfaceFullCount = new dBOS_SEMAPHOREINTERFACE(m_pSemaphoreFullCount);

		if(m_pSemInterfaceFullCount != NULLPTR){
			m_pK->RegisterSyncObject(m_pSemaphoreFullCount);
		}
	}

	if((m_pSemInterfaceFullCount != NULLPTR) && (m_pSemInterfaceEmptyCount != NULLPTR)){
		m_pImpl = new dBOS_QUEUE(ID, Size, ItemSize);
	}

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section

}

dBOS_QUEUEINTERFACE::dBOS_QUEUEINTERFACE(DBOS_ID_t const ID, UNATIVECPU_t const Size, UNATIVECPU_t const ItemSize, INTERRUPT_MASK_t const Mask) :
		m_pSemaphoreEmptyCount(NULLPTR), m_pSemaphoreFullCount(NULLPTR), m_pSemInterfaceEmptyCount(NULLPTR), m_pSemInterfaceFullCount(NULLPTR), m_pImpl(NULLPTR){

	dBOS_KERNEL * const m_pK = dBOS_KERNEL::GetInstance();

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	m_pSemaphoreEmptyCount = new dBOS_QUEUESEMAPHORE(ID, m_pK->GetNextSyncObjectID(), Size, Size, FIRSTTOWAITFIRSTTORUNLIST, Mask);

	if(m_pSemaphoreEmptyCount != NULLPTR){
		m_pSemInterfaceEmptyCount = new dBOS_SEMAPHOREINTERFACE(m_pSemaphoreEmptyCount);

		if(m_pSemInterfaceEmptyCount != NULLPTR){
			m_pK->RegisterSyncObject(m_pSemaphoreEmptyCount);
		}
	}

	m_pSemaphoreFullCount = new dBOS_QUEUESEMAPHORE(ID, m_pK->GetNextSyncObjectID(), 0U, Size, PRIORITYLIST, Mask);

	if(m_pSemaphoreFullCount != NULLPTR){
		m_pSemInterfaceFullCount = new dBOS_SEMAPHOREINTERFACE(m_pSemaphoreFullCount);

		if(m_pSemInterfaceFullCount != NULLPTR){
			m_pK->RegisterSyncObject(m_pSemaphoreFullCount);
		}
	}

	if((m_pSemInterfaceFullCount != NULLPTR) && (m_pSemInterfaceEmptyCount != NULLPTR)){
		m_pImpl = new dBOS_QUEUE(ID, Size, ItemSize, Mask);
	}

	DBOS_Port_ExitCritical(PrevCS);	// Exit OS critical section
}

dBOS_QUEUEINTERFACE::~dBOS_QUEUEINTERFACE(void){

}

UNATIVECPU_t dBOS_QUEUEINTERFACE::GetLevel(void) const{

	return m_pImpl->GetLevel();
}

dBOS_SEMAPHOREINTERFACE_P_t dBOS_QUEUEINTERFACE::GetEmptyCountSemaphore(void){

	return m_pSemInterfaceEmptyCount;
}

dBOS_SEMAPHOREINTERFACE_P_t dBOS_QUEUEINTERFACE::GetFullCountSemaphore(void){

	return m_pSemInterfaceFullCount;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_QUEUEINTERFACE::PrintDetails() const{

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	INTERRUPT_MASK_t const PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(m_pImpl->GetISRMask());	// Enter ISR critical section

	ASSERT_FROM_OSCS(PrevISRMask != ISRMASK_NONE)
	// DBOS_Port_IncreaseInterruptMaskingLevel was called from within OS critical section.

	m_pImpl->PrintDetails();
	DBOS_printf("\nEmpty Count Semaphore:");
	m_pSemInterfaceEmptyCount->PrintDetails();
	DBOS_printf("\nFull Count Semaphore:");
	m_pSemInterfaceFullCount->PrintDetails();

	INTERRUPT_MASK_t const TestISRMask = DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask); // Exit ISR critical section
	ASSERT_FROM_OSCS(DBOS_Port_MaskAHigherOrEqualPriorityThanB(TestISRMask, ISR_MASK_MASK(m_pImpl->GetISRMask())) == TRUE)
	// Check ISR Mask Was Still Raised

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return;
}

void dBOS_QUEUEINTERFACE::PrintTypeAndID() const{

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	m_pImpl->PrintTypeAndID();

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

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
