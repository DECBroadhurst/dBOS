/**
 * @file dBOS_HSMState.hpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Definition of dBOS Hierarchical State Machine State.
 *
 */

// -------------------------------------------------------------------------------------------------
#ifndef DBOS_HSMSTATE_H_
#define DBOS_HSMSTATE_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSMTypes.hpp"
#include "dBOS_HSMEvent.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// FORWARD DECLERATIONS

// -------------------------------------------------------------------------------------------------
// DATA TYPES
typedef class dBOS_HSMSTATE * pDBOS_HSMSTATE_t;

typedef class dBOS_HSMSTATE const * pDBOS_HSMSTATE_CONST_t;

typedef struct dBOS_HSMSTATES * pDBOS_HSMSTATES_t;

typedef struct dBOS_HSMSTATES const * pDBOS_HSMSTATES_CONST_t;

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
class dBOS_HSMSTATE {

public:

	explicit dBOS_HSMSTATE(char const * const pName);
	dBOS_HSMSTATE(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);
	virtual ~dBOS_HSMSTATE();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const = 0;
	virtual void Entry(void * const pExtended) const = 0;
	virtual void Exit(void * const pExtended) const = 0;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const = 0;

protected:

	dBOS_HSMSTATE() = delete;
	dBOS_HSMSTATE(dBOS_HSMSTATE const &) = delete;
	dBOS_HSMSTATE& operator=(dBOS_HSMSTATE const &) = delete;

private:

	friend class dBOS_HSM;

	pDBOS_HSMSTATE_CONST_t const m_pParent; /**< Pointer to parent state. */
	char const * const m_pName; /**< Pointer to name string. */
	UNATIVECPU_t const m_Level; /**< The level of this state in the hierarchy. Top level state is level 0. */
	pDBOS_HSMSTATE_CONST_t m_pDestNext; /**< Used to remember path to take through states when going from LCA state to destination state. */

};

struct dBOS_HSMSTATES {

public:

	dBOS_HSMSTATES() :
			pTopState(NULLPTR){
	}
	;

	virtual ~dBOS_HSMSTATES(){
	}
	;

	pDBOS_HSMSTATE_CONST_t pTopState;

protected:

	dBOS_HSMSTATES(dBOS_HSMSTATES const &) = delete;
	dBOS_HSMSTATES& operator=(dBOS_HSMSTATES const &) = delete;

private:

};

} // namespace

#endif /* DBOS_HSMSTATE_H_ */
