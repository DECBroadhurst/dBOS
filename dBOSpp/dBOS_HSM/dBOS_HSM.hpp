/**
 * @file dBOS_HSM.cpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Definition of dBOS Hierarchical State Machine.
 *
 */

// -------------------------------------------------------------------------------------------------
#ifndef DBOS_HSM_H_
#define DBOS_HSM_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSMState.hpp"

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
class dBOS_HSM {

public:

	dBOS_HSM(char const * const pName, dBOS_HSM_DEBUG_MODE_t const DebugMode);
	virtual ~dBOS_HSM();

	virtual void Init(void) = 0;

	dBOS_HSM_HANDLER_RETURN_t ProcessEvent(pdBOS_HSMEVENT_CONST_t const pEvent);

protected:

	dBOS_HSM() = delete;
	dBOS_HSM(dBOS_HSM const &) = delete;
	dBOS_HSM& operator=(dBOS_HSM const &) = delete;

	virtual void Start(pDBOS_HSMSTATE_CONST_t const pInitialState);

	pDBOS_HSMSTATES_t m_pStates; /**< Pointer to structure of states. */

	void * m_pExtended; /**< Pointer to structure of extended state variables. */

private:

	pDBOS_HSMSTATE_CONST_t Transistion(pDBOS_HSMSTATE_CONST_t const pSource, pDBOS_HSMSTATE_CONST_t const pDest);

	pDBOS_HSMSTATE_CONST_t m_pCurrentState; /**< Pointer to current state. */
	char const * const m_pName; /**< Pointer to name string. */
	dBOS_HSM_DEBUG_MODE_t const m_DebugMode; /**< When set to 1U Debug output will be displayed. */

};

} // namespace

#endif /* DBOS_HSM_H_ */
