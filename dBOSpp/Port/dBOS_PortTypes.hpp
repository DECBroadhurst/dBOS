/**
 * @file dBOS_PortTypes.hpp
 * @author David Broadhurst
 * @date 5/02/2014
 *
 * \brief Provides port dependent type definitions.
 *
 */

#ifndef DBOS_PORTTYPES_H_
#define DBOS_PORTTYPES_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "DataTypes.h"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// DATA TYPES
/**
 * \brief Type used for task priorities.
 */
typedef INT8U DBOS_TASK_PRIORITY_t;

/**
 * \brief Type used for counting OS ticks.
 */
typedef INT32S DBOS_TIMECOUNT_t;				// Must be signed type. // TODO CHANGE TO UNSIGNED TYPE 0xFFFFFFFF = WAIT FOREVER

/**
 * \brief Type used for data passed with synchronisation object signalling.
 */
typedef INT32U SIGNALLED_DATA_t;

/**
 * \brief Type that is equivalent to a stack pointer.
 */
typedef UNATIVECPU_t const * DBOS_STKPTR_t;		// Stack Pointer.

/**
 * \brief Type that is equivalent to an element on the stack.
 */
typedef UNATIVECPU_t DBOS_STK_t;				// Element on stack.

/**
 * \brief Unsigned integer of same width as a pointer.
 */
typedef INT32U DBOS_PNTEQUIV_t;

/**
 * \brief Interrupt mask type.
 */
typedef UNATIVECPU_t INTERRUPT_MASK_t;

// -------------------------------------------------------------------------------------------------
// CONSTANTS
/**
 * \brief Time value to indicate wait forever.
 */
DBOS_TIMECOUNT_t const TIMEWAITFOREVER = -1;

/**
 * \brief Width of a native word in bits, 32 for a 32 bit machine.
 */
#define CPU_BITS_PER_WORD 32U

/**
 * \brief Represents an invalid task priority.
 *
 * @see DBOS_TASK_PRIORITY_t
 */
DBOS_TASK_PRIORITY_t const INVALID_PRIORITY_LEVEL = 0xFFU;

/**
 * \brief Used when passing no data when signalling an object.
 *
 * @see SIGNALLED_DATA_t
 */
SIGNALLED_DATA_t const SIGNALDATANONE = 0U;

/**
 * \brief Priority of the idle task. <b>Must be lowest possibly priority</b>.
 *
 * @see DBOS_TASK_PRIORITY_t
 */
DBOS_TASK_PRIORITY_t const IDLETASKPRIORITY = 0U;

/**
 * \brief Interrupt mask to enable all interrupts.
 */
INTERRUPT_MASK_t const ISRMASK_NONE = 0U;

/**
 * \brief Interrupt mask to disable all interrupts.
 *
 * \note disables all interrupts aside from SVCall
 */
INTERRUPT_MASK_t const ISRMASK_ALL = 0x01U;

/**
 * \brief Interrupt mask to disable PendSV Handler.
 */
INTERRUPT_MASK_t const ISRMASK_OSCRITICAL = 0xFFU;

/**
 * \brief Interrupt mask to disable OS tick timer interrupt (and lower priority interrupts).
 */
INTERRUPT_MASK_t const ISRMASK_OSTIMER = (0xFFU - 1U);

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S
/**
 * \brief Compilier specific beginning of assembly code block.
 */
#define PORT_ASM_VOLATILE __asm volatile

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

}// namespace

#endif /* DBOS_PORTTYPES_H_ */
