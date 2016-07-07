/**
 * @file dBOS_TaskInterface.cpp
 * @author David Broadhurst
 * @date 15/03/2015
 *
 * \brief Implementation of dBOS Task Interface.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_TaskInterface.hpp"
#include "dBOS_Task.hpp"
#include "dBOS_Kernel.hpp"
#include "dBOS_Port.hpp"
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
dBOS_KERNEL * dBOS_TASKINTERFACE::m_pK = NULLPTR;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS

dBOS_TASKINTERFACE::dBOS_TASKINTERFACE(dBOS_TASK * const pTask) :
		m_pImpl(pTask){

	m_pK = dBOS_KERNEL::GetInstance();

	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(ISRMASK_OSCRITICAL) == TRUE)

}

dBOS_TASKINTERFACE::dBOS_TASKINTERFACE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const UniqueSyncID, DBOS_TASK_PRIORITY_t const Priority, DBOS_PNTEQUIV_t const TaskStackSize, DBOS_TASKENTRY_FUNCTION_PNT_t const pTaskFunction,
		void const * const pTaskEntryData) :
		m_pImpl(new dBOS_TASK(ID, UniqueSyncID, Priority, TaskStackSize, new DBOS_STK_t[TaskStackSize], pTaskFunction, pTaskEntryData, this)){

	m_pK = dBOS_KERNEL::GetInstance();

	ASSERT_FROM_OSCS(DBOS_Port_CheckCurrentMaskLevel(ISRMASK_OSCRITICAL) == TRUE)
	// TODO Handle out of memory.

	ASSERT_FROM_OSCS(m_pImpl != NULLPTR)

	m_pImpl->SetStartUpState();

	m_pK->RegisterSyncObject(&m_pImpl->m_TaskTimer);

	m_pK->AddNewTaskToReadyList(m_pImpl->GetpReadyLink());

}

dBOS_TASKINTERFACE::~dBOS_TASKINTERFACE(){

}

DBOS_TASK_PRIORITY_t dBOS_TASKINTERFACE::GetCurrentPriority(void) const{

	DBOS_TASK_PRIORITY_t Result;

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	Result = m_pImpl->GetPriority();

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return Result;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

void dBOS_TASKINTERFACE::PrintDetails(void) const{

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	m_pImpl->PrintDetails();

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return;
}

void dBOS_TASKINTERFACE::PrintTypeAndID(void) const{

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	m_pImpl->PrintTypeAndID();

	DBOS_Port_ExitCritical(PrevCS); // Exit OS Critical Section

	return;
}

void dBOS_TASKINTERFACE::PrintFreeStackSpace(void) const{

	INTERRUPT_MASK_t const PrevCS = DBOS_Port_EnterCritical(); // Enter OS Critical Section

	m_pImpl->PrintFreeStackSpace();

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
