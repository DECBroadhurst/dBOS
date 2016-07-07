/**
 * @file dBOS_TaskInterface.hpp
 * @author David Broadhurst
 * @date 15/03/2015
 *
 * \brief Definition of dBOS Task Interface.
 *
 */

#ifndef DBOS_TASKINTERFACE_HPP_
#define DBOS_TASKINTERFACE_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_DataTypes.hpp"

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

class dBOS_TASKINTERFACE {

public:

	explicit dBOS_TASKINTERFACE(class dBOS_TASK * const pTask);
	dBOS_TASKINTERFACE(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const UniqueSyncID, DBOS_TASK_PRIORITY_t const Priority, DBOS_PNTEQUIV_t const TaskStackSize, DBOS_TASKENTRY_FUNCTION_PNT_t const pTaskFunction,
			void const * const pTaskEntryData);
	~dBOS_TASKINTERFACE();

	dBOS_TASKINTERFACE() = delete;
	dBOS_TASKINTERFACE(dBOS_TASKINTERFACE const &) = delete;
	dBOS_TASKINTERFACE& operator=(dBOS_TASKINTERFACE const &) = delete;

	DBOS_TASK_PRIORITY_t GetCurrentPriority(void) const;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	void PrintDetails(void) const;
	void PrintTypeAndID(void) const;
	void PrintFreeStackSpace(void) const;
#endif

protected:

private:

	static class dBOS_KERNEL * m_pK;

	class dBOS_TASK * const m_pImpl;

};

} // namespace

#endif /* DBOS_TASKINTERFACE_HPP_ */
