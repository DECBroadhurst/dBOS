/**
 * @file dBOS_Register.hpp
 * @author David Broadhurst
 * @date 24/01/2015
 *
 * \brief Definition of register synchronization object.
 *
 */

#ifndef DBOS_REGISTER_HPP_
#define DBOS_REGISTER_HPP_

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
class dBOS_REGISTER : public dBOS_SYNC {

public:

	dBOS_REGISTER(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType);
	dBOS_REGISTER(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask);

	virtual ~dBOS_REGISTER();

	dBOS_REGISTER() = delete;
	dBOS_REGISTER(dBOS_REGISTER const &) = delete;
	dBOS_REGISTER& operator=(dBOS_REGISTER const &) = delete;

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

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails() const override;
	virtual void PrintTypeAndID() const override;
#endif

	DBOS_RESULT_t SetRegisterANDMask(SIGNALLED_DATA_t const Mask);
	DBOS_RESULT_t SetRegisterORMask(SIGNALLED_DATA_t const Mask);

	SIGNALLED_DATA_t GetRegisterState(void) const;

protected:

private:

	SIGNALLED_DATA_t m_SignalData;

	SIGNALLED_DATA_t m_Reg;
	SIGNALLED_DATA_t m_ORMask;
	SIGNALLED_DATA_t m_ANDMask;

};

} // namespace

#endif /* DBOS_REGISTER_HPP_ */
