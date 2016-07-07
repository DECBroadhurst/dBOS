/**
 * @file dBOS_TaskTimer.hpp
 * @author David Broadhurst
 * @date 12/04/2015
 *
 * \brief Definition of task timer synchronization object.
 *
 */

#ifndef SYNCOBJECTS_DBOS_TASKTIMER_HPP_
#define SYNCOBJECTS_DBOS_TASKTIMER_HPP_

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
class dBOS_TASKTIMER : public dBOS_BASICSYNC {

public:

	dBOS_TASKTIMER(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID);
	virtual ~dBOS_TASKTIMER();

	dBOS_TASKTIMER() = delete;
	dBOS_TASKTIMER(dBOS_TASKTIMER const &) = delete;
	dBOS_TASKTIMER& operator=(dBOS_TASKTIMER const &) = delete;

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

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails(void) const override;
	virtual void PrintTypeAndID(void) const override;
#endif

protected:

private:

	dBOS_TIMERCOUNTDOWN m_Timer;

};

} // namespace

#endif /* SYNCOBJECTS_DBOS_TASKTIMER_HPP_ */
