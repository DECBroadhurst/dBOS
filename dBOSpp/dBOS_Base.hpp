/**
 * @file dBOS_Base.hpp
 * @author David Broadhurst
 * @date 15/01/2015
 *
 * \brief Definition of dBOS Base Class.
 *
 */

#ifndef DBOS_BASE_HPP_
#define DBOS_BASE_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Core_DataTypes.hpp"
#include "dBOS_Config.hpp"

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
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// CLASSES

class dBOS_BASE {

public:

	explicit dBOS_BASE(DBOS_ID_t const ID);
	virtual ~dBOS_BASE();

	dBOS_BASE() = delete;
	dBOS_BASE(dBOS_BASE const &) = delete;
	dBOS_BASE& operator=(dBOS_BASE const &) = delete;

	virtual DBOS_ID_t GetID(void) const;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintID(void) const;
#endif

protected:

private:

	DBOS_ID_t const m_ID;

};

} // namespace

#endif /* DBOS_BASE_HPP_ */
