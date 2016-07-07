/**
 * @file dBOS_HSMEvent.hpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Definition of dBOS Hierarchical State Machine Event.
 *
 */

// -------------------------------------------------------------------------------------------------
#ifndef DBOS_HSMEVENT_H_
#define DBOS_HSMEVENT_H_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSMTypes.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// FORWARD DECLERATIONS
class dBOS_HSMEVENTPROP;
class dBOS_HSMEVENTDATA;
class dBOS_HSMEVENT;

// -------------------------------------------------------------------------------------------------
// DATA TYPES
typedef dBOS_HSMEVENTPROP * pdBOS_HSMEVENTPROP_t;

typedef dBOS_HSMEVENTPROP const * pdBOS_HSMEVENTPROP_CONST_t;

typedef dBOS_HSMEVENT * pdBOS_HSMEVENT_t;

typedef dBOS_HSMEVENT const * pdBOS_HSMEVENT_CONST_t;

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
class dBOS_HSMEVENTPROP {
public:

	constexpr dBOS_HSMEVENTPROP(dBOS_HSM_EVENT_ID_t const ID, char const * const pName) :
			m_ID(ID), m_pName(pName){
	}
	;

	constexpr operator int() const{
		return m_ID;
	}

	dBOS_HSM_EVENT_ID_t const m_ID;
	char const * const m_pName;

protected:

	dBOS_HSMEVENTPROP() = delete;
	dBOS_HSMEVENTPROP(const dBOS_HSMEVENTPROP &E) = delete;
	dBOS_HSMEVENTPROP & operator=(const dBOS_HSMEVENTPROP & E) = delete;

private:

};

class dBOS_HSMEVENTDATA {
public:

	dBOS_HSMEVENTDATA();
	dBOS_HSMEVENTDATA(const dBOS_HSMEVENTDATA &D);
	dBOS_HSMEVENTDATA & operator=(const dBOS_HSMEVENTDATA & D);

	virtual ~dBOS_HSMEVENTDATA();

	union {
		void * pnt;
		INT32U Uint32;
		INT32S Sint32;
		INT16U Uint16[2];
		INT16S Sint16[2];
		INT8U Uint8[4];
		INT8S Sint8[4];
	};

protected:

private:

};

class dBOS_HSMEVENT {
public:

	dBOS_HSMEVENT();
	dBOS_HSMEVENT(pdBOS_HSMEVENTPROP_CONST_t const pProp, dBOS_HSMEVENTDATA const Data);
	explicit dBOS_HSMEVENT(const dBOS_HSMEVENT &E);
	dBOS_HSMEVENT & operator=(const dBOS_HSMEVENT & E);

	virtual ~dBOS_HSMEVENT();

	pdBOS_HSMEVENTPROP_CONST_t m_pProp;
	dBOS_HSMEVENTDATA m_Data;

protected:

private:

};

} // namespace

#endif /* DBOS_HSMEVENT_H_ */
