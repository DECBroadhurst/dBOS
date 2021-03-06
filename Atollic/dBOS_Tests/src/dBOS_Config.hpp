/**
 * @file dBOS_Config.hpp
 * @author David Broadhurst
 * @date 13/01/2015
 *
 * \brief Configuration of features and options.
 *
 */

#ifndef DBOS_CONFIG_HPP_
#define DBOS_CONFIG_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

/**
 * \brief Enabled or disable preemptive context switching.
 *
 * 0U = Preemptive task switching disabled.\n
 * 1U = Preemptive task switching enabled.
 */
#define DBOS_PREEMPTIONENABLED	0U

/**
 * \brief Enable or disable time slicing.
 *
 * 0U = Time slicing disabled.\n
 * 1U = Time slicing enabled.
 */
#define DBOS_USE_TIMESLICING 	0U

/**
 * \brief Maximum number of time counts a task will run for before a preemptive context switch.
 *
 * Valid range is greater than 0.
 */
#define DBOS_USE_MAX_TIME_SLICE 20U

/**
 * \brief Maximum priority a task can have.
 *
 * Priority level 0 (lowest) is reserved for idle task.\n
 * Application tasks can have priority levels from 1 to MAXTASKPRIORITY inclusive.
 */
#define MAXTASKPRIORITY			20U

/**
 * \brief Maximum Number of tasks.
 */
#define MAXNUMOFTASKS			36U

/**
 * \brief Maximum Number of Sync Objects
 */
#define MAXNUMOFSYNCOBJECTS		64U

/**
 * \brief Enabled or disable debug text output.
 *
 * 0U = All functions related to debug output are not included in the build.\n
 * 1U = All functions related to debug output are included in the build.
 */
#define DBOS_DEBUGOUTPUTENABLED	1U

/**
 * \brief Enable or disable code coverage print output.
 *
 * 0U = code coverage print output disabled.
 * 1U = code coverage print output enabled.
 */
#define DBOS_COVERAGEPRINTOUTPUT 1U

/**
 * \brief Enabled or disable test output for unit tests.
 *
 * 0U = Test output disable.\n
 * 1U = Test output enabled.
 */
#define DBOS_TESTSENABLED		1U

/**
 * \brief Enabled or disable assertions.
 *
 * 0U = Assertions disabled.\n
 * 1U = Assertions enabled.
 */
#define DBOS_ASSERTIONSENABLED	1U

/**
 * \brief Enable or disable use of OS object name strings.
 *
 * 0U = Object name strings disabled.\n
 * 1U = Object name strings enabled.
 */
#define DBOS_USE_OBJECTNAMES 	1U // TODO Test with 0

// -------------------------------------------------------------------------------------------------
// DATA TYPES

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO�S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

}// namespace

#endif /* DBOS_CONFIG_HPP_ */
