/**
 * @file dBOS_Port.hpp
 * @author David Broadhurst
 * @date 9/02/2014
 *
 * \brief Definition of interface to Port specific services.
 *
 */

#ifndef DBOS_PORT_HPP_
#define DBOS_PORT_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_DataTypes.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES
/**
 * \brief Takes an ISR mask in CMSIS convention and converts it to what it would be once read back from mask register and converted back to CMSIS convention.
 *
 * \attention Be sure to change this to suit CPU.
 */
#define ISR_MASK_MASK(A) ((A) & 0x0FU)

// -------------------------------------------------------------------------------------------------
// DATA TYPES

// -------------------------------------------------------------------------------------------------
// CONSTANTS
/**
 * \brief Value to fill task stack spaces with when the stack space is created.
 */
DBOS_STK_t const DBOS_STACKWATERMARK = 0xDBDBDBDBU;

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// CALLBACK FUNCTIONS

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
void DBOS_Port_Init(void);

INTERRUPT_MASK_t DBOS_Port_EnterCritical_FromISR(void);
void DBOS_Port_ExitCritical_FromISR(INTERRUPT_MASK_t const PrevCSState);

INTERRUPT_MASK_t DBOS_Port_EnterCritical(void);
void DBOS_Port_ExitCritical(INTERRUPT_MASK_t const PrevCSState);

INTERRUPT_MASK_t DBOS_Port_GetInterruptMaskingLevel(void);

BOOL DBOS_Port_CheckCurrentMaskLevel(INTERRUPT_MASK_t const Mask);

INTERRUPT_MASK_t DBOS_Port_IncreaseInterruptMaskingLevel(INTERRUPT_MASK_t NewMaskLevel);
INTERRUPT_MASK_t DBOS_Port_DecreaseInterruptMaskingLevel(INTERRUPT_MASK_t NewMaskLevel);

DBOS_STKPTR_t DBOS_Port_InitTaskStack(DBOS_STKPTR_t const StackSpaceBottom, DBOS_PNTEQUIV_t const StackSize, DBOS_TASKENTRY_FUNCTION_PNT_t EntryPnt, void const * const TaskData);

UNATIVECPU_t DBOS_Port_CountFreeStackSpaces(DBOS_STK_t const * const StackSpaceBottom, DBOS_PNTEQUIV_t const StackSize);

#if !defined(__DOXYGEN__)
__attribute__(( naked ))
#else
#error // DON'T BUILD
#endif
void DBOS_Port_StartIdleTask(DBOS_STKPTR_t const * const pCurrentStackPointer, DBOS_STKPTR_t IdleTaskStackkSpaceBottom, DBOS_PNTEQUIV_t const StackSize,
		DBOS_TASKENTRY_FUNCTION_PNT_t const IdleTaskEntryPnt, void const * const pTaskData);

void DBOS_Port_SwCntxtSw(void);

void DBOS_Port_SetPendContextSwitch(void);

} // namespace

#endif /* DBOS_PORT_H_ */
