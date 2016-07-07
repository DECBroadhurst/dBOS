/**
 * @file dBOS_SyncInterface.hpp
 * @author David Broadhurst
 * @date 19/02/2015
 *
 * \brief Definition of Interfaces to Sync Objects.
 *
 */

#ifndef DBOS_SYNCINTERFACE_HPP_
#define DBOS_SYNCINTERFACE_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Core_DataTypes.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// FORWARD DECLERATIONS
class dBOS_SYNCINTERFACE;

class dBOS_FLAGINTERFACE;
class dBOS_MUTEXINTERFACE;
class dBOS_REGISTERINTERFACE;
class dBOS_SEMAPHOREINTERFACE;
class dBOS_SIGNALINTERFACE;
class dBOS_QUEUEINTERFACE;
class dBOS_TIMERINTERFACE;

// -------------------------------------------------------------------------------------------------
// DATA TYPES
typedef dBOS_SYNCINTERFACE * dBOS_SYNCINTERFACE_P_t;

typedef dBOS_FLAGINTERFACE * dBOS_FLAGINTERFACE_P_t;
typedef dBOS_MUTEXINTERFACE * dBOS_MUTEXINTERFACE_P_t;
typedef dBOS_REGISTERINTERFACE * dBOS_REGISTERINTERFACE_P_t;
typedef dBOS_SEMAPHOREINTERFACE * dBOS_SEMAPHOREINTERFACE_P_t;
typedef dBOS_SIGNALINTERFACE * dBOS_SIGNALINTERFACE_P_t;
typedef dBOS_QUEUEINTERFACE * dBOS_QUEUEINTERFACE_P_t;
typedef dBOS_TIMERINTERFACE * dBOS_TIMERINTERFACE_P_t;

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// CLASSES
class dBOS_SYNCINTERFACE {

public:

	dBOS_SYNCINTERFACE();
	virtual ~dBOS_SYNCINTERFACE();

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails(void) const;
	virtual void PrintTypeAndID(void) const;
	virtual void PrintWaitList(void) const;
#endif

	virtual dBOS_TASKINTERFACE_CONST_P_t GetTaskAtWaitListPosition(UNATIVECPU_t Position) const;

protected:

	dBOS_SYNCINTERFACE(dBOS_SYNCINTERFACE const &) = delete;
	dBOS_SYNCINTERFACE& operator=(dBOS_SYNCINTERFACE const &) = delete;

private:

	friend class dBOS_KERNELINTERFACE;	// Kernel can call GetSyncObject and GetConstSyncObject

	virtual DBOS_SYNCBASE_CONST_P_t GetConstSyncObject(void) const = 0;
	virtual DBOS_SYNCBASE_P_t GetSyncObject(void) = 0;

};

class dBOS_FLAGINTERFACE : public dBOS_SYNCINTERFACE {

public:

	explicit dBOS_FLAGINTERFACE(DBOS_FLAG_P_t const pImpl);

	virtual ~dBOS_FLAGINTERFACE();

	dBOS_FLAGINTERFACE() = delete;
	dBOS_FLAGINTERFACE(dBOS_FLAGINTERFACE const &) = delete;
	dBOS_FLAGINTERFACE& operator=(dBOS_FLAGINTERFACE const &) = delete;

	UNATIVECPU_t GetFlagState(void) const;

protected:

private:

	virtual DBOS_SYNCBASE_CONST_P_t GetConstSyncObject(void) const override;
	virtual DBOS_SYNCBASE_P_t GetSyncObject(void) override;

	DBOS_FLAG_P_t const m_pImpl;

};

class dBOS_MUTEXINTERFACE : public dBOS_SYNCINTERFACE {

public:

	explicit dBOS_MUTEXINTERFACE(DBOS_MUTEX_P_t const pImpl);

	virtual ~dBOS_MUTEXINTERFACE();

	dBOS_MUTEXINTERFACE() = delete;
	dBOS_MUTEXINTERFACE(dBOS_MUTEXINTERFACE const &) = delete;
	dBOS_MUTEXINTERFACE& operator=(dBOS_MUTEXINTERFACE const &) = delete;

	SNATIVECPU_t GetCount(void) const;

protected:

private:

	virtual DBOS_SYNCBASE_CONST_P_t GetConstSyncObject(void) const override;
	virtual DBOS_SYNCBASE_P_t GetSyncObject(void) override;

	DBOS_MUTEX_P_t const m_pImpl;

};

class dBOS_REGISTERINTERFACE : public dBOS_SYNCINTERFACE {

public:

	explicit dBOS_REGISTERINTERFACE(DBOS_REGISTER_P_t const pImpl);

	virtual ~dBOS_REGISTERINTERFACE();

	dBOS_REGISTERINTERFACE() = delete;
	dBOS_REGISTERINTERFACE(dBOS_REGISTERINTERFACE const &) = delete;
	dBOS_REGISTERINTERFACE& operator=(dBOS_REGISTERINTERFACE const &) = delete;

	DBOS_RESULT_t SetRegisterANDMask(SIGNALLED_DATA_t const Mask);
	DBOS_RESULT_t SetRegisterORMask(SIGNALLED_DATA_t const Mask);

	SIGNALLED_DATA_t GetRegisterState(void) const;

protected:

private:

	virtual DBOS_SYNCBASE_CONST_P_t GetConstSyncObject(void) const override;
	virtual DBOS_SYNCBASE_P_t GetSyncObject(void) override;

	DBOS_REGISTER_P_t const m_pImpl;

};

class dBOS_SEMAPHOREINTERFACE : public dBOS_SYNCINTERFACE {

public:

	explicit dBOS_SEMAPHOREINTERFACE(DBOS_SEMAPHORE_P_t const pImpl);

	virtual ~dBOS_SEMAPHOREINTERFACE();

	dBOS_SEMAPHOREINTERFACE() = delete;
	dBOS_SEMAPHOREINTERFACE(dBOS_SEMAPHOREINTERFACE const &) = delete;
	dBOS_SEMAPHOREINTERFACE& operator=(dBOS_SEMAPHOREINTERFACE const &) = delete;

	SNATIVECPU_t GetCount(void) const;

protected:

private:

	virtual DBOS_SYNCBASE_CONST_P_t GetConstSyncObject(void) const override;
	virtual DBOS_SYNCBASE_P_t GetSyncObject(void) override;

	DBOS_SEMAPHORE_P_t const m_pImpl;

};

class dBOS_SIGNALINTERFACE : public dBOS_SYNCINTERFACE {

public:

	explicit dBOS_SIGNALINTERFACE(DBOS_SIGNAL_P_t const pImpl);

	virtual ~dBOS_SIGNALINTERFACE();

	dBOS_SIGNALINTERFACE() = delete;
	dBOS_SIGNALINTERFACE(dBOS_SIGNALINTERFACE const &) = delete;
	dBOS_SIGNALINTERFACE& operator=(dBOS_SIGNALINTERFACE const &) = delete;

protected:

private:

	virtual DBOS_SYNCBASE_CONST_P_t GetConstSyncObject(void) const override;
	virtual DBOS_SYNCBASE_P_t GetSyncObject(void) override;

	DBOS_SIGNAL_P_t const m_pImpl;

};

class dBOS_TIMERINTERFACE : public dBOS_SYNCINTERFACE {

public:

	explicit dBOS_TIMERINTERFACE(DBOS_TIMER_P_t const pImpl);

	virtual ~dBOS_TIMERINTERFACE();

	dBOS_TIMERINTERFACE() = delete;
	dBOS_TIMERINTERFACE(dBOS_TIMERINTERFACE const &) = delete;
	dBOS_TIMERINTERFACE& operator=(dBOS_TIMERINTERFACE const &) = delete;

	void Pause(void);
	void Run(void);
	void Reset(void);
	void ResetAndPause(void);
	void SetTime(DBOS_TIMECOUNT_t const Time);
	DBOS_TIMECOUNT_t GetTime(void) const;
	void Configure(INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime);

protected:

private:

	virtual DBOS_SYNCBASE_CONST_P_t GetConstSyncObject(void) const override;
	virtual DBOS_SYNCBASE_P_t GetSyncObject(void) override;

	DBOS_TIMER_P_t const m_pImpl;

};

} // namespace

#endif /* DBOS_SYNCINTERFACE_HPP_ */
