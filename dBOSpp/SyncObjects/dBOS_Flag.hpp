/**
 * @file dBOS_Flag.hpp
 * @author David Broadhurst
 * @date 24/01/2015
 *
 * \brief Definition of Flag synchronization object.
 *
 */

#ifndef DBOS_FLAG_HPP_
#define DBOS_FLAG_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Sync.hpp"

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
class dBOS_FLAG : public dBOS_SYNC {

public:

	dBOS_FLAG(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType);
	dBOS_FLAG(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask);

	virtual ~dBOS_FLAG();

	dBOS_FLAG() = delete;
	dBOS_FLAG(dBOS_FLAG const &) = delete;
	dBOS_FLAG& operator=(dBOS_FLAG const &) = delete;

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

	UNATIVECPU_t GetFlagState(void) const;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails() const override;
	virtual void PrintTypeAndID() const override;
#endif

protected:

private:

	SIGNALLED_DATA_t m_SignalData;
	UNATIVECPU_t m_Flag;

};

} // namespace

#endif /* DBOS_FLAG_HPP_ */
