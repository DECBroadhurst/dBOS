/**
 * @file dBOS_TimerManager.hpp
 * @author David Broadhurst
 * @date 2/03/2015
 *
 * \brief Definition of dBOS Timer Manager.
 *
 */

#ifndef DBOS_TIMERMANAGER_HPP_
#define DBOS_TIMERMANAGER_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Core_DataTypes.hpp"
#include "dBOS_List.hpp"

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

// Singleton Class
class dBOS_TIMERMANAGER {

public:

	static DBOS_RESULT_t Init(void); // To be called before OS starts.
	static dBOS_TIMERMANAGER * GetInstance(void);

	void AddTimer(dBOS_LISTNODE<class dBOS_TIMERBASE> * const pNode);
	void RemoveTimer(dBOS_LISTNODE<class dBOS_TIMERBASE> * const pNode);

	void ISR(void);

	DBOS_TIMECOUNT_t GetEllapsedTime(void) const;

protected:

private:

	dBOS_TIMERMANAGER();
	~dBOS_TIMERMANAGER();
	dBOS_TIMERMANAGER(dBOS_TIMERMANAGER const &) = delete;
	dBOS_TIMERMANAGER& operator=(dBOS_TIMERMANAGER const &) = delete;

	static dBOS_TIMERMANAGER * m_pInstance;

	DBOS_TIMECOUNT_t m_EllpasedTime;

	dBOS_LINKEDLIST<class dBOS_TIMERBASE> m_TimerList;

};

} // namespace

#endif /* DBOS_TIMERMANAGER_HPP_ */
