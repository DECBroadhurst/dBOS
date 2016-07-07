/**
 * @file dBOS_PortFunctions.hpp
 * @author David Broadhurst
 * @date 14/01/2015
 *
 * \brief Implementation of port specific inline functions.
 *
 */

#ifndef DBOS_PORTFUNCTIONS_H_
#define DBOS_PORTFUNCTIONS_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_DataTypes.hpp"

namespace dBOS {

// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// CONSTANTS

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// FORWARD DECLERATIONS

// -------------------------------------------------------------------------------------------------
// DATA TYPES

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// INLINE FUNCTIONS

//lint -save
//lint -e530 	Warning 530: Symbol 'LeadingZeros' not initialized [MISRA 2004 Rule 9.1, required]
//				Symbol is initialized in assembly code.
//lint -e715	Info 715: Symbol 'Word' not referenced
//				Word is referenced in assembly code.
/**
 * \brief Counts the number of leading zeros in the argument.
 *
 * The result is the width of the word if no bits are set.\n
 * The result is zero if the MSb is set.\n
 *
 * @param[in] Word unsigned integer.
 *
 * \return Count of the number of leading zeros.
 */
static __attribute__(( always_inline )) inline UNATIVECPU_t DBOS_Port_CountLeadingZeros(UNATIVECPU_t const Word){

	UNATIVECPU_t LeadingZeros;

	PORT_ASM_VOLATILE( "CLZ %[LZReg], %[WordReg]	\n"
			: [LZReg] "=r" ( LeadingZeros ) // Outputs
			: [WordReg] "r" ( Word )// Inputs
			:// Clobber list
	)
	;

	return LeadingZeros;
} //lint !e953 // Note 953: Variable 'LeadingZeros' (line 69) could be declared as const --- Eff. C++ 3rd Ed. item 3
//lint -restore

//lint -save
//lint -e530 	Warning 530: Symbol 'LeadingZeros' not initialized [MISRA 2004 Rule 9.1, required]
//				Symbol is initialized in assembly code.
//lint -e715	Info 715: Symbol 'Word' not referenced
//				Word is referenced in assembly code.
/**
 * \brief Counts the number of trailing zeros in the argument.
 *
 * The result is the width of the word if no bits are set.\n
 * The result is zero if the LSb is set.\n
 *
 * @param[in] Word unsigned integer.
 *
 * \return Count of the number of trailing zeros.
 */
static __attribute__(( always_inline )) inline UNATIVECPU_t DBOS_Port_CountTrailingZeros(UNATIVECPU_t const Word){

	UNATIVECPU_t TrailingZeros;

	PORT_ASM_VOLATILE( "RBIT r0, %[WordReg]		\n"
			"CLZ %[LZReg], r0					\n"
			: [LZReg] "=r" ( TrailingZeros ) // Outputs
			: [WordReg] "r" ( Word )// Inputs
			: "r0"// Clobber list
	)
	;

	return TrailingZeros;
} //lint !e953 // Note 953: Variable 'TrailingZeros' (line 98) could be declared as const --- Eff. C++ 3rd Ed. item 3
//lint -restore

/**
 * \brief Compares two interrupt masks.
 *
 * Returns TRUE if first argument will only mask interrupts of lower or equal priority than second argument.
 *
 * \note Arguments use the CMSIS convention for priority.
 *
 * @param[in] A First argument in comparison.
 * @param[in] B Second argument in comparison.
 *
 * \return Result of comparison TRUE or FALSE.
 */
static __attribute__( ( always_inline )) inline BOOL DBOS_Port_MaskALowerOrEqualPriorityThanB(INTERRUPT_MASK_t const A, INTERRUPT_MASK_t const B){

	BOOL Result;

	if(A == ISRMASK_NONE){
		Result = TRUE;
	}
	else if(B == ISRMASK_NONE){
		Result = FALSE;
	}
	else if(A >= B){	// Higher numbers are lower priority on Cortex M
		Result = TRUE;
	}
	else{
		Result = FALSE;
	}

	return Result;
}

/**
 * \brief Compares two interrupt masks.
 *
 * Returns TRUE if first argument will mask interrupts of higher or equal priority than second argument.
 *
 * \note Arguments use the CMSIS convention for priority.
 *
 * @param[in] A First argument in comparison.
 * @param[in] B Second argument in comparison.
 *
 * \return Result of comparison TRUE or FALSE.
 */
static __attribute__( ( always_inline )) inline BOOL DBOS_Port_MaskAHigherOrEqualPriorityThanB(INTERRUPT_MASK_t const A, INTERRUPT_MASK_t const B){

	BOOL Result;

	if(B == ISRMASK_NONE){
		Result = TRUE;
	}
	else if(A == ISRMASK_NONE){
		Result = FALSE;
	}
	else if(A <= B){	// Higher numbers are lower priority on Cortex M
		Result = TRUE;
	}
	else{
		Result = FALSE;
	}

	return Result;
}

// -------------------------------------------------------------------------------------------------
// CLASSES

}// namespace

#endif /* DBOS_PORTFUNCTIONS_H_ */
