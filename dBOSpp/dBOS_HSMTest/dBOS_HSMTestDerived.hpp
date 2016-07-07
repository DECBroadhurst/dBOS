/**
 * @file dBOS_HSMTestDerived.hpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Definition of a dBOS HSM derived from dBOS_HSMTESTBASE for unit testing.
 *
 */

// -------------------------------------------------------------------------------------------------
#ifndef DBOS_HSMTESTDERIVED_H_
#define DBOS_HSMTESTDERIVED_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSMTestBase.hpp"

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
class dBOS_HSMTESTSTATE_S1_1 : public dBOS_HSMTESTSTATE_S1 {

public:

	explicit dBOS_HSMTESTSTATE_S1_1(char const * const pName);
	dBOS_HSMTESTSTATE_S1_1(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S1_1();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S1_1() = delete;
	dBOS_HSMTESTSTATE_S1_1(dBOS_HSMTESTSTATE_S1_1 const &) = delete;
	dBOS_HSMTESTSTATE_S1_1& operator=(dBOS_HSMTESTSTATE_S1_1 const &) = delete;

private:

};

class dBOS_HSMTESTSTATE_S123 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S123(char const * const pName);
	dBOS_HSMTESTSTATE_S123(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S123();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S123() = delete;
	dBOS_HSMTESTSTATE_S123(dBOS_HSMTESTSTATE_S123 const &) = delete;
	dBOS_HSMTESTSTATE_S123& operator=(dBOS_HSMTESTSTATE_S123 const &) = delete;

private:

};

struct dBOS_HSMTESTDERIVEDSTATES : public dBOS_HSMTESTBASESTATES {

public:

	dBOS_HSMTESTDERIVEDSTATES() :
			dBOS_HSMTESTBASESTATES(), pS123(NULLPTR){
	}
	;

	virtual ~dBOS_HSMTESTDERIVEDSTATES(){
	}
	;

	pDBOS_HSMSTATE_CONST_t pS123; // Adds new state to base state machine.

protected:

	dBOS_HSMTESTDERIVEDSTATES(dBOS_HSMTESTDERIVEDSTATES const &) = delete;
	dBOS_HSMTESTDERIVEDSTATES& operator=(dBOS_HSMTESTDERIVEDSTATES const &) = delete;

private:

};

class dBOS_HSMTESTDERIVED : public dBOS_HSMTESTBASE {

public:

	dBOS_HSMTESTDERIVED(char const * const pName, dBOS_HSM_DEBUG_MODE_t const DebugMode);
	virtual ~dBOS_HSMTESTDERIVED();

	virtual void Init(void) override;

protected:

	dBOS_HSMTESTDERIVED() = delete;
	dBOS_HSMTESTDERIVED(dBOS_HSMTESTDERIVED const &) = delete;
	dBOS_HSMTESTDERIVED& operator=(dBOS_HSMTESTDERIVED const &) = delete;

private:

};

} // namespace

#endif /* DBOS_HSMTESTBASE_H_ */
