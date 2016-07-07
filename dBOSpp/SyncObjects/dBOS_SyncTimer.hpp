/**
 * @file dBOS_SyncTimer.hpp
 * @author David Broadhurst
 * @date 28/02/2015
 *
 * \brief Definition of synchronization timer object.
 *
 */

#ifndef DBOS_SYNCTIMER_HPP_
#define DBOS_SYNCTIMER_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Sync.hpp"
#include "dBOS_Timers.hpp"

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
class dBOS_SYNCTIMER : public dBOS_SYNC {

public:

	dBOS_SYNCTIMER(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime, DBOS_WAITLIST_SORT_t const WaitListType);
	virtual ~dBOS_SYNCTIMER();

	dBOS_SYNCTIMER() = delete;
	dBOS_SYNCTIMER(dBOS_SYNCTIMER const &) = delete;
	dBOS_SYNCTIMER& operator=(dBOS_SYNCTIMER const &) = delete;

	virtual SIGNALLED_DATA_t GetSignalData(void) const override;
	virtual DBOS_TASK_P_t GetOwnerTask(void) const override;

	virtual DBOS_RESULT_t InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override;
	virtual DBOS_RESULT_t FROMISR_InitialWaitCheck() const override;
	virtual DBOS_RESULT_t PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override;
	virtual DBOS_RESULT_t GiveObject(DBOS_TASK_P_t const pRxTask) override;
	virtual DBOS_RESULT_t TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask) override;
	virtual DBOS_RESULT_t FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData) override;
	virtual DBOS_RESULT_t SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask) override;
	virtual DBOS_RESULT_t FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode) override;

	void Pause(void);
	void Run(void);
	void Reset(void);
	void ResetAndPause(void);
	void SetTime(DBOS_TIMECOUNT_t const Time);
	DBOS_TIMECOUNT_t GetTime(void) const;
	void Configure(INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime);

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails(void) const override;
	virtual void PrintTypeAndID(void) const override;
#endif

protected:

private:

	dBOS_TIMERFULLFEATURE m_Timer;

};

} // namespace

#endif /* DBOS_SYNCTIMER_HPP_ */
