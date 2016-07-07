/**
 * @file dBOS_HSMTestBase.hpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Definition of a dBOS HSM for unit testing.
 *
 */

// -------------------------------------------------------------------------------------------------
#ifndef DBOS_HSMTESTBASE_H_
#define DBOS_HSMTESTBASE_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSM.hpp"

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
class dBOS_HSMTESTSTATE_S1 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S1(char const * const pName);
	dBOS_HSMTESTSTATE_S1(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S1();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S1() = delete;
	dBOS_HSMTESTSTATE_S1(dBOS_HSMTESTSTATE_S1 const &) = delete;
	dBOS_HSMTESTSTATE_S1& operator=(dBOS_HSMTESTSTATE_S1 const &) = delete;

private:

};

class dBOS_HSMTESTSTATE_S11 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S11(char const * const pName);
	dBOS_HSMTESTSTATE_S11(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S11();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S11() = delete;
	dBOS_HSMTESTSTATE_S11(dBOS_HSMTESTSTATE_S11 const &) = delete;
	dBOS_HSMTESTSTATE_S11& operator=(dBOS_HSMTESTSTATE_S11 const &) = delete;

private:

};

class dBOS_HSMTESTSTATE_S12 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S12(char const * const pName);
	dBOS_HSMTESTSTATE_S12(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S12();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S12() = delete;
	dBOS_HSMTESTSTATE_S12(dBOS_HSMTESTSTATE_S12 const &) = delete;
	dBOS_HSMTESTSTATE_S12& operator=(dBOS_HSMTESTSTATE_S12 const &) = delete;

private:

};

class dBOS_HSMTESTSTATE_S121 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S121(char const * const pName);
	dBOS_HSMTESTSTATE_S121(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S121();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S121() = delete;
	dBOS_HSMTESTSTATE_S121(dBOS_HSMTESTSTATE_S121 const &) = delete;
	dBOS_HSMTESTSTATE_S121& operator=(dBOS_HSMTESTSTATE_S121 const &) = delete;

private:

};

class dBOS_HSMTESTSTATE_S122 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S122(char const * const pName);
	dBOS_HSMTESTSTATE_S122(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S122();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S122() = delete;
	dBOS_HSMTESTSTATE_S122(dBOS_HSMTESTSTATE_S122 const &) = delete;
	dBOS_HSMTESTSTATE_S122& operator=(dBOS_HSMTESTSTATE_S122 const &) = delete;

private:

};

class dBOS_HSMTESTSTATE_S1221 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S1221(char const * const pName);
	dBOS_HSMTESTSTATE_S1221(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S1221();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S1221() = delete;
	dBOS_HSMTESTSTATE_S1221(dBOS_HSMTESTSTATE_S1221 const &) = delete;
	dBOS_HSMTESTSTATE_S1221& operator=(dBOS_HSMTESTSTATE_S1221 const &) = delete;

private:

};

class dBOS_HSMTESTSTATE_S13 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S13(char const * const pName);
	dBOS_HSMTESTSTATE_S13(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S13();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S13() = delete;
	dBOS_HSMTESTSTATE_S13(dBOS_HSMTESTSTATE_S13 const &) = delete;
	dBOS_HSMTESTSTATE_S13& operator=(dBOS_HSMTESTSTATE_S13 const &) = delete;

private:

};

class dBOS_HSMTESTSTATE_S131 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S131(char const * const pName);
	dBOS_HSMTESTSTATE_S131(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S131();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S131() = delete;
	dBOS_HSMTESTSTATE_S131(dBOS_HSMTESTSTATE_S131 const &) = delete;
	dBOS_HSMTESTSTATE_S131& operator=(dBOS_HSMTESTSTATE_S131 const &) = delete;

private:

};

class dBOS_HSMTESTSTATE_S132 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S132(char const * const pName);
	dBOS_HSMTESTSTATE_S132(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S132();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S132() = delete;
	dBOS_HSMTESTSTATE_S132(dBOS_HSMTESTSTATE_S132 const &) = delete;
	dBOS_HSMTESTSTATE_S132& operator=(dBOS_HSMTESTSTATE_S132 const &) = delete;

private:

};

class dBOS_HSMTESTSTATE_S1321 : public dBOS_HSMSTATE {

public:

	explicit dBOS_HSMTESTSTATE_S1321(char const * const pName);
	dBOS_HSMTESTSTATE_S1321(pDBOS_HSMSTATE_CONST_t const pParent, char const * const pName);

	virtual ~dBOS_HSMTESTSTATE_S1321();

	virtual dBOS_HSM_HANDLER_RETURN_t EntryChecks(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;
	virtual void Entry(void * const pExtended) const override;
	virtual void Exit(void * const pExtended) const override;
	virtual dBOS_HSM_HANDLER_RETURN_t Event(pDBOS_HSMSTATES_CONST_t const pStates, void * const pExtended, pdBOS_HSMEVENT_CONST_t const pEvent, pDBOS_HSMSTATE_CONST_t * const pNextState) const override;

protected:

	dBOS_HSMTESTSTATE_S1321() = delete;
	dBOS_HSMTESTSTATE_S1321(dBOS_HSMTESTSTATE_S1321 const &) = delete;
	dBOS_HSMTESTSTATE_S1321& operator=(dBOS_HSMTESTSTATE_S1321 const &) = delete;

private:

};

struct dBOS_HSMTESTBASESTATES : public dBOS_HSMSTATES {

public:

	dBOS_HSMTESTBASESTATES() :
			dBOS_HSMSTATES(), pS11(NULLPTR), pS12(NULLPTR), pS121(NULLPTR), pS122(NULLPTR), pS1221(NULLPTR), pS13(NULLPTR), pS131(NULLPTR), pS132(NULLPTR), pS1321(NULLPTR){
	}
	;

	virtual ~dBOS_HSMTESTBASESTATES(){
	}
	;

	pDBOS_HSMSTATE_CONST_t pS11;
	pDBOS_HSMSTATE_CONST_t pS12;
	pDBOS_HSMSTATE_CONST_t pS121;
	pDBOS_HSMSTATE_CONST_t pS122;
	pDBOS_HSMSTATE_CONST_t pS1221;
	pDBOS_HSMSTATE_CONST_t pS13;
	pDBOS_HSMSTATE_CONST_t pS131;
	pDBOS_HSMSTATE_CONST_t pS132;
	pDBOS_HSMSTATE_CONST_t pS1321;

protected:

	dBOS_HSMTESTBASESTATES(dBOS_HSMTESTBASESTATES const &) = delete;
	dBOS_HSMTESTBASESTATES& operator=(dBOS_HSMTESTBASESTATES const &) = delete;

private:

};

class dBOS_HSMTESTBASE : public dBOS_HSM {

public:

	dBOS_HSMTESTBASE(char const * const pName, dBOS_HSM_DEBUG_MODE_t const DebugMode);
	virtual ~dBOS_HSMTESTBASE();

	virtual void Init(void) override;

protected:

	dBOS_HSMTESTBASE() = delete;
	dBOS_HSMTESTBASE(dBOS_HSMTESTBASE const &) = delete;
	dBOS_HSMTESTBASE& operator=(dBOS_HSMTESTBASE const &) = delete;

private:

};

} // namespace

#endif /* DBOS_HSMTESTBASE_H_ */
