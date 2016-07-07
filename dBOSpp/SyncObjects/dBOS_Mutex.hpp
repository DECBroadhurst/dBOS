/**
 * @file dBOS_Mutex.hpp
 * @author David Broadhurst
 * @date 17/01/2015
 *
 * \brief Definition of Mutex synchronization object.
 *
 */

#ifndef DBOS_MUTEX_HPP_
#define DBOS_MUTEX_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Sync.hpp"
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
class dBOS_MUTEX : public dBOS_SYNC {

public:

	dBOS_MUTEX(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType);
	virtual ~dBOS_MUTEX();

	virtual SIGNALLED_DATA_t GetSignalData(void) const override;
	virtual DBOS_TASK_P_t GetOwnerTask(void) const override;

	virtual DBOS_RESULT_t InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override = 0;
	virtual DBOS_RESULT_t FROMISR_InitialWaitCheck() const override;
	virtual DBOS_RESULT_t PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override = 0;
	virtual DBOS_RESULT_t GiveObject(DBOS_TASK_P_t const pRxTask) override = 0;
	virtual DBOS_RESULT_t TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask) override = 0;
	virtual DBOS_RESULT_t FROMISR_TakeObject(SIGNALLED_DATA_t * const pSignalData) override;
	virtual DBOS_RESULT_t SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask) override = 0;
	virtual DBOS_RESULT_t FROMISR_SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode) override;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintDetails() const override;
	virtual void PrintTypeAndID() const override;
#endif

	virtual SNATIVECPU_t GetCount(void) const;

	virtual DBOS_RESULT_t InsertTaskIntoObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink) override;
	virtual DBOS_TASK_P_t FromFrontRemoveTaskFromObjectWaitList(void) override;
	virtual DBOS_RESULT_t RemoveTaskFromObjectWaitList(dBOS_LISTNODE<dBOS_TASK> * const pTaskWaitLink) override;

protected:

	dBOS_MUTEX() = delete;
	dBOS_MUTEX(dBOS_MUTEX const &) = delete;
	dBOS_MUTEX& operator=(dBOS_MUTEX const &) = delete;

	SNATIVECPU_t m_Count; /**< Mutex Count. 																						*/
	DBOS_TASK_P_t m_pOwner; /**< Pointer to the Task which owns the mutex, set to NULL if mutex has not been acquired by a task. 	*/
	dBOS_LISTNODE<dBOS_MUTEX> m_Link; /**< Mutex objects are linked together as task acquire them. 									*/
	SIGNALLED_DATA_t m_SignalData;

private:

};

class dBOS_NONRECURSIVEMUTEX : public dBOS_MUTEX {

public:

	dBOS_NONRECURSIVEMUTEX(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType);
	virtual ~dBOS_NONRECURSIVEMUTEX();

	dBOS_NONRECURSIVEMUTEX() = delete;
	dBOS_NONRECURSIVEMUTEX(dBOS_NONRECURSIVEMUTEX const &) = delete;
	dBOS_NONRECURSIVEMUTEX& operator=(dBOS_NONRECURSIVEMUTEX const &) = delete;

	virtual DBOS_RESULT_t InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override;
	virtual DBOS_RESULT_t PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override;
	virtual DBOS_RESULT_t GiveObject(DBOS_TASK_P_t const pRxTask) override;
	virtual DBOS_RESULT_t TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask) override;
	virtual DBOS_RESULT_t SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask) override;

protected:

private:

};

class dBOS_RECURSIVEMUTEX : public dBOS_MUTEX {

public:

	dBOS_RECURSIVEMUTEX(DBOS_ID_t const ID, DBOS_UNIQUESYNCID_t const SyncID, DBOS_WAITLIST_SORT_t const WaitListType);
	virtual ~dBOS_RECURSIVEMUTEX();

	dBOS_RECURSIVEMUTEX() = delete;
	dBOS_RECURSIVEMUTEX(dBOS_RECURSIVEMUTEX const &) = delete;
	dBOS_RECURSIVEMUTEX& operator=(dBOS_RECURSIVEMUTEX const &) = delete;

	virtual DBOS_RESULT_t InitialWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override;
	virtual DBOS_RESULT_t PostWaitCheck(DBOS_TASK_CONST_P_t const pCurrentTask) const override;
	virtual DBOS_RESULT_t GiveObject(DBOS_TASK_P_t const pRxTask) override;
	virtual DBOS_RESULT_t TakeObject(SIGNALLED_DATA_t * const pSignalData, DBOS_TASK_P_t const pCurrentTask) override;
	virtual DBOS_RESULT_t SignalObject(SIGNALLED_DATA_t const SignalData, DBOS_SIGNAL_MODE_t const SignalMode, DBOS_TASK_P_t const pCurrentTask) override;

protected:

private:

	DBOS_TASK_P_t m_pRxTask; /**< Pointer to the task the mutex has been given to. */

};

} // namespace

#endif /* DBOS_MUTEX_HPP_ */
