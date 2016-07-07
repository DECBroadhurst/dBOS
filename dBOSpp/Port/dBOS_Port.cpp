/**
 * @file dBOS_Port.cpp
 * @author David Broadhurst
 * @date 9/02/2014
 *
 * \brief Implementation of compiler and architecture specific services.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Port.hpp"
#include "dBOS_PortFunctions.hpp"
#include "dBOS_Config.hpp"
#include "MemoryBarrier.h"
#include "BitBand.h"
#include "dBOS_Assert.hpp"
#include "dBOS_TimerManager.hpp"
#include "dBOS_Kernel.hpp"
#include "Device.h"

//	- Info 752: local declarator '__StackTop' (line 25, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) not referenced [MISRA C++ Rule 0-1-3]
//	- Note 1960: Violates MISRA C++ 2008 Required Rule 17-0-2, Re-use of C++ identifier pattern: __StackTop
//	- Note 1960: Violates MISRA C++ 2008 Required Rule 7-3-1, Global declaration of symbol '__StackTop'
// This is referenced in assembly code below, it has to be outside of the namespace.
/**
 * \brief Initial value for stack pointer.
 */
extern void * _estack;

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// LOCAL DEFINES
// - Note 923: cast from unsigned int to pointer [MISRA C++ Rule 5-2-7], [MISRA C++ Rule 5-2-8]
// - Note 1924: C-style cast -- More Effective C++ #2, [MISRA C++ Rule 5-2-4]
//lint -emacro((923), SET_CONTEXT_SWITCH_PENDING)
//lint -emacro((1924), SET_CONTEXT_SWITCH_PENDING)
/**
 * \brief Sets the PendSV exception handler pending.
 */
#define SET_CONTEXT_SWITCH_PENDING 	*((volatile INT32U * ) 0xE000ED04U) = 0x10000000U;

// -------------------------------------------------------------------------------------------------
// LOCAL MACROS

// -------------------------------------------------------------------------------------------------
// LOCAL CONSTANTS

// -------------------------------------------------------------------------------------------------
// EXTERNAL CONSTANTS

// -------------------------------------------------------------------------------------------------
// LOCAL DATA TYPES

// -------------------------------------------------------------------------------------------------
// LOCAL TABLES

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES

// Warning 551: Symbol 'dBOS::S_DBOS_pCurrentStackPointer' (line 60, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) not accessed [MISRA C++ Rule 0-1-4]
// It is accessed in assembly code.
//lint -esym(551, dBOS::S_DBOS_pCurrentStackPointer)
/**
 * \brief Address where current task's stack pointer is saved.
 */
static DBOS_STKPTR_t const * S_DBOS_pCurrentStackPointer = NULLPTR;

/**
 * \brief Pointer to the Kernel Object Instance.
 *
 * @see dBOS_KERNEL
 */
static dBOS_KERNEL *S_pKernel = NULLPTR;

/**
 * \brief Pointer to the Timer Manager Object Instance.
 *
 * @see dBOS_TIMERMANAGER
 */
static dBOS_TIMERMANAGER *S_pTimerManager = NULLPTR;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

#if (DBOS_PREEMPTIONENABLED == 1U)
#if (DBOS_USE_TIMESLICING == 1U)
/**
 * \brief Used to count number of OS timer ticks.
 *
 * Counts down from DBOS_USE_MAX_TIME_SLICE. When the count reaches zero a context switch is set pending.
 * The count is reset to DBOS_USE_MAX_TIME_SLICE each time a context switch occurs.
 */
static UNATIVECPU_t S_TimeSliceCount = DBOS_USE_MAX_TIME_SLICE;
#endif
#endif

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES
//lint -esym(765, CB_DBOS_Task_Core_SelectNextTaskFromISR)
//lint -esym(714, CB_DBOS_Task_Core_SelectNextTaskFromISR)
//lint -esym(765, CB_DBOS_Task_Core_GetAddressOfNewStackPointer)
//lint -esym(714, CB_DBOS_Task_Core_GetAddressOfNewStackPointer)
// - Info 765: external 'CB_DBOS_Task_Core_SelectNextTaskFromISR(void)' (line 94, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) could be made static [MISRA C++ Rule 3-3-1]
// - Info 714: Symbol 'CB_DBOS_Task_Core_SelectNextTaskFromISR(void)' (line 94, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) not referenced [MISRA C++ Rule 0-1-3], [MISRA C++ Rule 0-1-10]
// - Info 765: external 'CB_DBOS_Task_Core_GetAddressOfNewStackPointer(void)' (line 92, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) could be made static [MISRA C++ Rule 3-3-1]
// - Info 714: Symbol 'CB_DBOS_Task_Core_GetAddressOfNewStackPointer(void)' (line 92, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) not referenced [MISRA C++ Rule 0-1-3], [MISRA C++ Rule 0-1-10]
// CB_DBOS_Task_Core_SelectNextTaskFromISR and CB_DBOS_Task_Core_GetAddressOfNewStackPointer are referenced in assembly code and therefore can not be made static.
extern "C" {
// Called from within assembly code.
DBOS_STKPTR_t const * CB_DBOS_Task_Core_SelectNextTaskFromISR(void); 		// CALLED FROM WITHIN PendSV HANDLER
DBOS_STKPTR_t const * CB_DBOS_Task_Core_GetAddressOfNewStackPointer(void);	// CALLED FROM WITHIN SVC HANDLER
}

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS

/**
 * \brief Performs port specific hardware setup at start up.
 *
 * @return NONE
 */
void DBOS_Port_Init(void){

	S_pKernel = dBOS_KERNEL::GetInstance();
	S_pTimerManager = dBOS_TIMERMANAGER::GetInstance();

	NVIC_SetPriorityGrouping(0U);				// Sub-priority set to bit 0 only, sub-priority feature therefore not used since
												// only 4 MSbs of priority registers are implemented on STM32

	NVIC_SetPriority(PendSV_IRQn, 0xFF);		// Lowest priority level in the system.
												// NVIC_SetPriority function sets the hardware register to (priority << (8 - __NVIC_PRIO_BITS))

	NVIC_SetPriority(SVCall_IRQn, 0);			// Highest priority level in the system.
												// NVIC_SetPriority function sets the hardware register to (priority << (8 - __NVIC_PRIO_BITS))

	NVIC_SetPriority(TIM2_IRQn, ISRMASK_OSTIMER);	// Next priority level above PendSV exception
													// NVIC_SetPriority function sets the hardware register to (priority << (8 - __NVIC_PRIO_BITS))

	// All other IRQs should have priority levels above that of system tick timer.

	NVIC_EnableIRQ(TIM2_IRQn);

	//lint -save -e1960 -e923 -e1924 -e835
	// Suppress lint messages due to use of CMSIS here.
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

	TIM2->PSC = 71U;

	TIM2->ARR = 1000U - 1U;

	TIM2->CR2 = 0U;

	TIM2->DIER = TIM_DIER_UIE;

	TIM2->CCMR1 = 0U;
	TIM2->CCMR2 = 0U;

	TIM2->CR1 = TIM_CR1_ARPE;
	TIM2->CR1 |= TIM_CR1_CEN;

	TIM2->EGR |= TIM_EGR_UG;

	//lint -restore

}

//lint -efunc(530, dBOS::DBOS_Port_EnterCritical_FromISR)	Warning 530: Symbol 'CurrentMaskLevel' not initialized [MISRA 2004 Rule 9.1, required]
//													Symbol is initialized in assembly code.
//lint -efunc(438, dBOS::DBOS_Port_EnterCritical_FromISR)	Warning 438: Last value assigned to variable 'NewMaskLevel' not used
//													Assigned value is used in assembly code.
//lint -efunc(550, dBOS::DBOS_Port_EnterCritical_FromISR)	Warning 550: Symbol 'NewMaskLevel' not accessed
//													NewMaskLevel is accessed in assembly code.
/**
 * \brief Raises interrupt priority masking (if not raised already) to enable atomic access to OS. <b>To be called from ISR(s) only</b>.
 *
 * For the Cortex M port the PendSV_Handler() Handler needs to be the only one disabled when the priority mask level is raised.
 *
 * \note Return value uses the CMSIS convention for priority values.
 *
 * @return Priority masking level prior to call to this function.
 * @see DBOS_Port_ExitCritical_FromISR
 * @see PendSV_Handler
 */
INTERRUPT_MASK_t DBOS_Port_EnterCritical_FromISR(void){

	INTERRUPT_MASK_t NewMaskLevel;

	NewMaskLevel = ISRMASK_OSCRITICAL;

	NewMaskLevel <<= (8U - __NVIC_PRIO_BITS);

	NewMaskLevel = NewMaskLevel & 0xFFU;

	INTERRUPT_MASK_t CurrentMaskLevel;

	PORT_ASM_VOLATILE
	(
			"	MRS %[I_Reg], BASEPRI				\n"

			: [I_Reg] "=r" (CurrentMaskLevel)		// Outputs
			:// Inputs
			:// Clobber List
	)
	;

	CurrentMaskLevel = CurrentMaskLevel & 0xFFU;

	PORT_ASM_VOLATILE
	(
			"	MSR BASEPRI_MAX, %[D_Reg]			\n" 	// Use CPU BASEPRI_MAX feature to selectively write only higher masking levels to BASEPRI
			:// Outputs
			: [D_Reg] "r" (NewMaskLevel)// Inputs
			:// Clobber List
	)
	;

	MEMORY_BARRIER

	CurrentMaskLevel >>= (8U - __NVIC_PRIO_BITS);

	return CurrentMaskLevel;
}

//lint -efunc(530, dBOS::DBOS_Port_ExitCritical_FromISR) 	Warning 530: Symbol 'CurrentMaskLevel' not initialized [MISRA 2004 Rule 9.1, required]
//															Symbol is initialized in assembly code.
/**
 * \brief Restores interrupt masking to state it was prior to call to DBOS_Port_EnterCritical_FromISR. <b>To be called from ISR(s) only</b>.
 *
 * \note Argument uses the CMSIS convention for priority values.
 *
 * @param[in] PrevCSState should be return value from previous call to DBOS_Port_EnterCritical_FromISR
 * @return NONE
 * @see DBOS_Port_EnterCritical_FromISR
 */
void DBOS_Port_ExitCritical_FromISR(INTERRUPT_MASK_t const PrevCSState){

	MEMORY_BARRIER

	INTERRUPT_MASK_t NewMaskLevel;

	NewMaskLevel = PrevCSState;

	NewMaskLevel <<= (8U - __NVIC_PRIO_BITS);

	NewMaskLevel = NewMaskLevel & 0xFFU;

	INTERRUPT_MASK_t CurrentMaskLevel;

	PORT_ASM_VOLATILE
	(
			"	MRS %[I_Reg], BASEPRI				\n"

			: [I_Reg] "=r" (CurrentMaskLevel)		// Outputs
			:// Inputs
			:// Clobber List
	)
	;

	CurrentMaskLevel = CurrentMaskLevel & 0xFFU;

	ASSERT_FROM_OSCS(DBOS_Port_MaskALowerOrEqualPriorityThanB( (NewMaskLevel >> (8U - __NVIC_PRIO_BITS)), (CurrentMaskLevel >> (8U - __NVIC_PRIO_BITS))) == 1U)

	ASSERT_FROM_OSCS((CurrentMaskLevel >> (8U - __NVIC_PRIO_BITS)) != ISR_MASK_MASK(ISRMASK_NONE))

	PORT_ASM_VOLATILE
	(
			"	MSR BASEPRI, %[D_Reg]				\n"
			:										// Outputs
			: [D_Reg] "r" (NewMaskLevel)// Inputs
			:// Clobber List
	)
	;

}

//lint -efunc(530, dBOS::DBOS_Port_EnterCritical)	Warning 530: Symbol 'CurrentMaskLevel' not initialized [MISRA 2004 Rule 9.1, required]
//													Symbol is initialized in assembly code.
//lint -efunc(438, dBOS::DBOS_Port_EnterCritical)	Warning 438: Last value assigned to variable 'NewMaskLevel' not used
//													Assigned value is used in assembly code.
//lint -efunc(550, dBOS::DBOS_Port_EnterCritical)	Warning 550: Symbol 'NewMaskLevel' not accessed
//													NewMaskLevel is accessed in assembly code.
/**
 * \brief Raises interrupt priority masking (if not raised already) to enable atomic access to OS.
 *
 * \note For the Cortex M port the PendSV_Handler() Handler needs to be the only one disabled when the priority mask level is raised.
 *
 * \note Return value uses the CMSIS convention for priority values.
 *
 * @return Priority masking level prior to call to this function.
 * @see DBOS_Port_ExitCritical
 * @see PendSV_Handler
 */
INTERRUPT_MASK_t DBOS_Port_EnterCritical(void){

	INTERRUPT_MASK_t NewMaskLevel;

	NewMaskLevel = ISRMASK_OSCRITICAL;

	NewMaskLevel <<= (8U - __NVIC_PRIO_BITS);

	NewMaskLevel = NewMaskLevel & 0xFFU;

	INTERRUPT_MASK_t CurrentMaskLevel;

	PORT_ASM_VOLATILE
	(
			"	MRS %[I_Reg], BASEPRI				\n"

			: [I_Reg] "=r" (CurrentMaskLevel)		// Outputs
			:// Inputs
			:// Clobber List
	)
	;

	CurrentMaskLevel = CurrentMaskLevel & 0xFFU;

	PORT_ASM_VOLATILE
	(
			"	MSR BASEPRI_MAX, %[D_Reg]			\n" 	// Use CPU BASEPRI_MAX feature to selectively write only higher masking levels to BASEPRI
			:// Outputs
			: [D_Reg] "r" (NewMaskLevel)// Inputs
			:// Clobber List
	)
	;

	CurrentMaskLevel >>= (8U - __NVIC_PRIO_BITS);

	MEMORY_BARRIER

	return CurrentMaskLevel;
}

//lint -efunc(530, dBOS::DBOS_Port_ExitCritical)	Warning 530: Symbol 'CurrentMaskLevel' not initialized [MISRA 2004 Rule 9.1, required]
//													Symbol is initialized in assembly code.
/**
 * \brief Restores interrupt masking to state it was prior to call to DBOS_Port_EnterCritical.
 *
 * \note Argument uses the CMSIS convention for priority values.
 *
 * @param[in] PrevCSState should be return value from previous call to DBOS_Port_EnterCritical
 * @return NONE
 * @see DBOS_Port_EnterCritical
 */
void DBOS_Port_ExitCritical(INTERRUPT_MASK_t const PrevCSState){

	MEMORY_BARRIER

	INTERRUPT_MASK_t NewMaskLevel;

	NewMaskLevel = PrevCSState;

	NewMaskLevel <<= (8U - __NVIC_PRIO_BITS);

	NewMaskLevel = NewMaskLevel & 0xFFU;

	INTERRUPT_MASK_t CurrentMaskLevel;

	PORT_ASM_VOLATILE
	(
			"	MRS %[I_Reg], BASEPRI				\n"

			: [I_Reg] "=r" (CurrentMaskLevel)		// Outputs
			:// Inputs
			:// Clobber List
	)
	;

	CurrentMaskLevel = CurrentMaskLevel & 0xFFU;

	ASSERT_FROM_OSCS(DBOS_Port_MaskALowerOrEqualPriorityThanB( (NewMaskLevel >> (8U - __NVIC_PRIO_BITS)), (CurrentMaskLevel >> (8U - __NVIC_PRIO_BITS))) == 1U)

	ASSERT_FROM_OSCS((CurrentMaskLevel >> (8U - __NVIC_PRIO_BITS)) != ISR_MASK_MASK(ISRMASK_NONE))

	PORT_ASM_VOLATILE
	(
			"	MSR BASEPRI, %[D_Reg]				\n"
			:										// Outputs
			: [D_Reg] "r" (NewMaskLevel)// Inputs
			:// Clobber List
	)
	;

	// Should have been in critical section when this was called.
}

//lint -efunc(530, dBOS::DBOS_Port_GetInterruptMaskingLevel) 	Warning 530: Symbol 'CurrentMaskLevel' not initialized [MISRA 2004 Rule 9.1, required]
//																Symbol is initialized in assembly code.
/**
 * \brief Returns the current interrupt priority mask level.
 *
 * \note Return value uses the CMSIS convention for priority.
 *
 * @return Current interrupt priority mask level.
 */
INTERRUPT_MASK_t DBOS_Port_GetInterruptMaskingLevel(void){

	INTERRUPT_MASK_t CurrentMaskLevel;

	PORT_ASM_VOLATILE
	(
			"	MRS %[I_Reg], BASEPRI				\n"

			: [I_Reg] "=r" (CurrentMaskLevel)		// Outputs
			:// Inputs
			:// Clobber List
	)
	;

	CurrentMaskLevel = CurrentMaskLevel & 0xFFU;

	CurrentMaskLevel >>= (8U - __NVIC_PRIO_BITS);

	return CurrentMaskLevel;
}

//lint -efunc(530, dBOS::DBOS_Port_CheckCurrentMaskLevel)	Warning 530: Symbol 'CurrentMaskLevel' not initialized [MISRA 2004 Rule 9.1, required]
//															Symbol is initialized in assembly code.
/**
 * \brief Check current ISR masking level is equal to or greater than the supplied mask level.
 *
 * @param[in] Mask Level to check current masking level against.
 * @retval TRUE Current ISR mask level is greater than or equal to Mask argument.
 * @retval FALSE Current ISR mask level is less than the Mask argument.
 */
BOOL DBOS_Port_CheckCurrentMaskLevel(INTERRUPT_MASK_t const Mask){

	INTERRUPT_MASK_t CurrentMaskLevel;

	PORT_ASM_VOLATILE
	(
			"	MRS %[I_Reg], BASEPRI				\n"

			: [I_Reg] "=r" (CurrentMaskLevel)		// Outputs
			:// Inputs
			:// Clobber List
	)
	;

	CurrentMaskLevel = CurrentMaskLevel & 0xFFU;

	CurrentMaskLevel >>= (8U - __NVIC_PRIO_BITS);

	return DBOS_Port_MaskAHigherOrEqualPriorityThanB(CurrentMaskLevel, ISR_MASK_MASK(Mask));
}

//lint -efunc(530, dBOS::DBOS_Port_IncreaseInterruptMaskingLevel)	Warning 530: Symbol 'CurrentMaskLevel' not initialized [MISRA 2004 Rule 9.1, required]
//																	Symbol is initialized in assembly code.
//lint -efunc(438, dBOS::DBOS_Port_IncreaseInterruptMaskingLevel)	Warning 438: Last value assigned to variable 'NewMaskLevel' not used
//																	Assigned value is used in assembly code.
/**
 * \brief Changes ISR Priority masking level ONLY if argument NewMaskLevel is of a higher priority than currently set masking level.
 *
 * Changes ISR Masking level ONLY if NewMaskLevel is higher (will disable interrupts of a higher priority) than currently set priority masking level.\n
 * Interrupts with priority equal to or less than NewMaskLevel will be disabled.\n
 *
 * \note Argument and return value uses the CMSIS convention for priority.
 *
 * @param[in] NewMaskLevel Interrupt priority level to mask, interrupts with priority equal to or less than NewMaskLevel will be disabled.
 *
 * @return Interrupt priority masking level prior to call to this function.
 */
INTERRUPT_MASK_t DBOS_Port_IncreaseInterruptMaskingLevel(INTERRUPT_MASK_t NewMaskLevel){

	NewMaskLevel <<= (8U - __NVIC_PRIO_BITS);

	NewMaskLevel = NewMaskLevel & 0xFFU;

	INTERRUPT_MASK_t CurrentMaskLevel;

	PORT_ASM_VOLATILE
	(
			"	MRS %[I_Reg], BASEPRI				\n"

			: [I_Reg] "=r" (CurrentMaskLevel)		// Outputs
			:// Inputs
			:// Clobber List
	)
	;

	CurrentMaskLevel = CurrentMaskLevel & 0xFFU;

	PORT_ASM_VOLATILE
	(
			"	MSR BASEPRI_MAX, %[D_Reg]			\n" 	// Use CPU BASEPRI_MAX feature to selectively write only higher masking levels to BASEPRI
			:// Outputs
			: [D_Reg] "r" (NewMaskLevel)// Inputs
			:// Clobber List
	)
	;

	MEMORY_BARRIER

	CurrentMaskLevel >>= (8U - __NVIC_PRIO_BITS);

	return CurrentMaskLevel;
}

//lint -efunc(530, dBOS::DBOS_Port_DecreaseInterruptMaskingLevel)	Warning 530: Symbol 'CurrentMaskLevel' not initialized [MISRA 2004 Rule 9.1, required]
//																	Symbol is initialized in assembly code.
/**
 * \brief Sets ISR Masking priority level to the specified priority level. This must be lower or equal to the currently set priority mask.
 *
 * \note Argument uses the CMSIS convention for priority.
 *
 * @param[in] NewMaskLevel Interrupt priority level to mask, interrupts with priority equal to or less than NewMaskLevel will be disabled.
 *
 * @return Interrupt priority masking level prior to call to this function.
 */
INTERRUPT_MASK_t DBOS_Port_DecreaseInterruptMaskingLevel(INTERRUPT_MASK_t NewMaskLevel){

	MEMORY_BARRIER

	NewMaskLevel <<= (8U - __NVIC_PRIO_BITS);

	NewMaskLevel = NewMaskLevel & 0xFFU;

	INTERRUPT_MASK_t CurrentMaskLevel;

	PORT_ASM_VOLATILE
	(
			"	MRS %[I_Reg], BASEPRI				\n"

			: [I_Reg] "=r" (CurrentMaskLevel)		// Outputs
			:// Inputs
			:// Clobber List
	)
	;

	CurrentMaskLevel = CurrentMaskLevel & 0xFFU;

	ASSERT_FROM_OSCS(DBOS_Port_MaskALowerOrEqualPriorityThanB( (NewMaskLevel >> (8U - __NVIC_PRIO_BITS)), (CurrentMaskLevel >> (8U - __NVIC_PRIO_BITS))) == 1U)

	PORT_ASM_VOLATILE
	(
			"	MSR BASEPRI, %[D_Reg]				\n"
			:										// Outputs
			: [D_Reg] "r" (NewMaskLevel)// Inputs
			:// Clobber List
	)
	;

	CurrentMaskLevel >>= (8U - __NVIC_PRIO_BITS);

	return CurrentMaskLevel;
}

/**
 * \brief Initialises task stack space.
 *
 * @param[in] StackSpaceBottom Address of memory to use as stack space, pointer to word in stack space with lowest address in memory.
 * @param[in] StackSize Number of words in allocated stack space.
 * @param[in] EntryPnt Pointer to task function.
 * @param[in] TaskData Data to pass to task function.
 *
 * @return Stack pointer after initialisation of the stack space.
 */
DBOS_STKPTR_t DBOS_Port_InitTaskStack(DBOS_STKPTR_t const pStackSpaceBottom, DBOS_PNTEQUIV_t const StackSize, DBOS_TASKENTRY_FUNCTION_PNT_t EntryPnt, void const * const TaskData){

	DBOS_STK_t * Pnt;
	DBOS_STK_t * StkTop;

	StkTop = (DBOS_STK_t *) (pStackSpaceBottom + (StackSize - 1U));
	// CAST HERE ALLOWS THIS ROUTINE TO WRITE TO STACK SPACE

	// Registers stacked as if saved on exception
	*StkTop = static_cast<INT32U>(0x01000000); 	//xPSR

	StkTop--;
	*StkTop = (INT32U) (EntryPnt); 	// PC
	StkTop--;
	*StkTop = static_cast<INT32U>(0xFFFFFFFEU); // R14 (LR)	//Invalid EXC_RETURN Value as bit 0 is 0.
	StkTop--;
	*StkTop = static_cast<INT32U>(0x12121212); 	// R12
	StkTop--;
	*StkTop = static_cast<INT32U>(0x03030303); 	// R3
	StkTop--;
	*StkTop = static_cast<INT32U>(0x02020202); 	// R2
	StkTop--;
	*StkTop = static_cast<INT32U>(0x01010101); 	// R1
	StkTop--;
	*StkTop = (INT32U) (TaskData); 	// R0 is argument
	StkTop--;
	// Remaining registers saved on process stack
	*StkTop = static_cast<INT32U>(0x11111111); 	// R11
	StkTop--;
	*StkTop = static_cast<INT32U>(0x10101010); 	// R10
	StkTop--;
	*StkTop = static_cast<INT32U>(0x09090909); 	// R9
	StkTop--;
	*StkTop = static_cast<INT32U>(0x08080808); 	// R8
	StkTop--;
	*StkTop = static_cast<INT32U>(0x07070707); 	// R7
	StkTop--;
	*StkTop = static_cast<INT32U>(0x06060606); 	// R6
	StkTop--;
	*StkTop = static_cast<INT32U>(0x05050505); 	// R5
	StkTop--;
	*StkTop = static_cast<INT32U>(0x04040404); 	// R4

	StkTop--;
	*StkTop = static_cast<INT32U>(0x00000000); 	// BASEPRI
	// BASEPRI is used to enable  / disable PendSV handler and therefore pre-emptive context switching.
	// It is saved as part of a task's context.
	// Initialised to 0x00 = no interrupt masking.

	Pnt = StkTop;
	Pnt--;

	while(Pnt > pStackSpaceBottom){
		*Pnt = DBOS_STACKWATERMARK;	// Fill Out rest of stack with StackWaterMark
		Pnt--;
	}

	*Pnt = DBOS_STACKWATERMARK;

	return StkTop;
}

/**
 * \brief Uses watermark technique to determine unused words in stack space.
 *
 * @param[in] StackSpaceBottom Address of memory to use as stack space, pointer to word in stack space with lowest address in memory.
 * @param[in] StackSize Total number of words in allocated stack space.
 *
 * @return Number of unused words in stack space.
 */
UNATIVECPU_t DBOS_Port_CountFreeStackSpaces(DBOS_STK_t const * const StackSpaceBottom, DBOS_PNTEQUIV_t const StackSize){

	UNATIVECPU_t Count;

	Count = 0U;

	while((StackSpaceBottom[Count] == DBOS_STACKWATERMARK) && (Count < StackSize)){
		Count++;
	}

	return Count;
}

//lint -efunc(550, dBOS::DBOS_Port_StartIdleTask)	Warning 550: Symbol 'IdleTaskStackkSpaceBottom' not accessed
//													IdleTaskStackkSpaceBottom is accessed in assembly code
//lint -efunc(715, dBOS::DBOS_Port_StartIdleTask)	Info 715: Symbol 'TaskData' not referenced
//													Info 715: Symbol 'IdleTaskEntryPnt' not referenced
//													TaskData and IdleTaskEntryPnt is referenced in assembly code
//lint -efunc(438, dBOS::DBOS_Port_StartIdleTask)	Warning 438: Last value assigned to variable 'IdleTaskStackkSpaceBottom' not used
//													IdleTaskStackkSpaceBottom is used in assembly code
/**
 * \brief Starts Idle Task - This function never returns.
 *
 * @param[in] pCurrentStackPointer Address to save idle task's stack pointer to on context switch.
 * @param[in] IdleTaskStackkSpaceBottom Address of memory to use as stack space for IDLE task, pointer to word in stack space with lowest address in memory.
 * @param[in] StackSize Number of words in allocated stack space for idle task.
 * @param[in] IdleTaskEntryPnt Pointer to idle task function.
 * @param[in] TaskData Data to pass to task function.
 *
 * @return This function never returns.
 */
#if !defined(__DOXYGEN__)
__attribute__(( naked ))
#else
#error // DON'T BUILD
#endif
void DBOS_Port_StartIdleTask(DBOS_STKPTR_t const * const pCurrentStackPointer, DBOS_STKPTR_t IdleTaskStackkSpaceBottom, DBOS_PNTEQUIV_t const StackSize, DBOS_TASKENTRY_FUNCTION_PNT_t const IdleTaskEntryPnt, void const * const pTaskData){

	S_DBOS_pCurrentStackPointer = pCurrentStackPointer;

	MEMORY_BARRIER
	// Memory barrier after write to shared variable.

	IdleTaskStackkSpaceBottom += StackSize;
	// IdleTaskStackkSpaceBottom is now pointing to top of stack, actually one word above as M3 will decrement the pointer then write to stack in that order.

	PORT_ASM_VOLATILE
	(

			"	MSR PSP, %[IdleSP]					\n"	// PSP = top of idle task stack space.

			"	MOV r12, #0x00000002				\n"// Switch to using process stack pointer.
			" 	MSR CONTROL, r12					\n"
			" 	ISB									\n"// This ensures that instructions after the ISB instruction execute using the new stack pointer

			" 	MSR MSP, %[e_stack]					\n"// Set MSP back to top of stack space, everything already stacked won't be used again, this function never returns.

			"	MOV r0, %[Data]						\n"// Data to be passed to task.

			" 	ORR %[EntryPnt], #0x1				\n"// LSb must be 1.
			"	BX %[EntryPnt]						\n"// Branch to idle task.

			:// Outputs
			: [IdleSP] "r" (IdleTaskStackkSpaceBottom), [EntryPnt] "r" (IdleTaskEntryPnt), [Data] "r" (pTaskData), [e_stack] "r" (&_estack)// Inputs
			: "r12", "cc"// Clobber List
	)
	;

}

/**
 * \brief Implements immediate context switch from within OS critical section.
 *
 * Context switch routine calls CB_DBOS_Task_Core_GetAddressOfNewStackPointer to determine task to switch to.
 *
 * @return Returns when context is eventually switched back to calling task.
 */
void DBOS_Port_SwCntxtSw(void){

	PORT_ASM_VOLATILE
	(
			" 	SVC 0							\n"
	)
	;

	return;
}

/**
 * \brief Sets pending the processing of any flags set by ISR(s) AND a preemptive context switch (if enabled)
 *
 * Sets an ISR pending. The ISR must run CB_DBOS_Task_Core_ProcessISRFlags() and may also perform a context switch if preemptive switching is enabled.\n
 * The ISR should run as soon as the OS exits the OS critical section (assuming no other ISRs are pending in which case they should run first (be of higher priority)).
 *
 * @return NONE
 */
void DBOS_Port_SetPendContextSwitch(void){

	SET_CONTEXT_SWITCH_PENDING

	return;
}

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
//lint -esym(1784, CB_DBOS_Task_Core_SelectNextTaskFromISR)
// Info 1784: Symbol 'CB_DBOS_Task_Core_SelectNextTaskFromISR(void)' previously declared as "C", compare with line 97
/**
 * \brief Called from assembly code. <b>CALLED FROM WITHIN PendSV HANDLER</b>
 *
 * @return Address of the stack pointer of the task to switch to.
 */
DBOS_STKPTR_t const * CB_DBOS_Task_Core_SelectNextTaskFromISR(void){

	return S_pKernel->SelectNextTaskFromISR();
}

//lint -esym(1784, CB_DBOS_Task_Core_GetAddressOfNewStackPointer)
// Info 1784: Symbol 'CB_DBOS_Task_Core_GetAddressOfNewStackPointer(void)' previously declared as "C", compare with line 98
/**
 * \brief Called from assembly code. <b>CALLED FROM WITHIN SVC HANDLER</b>
 *
 * @return Address of the stack pointer of the task to switch to.
 */
DBOS_STKPTR_t const * CB_DBOS_Task_Core_GetAddressOfNewStackPointer(void){

	return S_pKernel->GetAddressOfNewStackPointer();
}

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
extern "C" {

// - Info 765: external 'SVC_Handler(void)' (line 719, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) could be made static [MISRA C++ Rule 3-3-1]
// - Info 714: Symbol 'SVC_Handler(void)' (line 719, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) not referenced [MISRA C++ Rule 0-1-3], [MISRA C++ Rule 0-1-10]
//lint -esym(765, SVC_Handler)
//lint -esym(714, SVC_Handler)
/**
 * \brief Context switch routine triggered immediately by call to DBOS_Port_SwCntxtSw()
 *
 * Must have a priority one level higher than PendSV_Handler() and should be the only routine on that priority level.
 *
 * @see DBOS_Port_SwCntxtSw()
 * @see PendSV_Handler()
 */
#if !defined(__DOXYGEN__)
__attribute__(( naked ))
#else
#error // DON'T BUILD
#endif
void SVC_Handler(){ //lint !e963 // - Note 963: Qualifier const or volatile precedes a type; use +fqb to reverse the test

#if (DBOS_PREEMPTIONENABLED == 1U)
#if (DBOS_USE_TIMESLICING == 1U)
	S_TimeSliceCount = DBOS_USE_MAX_TIME_SLICE; // Assume this is an atomic operation
#endif
#endif

	PORT_ASM_VOLATILE
	(
			// Currently using process stack, PSR, PC, LR, R12, R3, R2, R1, R0 already pushed onto stack.
			"	MRS r12, PSP 						\n"// Recover PSP into R12
			"	STMDB r12!, {r4-r11} 				\n"// Push remaining (non-stack) registers onto the stack.

			" 	MRS r1, BASEPRI						\n"// Save BASEPRI register
			"	STMDB r12!, {r1} 					\n"

			"	LDR r1, [%[PtrPtrCurrSp]]			\n"// r1 = PtrCurrSp;;
			"	STR r12, [r1]						\n"// *PtrCurrSp = SP;

			"	PUSH {%[PtrPtrCurrSp]}				\n"

			"	BL CB_DBOS_Task_Core_GetAddressOfNewStackPointer	\n"// Return value is in r0 and is location of stack pointer to restore.

			"	POP {%[PtrPtrCurrSp]}				\n"

			"	STR r0, [%[PtrPtrCurrSp]]			\n"// Location of current stack pointer updated to stack pointer this is switching to.

			"	LDR r12, [r0]						\n"// r12 = NewSP

			" 	LDMIA r12!, {r1}					\n"
			"	MSR BASEPRI, r1						\n"// Restore BASEPRI register

			" 	LDMIA r12!, {R4-R11}				\n"// Restore non-stacked registers.
			"	MSR PSP, r12						\n"// PSP = r12
			" 	ISB									\n"// This ensures that instructions after the ISB instruction execute using the new stack pointer

			" 	MOV LR, #0xFFFFFFFD					\n"// Set EXC_RETURN value, Return to Thread Mode, and on return use the process Stack.
			" 	BX LR								\n"// Execute Exception Return.

			:// Outputs
			: [PtrPtrCurrSp] "r" (&S_DBOS_pCurrentStackPointer)// Inputs
			: "r0", "r1", "r12", "cc"// Clobber List
	)
	;

	return;
}

// - Info 765: external 'PendSV_Handler(void)' (line 785, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) could be made static [MISRA C++ Rule 3-3-1]
// - Info 714: Symbol 'PendSV_Handler(void)' (line 785, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) not referenced [MISRA C++ Rule 0-1-3], [MISRA C++ Rule 0-1-10]
//lint -esym(765, PendSV_Handler)
//lint -esym(714, PendSV_Handler)
/**
 * \brief Routine which is set pending by call to DBOS_Port_SetPendContextSwitch()
 *
 * This routine should run as soon as the OS exits the OS critical section and all other higher priority routines have run.\n
 * This should be the lowest priority routine in the system.\n
 * CB_DBOS_Task_Core_ProcessISRFlags() should be called as the first action in the routine and must be called even if preemptive switching is disabled.\n
 * The preemptive context switch should follow (if preemptive switching is enabled) and must call CB_DBOS_Task_Core_SelectNextTaskFromISR() to determine the next task to run.
 *
 * <b> The pending bit is cleared at the start of the handler.\n
 * It can be set pending by a higher priority ISR which runs while the PendSV_Handler is running.\n
 * This must cause the handler to run again. </b>
 *
 * @see DBOS_Port_SetPendContextSwitch()
 * @see dBOS::dBOS_KERNEL::ProcessISRFlags()
 * @see CB_DBOS_Task_Core_SelectNextTaskFromISR()
 */
#if !defined(__DOXYGEN__)
__attribute__(( naked ))
#else
#error // DON'T BUILD
#endif
void PendSV_Handler(){ //lint !e963 // - Note 963: Qualifier const or volatile precedes a type; use +fqb to reverse the test

// Suppress messages generated by use of CMSIS
//lint -save -e1960 -e923 -e1924
	SCB->ICSR = SCB_ICSR_PENDSVCLR;				// Clear Pending bit for PendSV handler, this is done automatically by hardware but may have been set again
												// if a higher priority ISR which ran (late arrival) and called OS which set PendSV pending again..
//lint -restore

#if (DBOS_PREEMPTIONENABLED == 1U)
#if (DBOS_USE_TIMESLICING == 1U)
	S_TimeSliceCount = DBOS_USE_MAX_TIME_SLICE; // Assume this is an atomic operation
#endif
#endif

	S_pKernel->ProcessISRFlags();

#if (DBOS_PREEMPTIONENABLED == 1U)

	PORT_ASM_VOLATILE
	(
			// Currently using process stack, PSR, PC, LR, R12, R3, R2, R1, R0 already pushed onto stack.
			"	MRS r12, PSP 						\n"// Recover PSP into R12
			"	STMDB r12!, {r4-r11} 				\n"// Push remaining (non-stack) registers onto the stack.

			" 	MRS r1, BASEPRI						\n"// Save BASEPRI register
			"	STMDB r12!, {r1} 					\n"

			"	LDR r1, [%[PtrPtrCurrSp]]			\n"// r1 = PtrCurrSp;
			"	STR r12, [r1]				 		\n"// *PtrCurrSp = SP;

			"	PUSH {%[PtrPtrCurrSp]}				\n"

			"	BL CB_DBOS_Task_Core_SelectNextTaskFromISR	\n"// Return value is in r0 and is location of stack pointer to restore.

			"	POP {%[PtrPtrCurrSp]}				\n"

			"	STR r0, [%[PtrPtrCurrSp]]			\n"// Location of current stack pointer updated to stack pointer this is switching to.

			"	LDR r12, [r0]						\n"// r12 = NewSP

			" 	LDMIA r12!, {r1}					\n"
			"	MSR BASEPRI, r1						\n"// Restore BASEPRI register

			" 	LDMIA r12!, {r4-r11}				\n"// Restore non-stacked registers.
			"	MSR PSP, r12						\n"// PSP = r12
			" 	ISB									\n"// This ensures that instructions after the ISB instruction execute using the new stack pointer

			" 	MOV LR, #0xFFFFFFFD					\n"// Set EXC_RETURN value, Return to Thread Mode, and on return use the process Stack.
			" 	BX LR								\n"// Execute Exception Return.
			:// Outputs
			: [PtrPtrCurrSp] "r" (&S_DBOS_pCurrentStackPointer)// Inputs
			: "r0", "r1", "r12", "cc"// Clobber List
	)
	;

#else
	PORT_ASM_VOLATILE
	(
			" 	MOV LR, #0xFFFFFFFD					\n" // Set EXC_RETURN value, Return to Thread Mode, and on return use the process Stack.
			" 	BX LR								\n"// Execute Exception Return.
			:// Outputs
			:// Inputs
			:// Clobber List
	)
	;
#endif

	return;
}

//	- Info 765: external 'TIM2_IRQHandler(void)' (line 855, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) could be made static [MISRA C++ Rule 3-3-1]
//	- Info 714: Symbol 'TIM2_IRQHandler(void)' (line 855, file C:\working\dBOS_Dev\dBOSpp\Port\dBOS_Port.cpp) not referenced [MISRA C++ Rule 0-1-3], [MISRA C++ Rule 0-1-10]
//lint -esym(765, TIM2_IRQHandler)
//lint -esym(714, TIM2_IRQHandler)
/**
 * \brief System Tick Time Routine
 *
 * Must have a priority two levels higher than PendSV_Handler() and should be the only routine on that priority level.
 * Must call CB_DBOS_Task_Core_RunTimersFromISR() and CB_DBOS_Timer_Process_ISR()
 *
 * @see PendSV_Handler()
 * @see dBOS_TIMERMANAGER::ISR()
 */
void TIM2_IRQHandler(){

	Per_ResetBit_BB(&(TIM2->SR), 0U);	// clear flag

	S_pTimerManager->ISR();

#if (DBOS_PREEMPTIONENABLED == 1U)
#if (DBOS_USE_TIMESLICING == 1U)

	if(S_TimeSliceCount > 0U){
		S_TimeSliceCount--;
	}

	if(S_TimeSliceCount == 0U){
		S_TimeSliceCount = DBOS_USE_MAX_TIME_SLICE;

		SET_CONTEXT_SWITCH_PENDING
	}

#endif
#endif

}

} // extern C

} // namespace
