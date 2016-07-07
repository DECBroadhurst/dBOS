/**
 * @file dBOS_Assert.h
 * @author David Broadhurst
 * @date 22/04/2014
 *
 * \brief Assertion Macros
 *
 */

#ifndef DBOS_ASSERT_HPP_
#define DBOS_ASSERT_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_IntDebugPrint.hpp"

// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// DATA TYPES

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S
/**
 * \brief Assertion Macro to be used from within operating system critical section or from within ISR.
 *
 */
#if DBOS_ASSERTIONSENABLED == 1U
#define ASSERT_FROM_OSCS(expr)		\
			if(expr){				\
			}						\
			else{					\
				DBOS_Intprintf("\n\nAssertion Fail: %s Line: %u", __func__, __LINE__);	\
				for( ; ; ){}		\
			};
#else
#define ASSERT_FROM_OSCS(expr)
#endif
/**
 * \brief Error Assertion Macro to be used from within operating system critical section or from within ISR.
 *
 */
#define ASSERT_ERROR_FROM_OSCS()	ASSERT_FROM_OSCS(false)

#if DBOS_ASSERTIONSENABLED == 1U
#define DBOSASSERT(expr) ASSERT_FROM_OSCS(expr)
#else
#define DBOSASSERT(expr)
#endif

#define DBOSASSERTERROR() DBOSASSERT(false)

/**
 * \brief Macro for code coverage print output.
 *
 */
#if DBOS_COVERAGEPRINTOUTPUT == 1U
#define COVERAGE_PRINT(S) DBOS_Intprintf("\nOS CC: %s, Line: %u, %s", __func__, __LINE__, (S));
#else
#define COVERAGE_PRINT(S)
#endif
// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

#endif /* DBOS_ASSERT_HPP_ */
