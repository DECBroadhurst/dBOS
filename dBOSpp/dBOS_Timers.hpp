/**
 * @file dBOS_Timers.hpp
 * @author David Broadhurst
 * @date 27/03/2015
 *
 * \brief Definition of dBOS Timers.
 *
 */

#ifndef DBOS_TIMERS_HPP_
#define DBOS_TIMERS_HPP_

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

class dBOS_TIMERBASE {

public:

	dBOS_TIMERBASE();
	virtual ~dBOS_TIMERBASE();

	virtual void RunFromISR(void) = 0;

	virtual void Pause(void) = 0;
	virtual void Run(void) = 0;
	virtual void Reset(void) = 0;
	virtual void ResetAndPause(void) = 0;
	virtual void SetTime(DBOS_TIMECOUNT_t const Time) = 0;
	virtual DBOS_TIMECOUNT_t GetTime(void) const = 0;

	virtual void PrintDetails(void) const = 0;

protected:

	static class dBOS_TIMERMANAGER * m_pTimerManager;
	static class dBOS_KERNEL * m_pK;

	dBOS_TIMERBASE(dBOS_TIMERBASE const &) = delete;
	dBOS_TIMERBASE& operator=(dBOS_TIMERBASE const &) = delete;

private:

};

class dBOS_TIMERCOUNTDOWN : public dBOS_TIMERBASE {

public:

	explicit dBOS_TIMERCOUNTDOWN(DBOS_SYNCBASE_P_t const pSyncObject);
	virtual ~dBOS_TIMERCOUNTDOWN();

	dBOS_TIMERCOUNTDOWN() = delete;
	dBOS_TIMERCOUNTDOWN(dBOS_TIMERCOUNTDOWN const &) = delete;
	dBOS_TIMERCOUNTDOWN& operator=(dBOS_TIMERCOUNTDOWN const &) = delete;

	virtual void RunFromISR(void) override;

	virtual void Pause(void) override;
	virtual void Run(void) override;
	virtual void Reset(void) override;
	virtual void ResetAndPause(void) override;
	virtual void SetTime(DBOS_TIMECOUNT_t const Time) override;
	virtual DBOS_TIMECOUNT_t GetTime(void) const override;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails(void) const override;
#endif

protected:

private:

	dBOS_LISTNODE<dBOS_TIMERBASE> m_Node;
	TIMERSTATE_t m_State;
	DBOS_TIMECOUNT_t m_CountTime;

	DBOS_SYNCBASE_P_t const m_pSyncObject;

};

class dBOS_TIMERFULLFEATURE : public dBOS_TIMERBASE {

public:

	dBOS_TIMERFULLFEATURE(INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime, DBOS_SYNCBASE_P_t const pSyncObject);
	virtual ~dBOS_TIMERFULLFEATURE();

	dBOS_TIMERFULLFEATURE() = delete;
	dBOS_TIMERFULLFEATURE(dBOS_TIMERFULLFEATURE const &) = delete;
	dBOS_TIMERFULLFEATURE& operator=(dBOS_TIMERFULLFEATURE const &) = delete;

	virtual void RunFromISR(void) override;

	virtual void Pause(void) override;
	virtual void Run(void) override;
	virtual void Reset(void) override;
	virtual void ResetAndPause(void) override;
	virtual void SetTime(DBOS_TIMECOUNT_t const Time) override;
	virtual DBOS_TIMECOUNT_t GetTime(void) const override;

	void Configure(INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime);

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails(void) const override;
#endif

protected:

private:

	dBOS_LISTNODE<dBOS_TIMERBASE> m_Node;
	TIMERSTATE_t m_State;
	DBOS_TIMECOUNT_t m_CountTime;
	DBOS_TIMECOUNT_t m_ReloadTime;
	INT8U m_Flags;

	DBOS_SYNCBASE_P_t const m_pSyncObject;

};

} // namespace

#endif /* DBOS_TIMERS_HPP_ */
