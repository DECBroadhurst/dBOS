/**
 * @file dBOS_KernelInterface.hpp
 * @author David Broadhurst
 * @date 5/03/2015
 *
 * \brief Definition of dBOS Kernel Interface.
 *
 */

#ifndef DBOS_KERNELINTERFACE_HPP_
#define DBOS_KERNELINTERFACE_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_DataTypes.hpp"
#include "dBOS_SyncInterface.hpp"
#include "dBOS_TaskInterface.hpp"
#include "dBOS_QueueInterface.hpp"

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
class dBOS_KERNELINTERFACE {

public:

	static DBOS_RESULT_t Init(void);
	static dBOS_KERNELINTERFACE * GetInstance(void);

	DBOS_RESULT_t Start(DBOS_ID_t const ID, DBOS_PNTEQUIV_t const TaskStackSize, DBOS_TASKENTRY_FUNCTION_PNT_t const pTaskFunction);
	DBOS_RESULT_t IdleTaskStarted(void);

	DBOS_RESULT_t Sleep(DBOS_TIMECOUNT_t const Time);
	DBOS_RESULT_t Yield();

	DBOS_TASK_PRIORITY_t GetCurrentTaskCurrentPriority() const;
	DBOS_TIMECOUNT_t GetElapsedTime() const;

	DBOS_RESULT_t Object_Take(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t * const pSignalData);
	DBOS_RESULT_t Object_TakeFROMISR(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t * const pSignalData);
	DBOS_RESULT_t Object_Wait(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t * const pSignalData, DBOS_TIMECOUNT_t const Time);
	DBOS_RESULT_t Object_WaitCV(dBOS_SYNCINTERFACE * const pObject, dBOS_MUTEXINTERFACE * const pMutex, SIGNALLED_DATA_t * const pSignalData, DBOS_TIMECOUNT_t const Time);
	DBOS_RESULT_t Object_WaitMultiple(dBOS_SYNCINTERFACE * const pObjects[], UNATIVECPU_t const NumOfObjects, DBOS_TIMECOUNT_t const Time, SNATIVECPU_t * const pSignalledObjectIndex, SIGNALLED_DATA_t * const pSignalData);
	DBOS_RESULT_t Object_Signal(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t const SignalledData, DBOS_SIGNAL_MODE_t const SignalMode, BOOL const SwitchToTaskAtHeadOfWaitList);
	DBOS_RESULT_t Object_SignalFROMISR(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t const SignalledData, DBOS_SIGNAL_MODE_t const SignalMode);
	DBOS_RESULT_t Object_SignalAndWait(dBOS_SYNCINTERFACE * const pObject, SIGNALLED_DATA_t const SignalledDataIn, DBOS_SIGNAL_MODE_t const SignalMode, BOOL const SwitchToTaskAtHeadOfWaitList, SIGNALLED_DATA_t * const pSignalDataOut,
			DBOS_TIMECOUNT_t const Time);

	DBOS_RESULT_t Queue_WriteToBack(dBOS_QUEUEINTERFACE_P_t const pQueue, void const * const pData, DBOS_TIMECOUNT_t const Time);
	DBOS_RESULT_t Queue_WriteToFront(dBOS_QUEUEINTERFACE_P_t const pQueue, void const * const pData, DBOS_TIMECOUNT_t const Time);
	DBOS_RESULT_t Queue_WriteMultipleToBack(dBOS_QUEUEINTERFACE_P_t const * const pQueue, UNATIVECPU_t const NumberOfQueues, void const * const pData, DBOS_TIMECOUNT_t const Time);
	DBOS_RESULT_t Queue_WriteMultipleToFront(dBOS_QUEUEINTERFACE_P_t const * const pQueue, UNATIVECPU_t const NumberOfQueues, void const * const pData, DBOS_TIMECOUNT_t const Time);
	DBOS_RESULT_t Queue_ReadFromFront(dBOS_QUEUEINTERFACE_P_t const pQueue, void * const pData, DBOS_TIMECOUNT_t const Time);
	DBOS_RESULT_t Queue_ReadFromBack(dBOS_QUEUEINTERFACE_P_t const pQueue, void * const pData, DBOS_TIMECOUNT_t const Time);

	dBOS_FLAGINTERFACE_P_t CreateFlag(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType);
	dBOS_FLAGINTERFACE_P_t CreateFlag(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask);

	dBOS_MUTEXINTERFACE_P_t CreateNonRecursiveMutex(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType);
	dBOS_MUTEXINTERFACE_P_t CreateRecursiveMutex(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType);

	dBOS_REGISTERINTERFACE_P_t CreateRegister(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType);
	dBOS_REGISTERINTERFACE_P_t CreateRegister(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask);

	dBOS_SEMAPHOREINTERFACE_P_t CreateSemaphore(DBOS_ID_t const ID, SNATIVECPU_t const InitialCount, SNATIVECPU_t const MaxCount, DBOS_WAITLIST_SORT_t const WaitListType);
	dBOS_SEMAPHOREINTERFACE_P_t CreateSemaphore(DBOS_ID_t const ID, SNATIVECPU_t const InitialCount, SNATIVECPU_t const MaxCount, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask);

	dBOS_SIGNALINTERFACE_P_t CreateSignal(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType);
	dBOS_SIGNALINTERFACE_P_t CreateSignal(DBOS_ID_t const ID, DBOS_WAITLIST_SORT_t const WaitListType, INTERRUPT_MASK_t const Mask);

	dBOS_QUEUEINTERFACE_P_t CreateQueue(DBOS_ID_t const ID, UNATIVECPU_t const size, UNATIVECPU_t const ItemSize);
	dBOS_QUEUEINTERFACE_P_t CreateQueue(DBOS_ID_t const ID, UNATIVECPU_t const size, UNATIVECPU_t const ItemSize, INTERRUPT_MASK_t const Mask);

	dBOS_TIMERINTERFACE_P_t CreateTimer(DBOS_ID_t const ID, INT8U const Flags, DBOS_TIMECOUNT_t const ReloadTime, DBOS_TIMECOUNT_t const InitialTime, DBOS_WAITLIST_SORT_t const WaitListType);

	dBOS_TASKINTERFACE_P_t CreateTask(DBOS_ID_t const ID, DBOS_TASK_PRIORITY_t const Priority, DBOS_PNTEQUIV_t const TaskStackSize, DBOS_TASKENTRY_FUNCTION_PNT_t const pTaskFunction, void const * const pTaskEntryData);

	dBOS_TASKINTERFACE_CONST_P_t GetTaskAtReadyListPosition(UNATIVECPU_t Position) const;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

	void PrintReadyTaskList(void) const;

#endif

protected:

	dBOS_KERNELINTERFACE();
	~dBOS_KERNELINTERFACE();
	dBOS_KERNELINTERFACE(dBOS_KERNELINTERFACE const &) = delete;
	dBOS_KERNELINTERFACE& operator=(dBOS_KERNELINTERFACE const &) = delete;

private:

	DBOS_RESULT_t Queue_WriteMultiple(dBOS_QUEUEINTERFACE_P_t const * const pQueue, UNATIVECPU_t const NumberOfQueues, void const * const pData, DBOS_TIMECOUNT_t const Time, BOOL const WriteToBack);
	DBOS_RESULT_t Queue_Write(dBOS_QUEUEINTERFACE_P_t const pQueue, void const * const pData, DBOS_TIMECOUNT_t const Time, BOOL const WriteToBack);
	DBOS_RESULT_t Queue_Read(dBOS_QUEUEINTERFACE_P_t const pQueue, void * const pData, DBOS_TIMECOUNT_t const Time, BOOL const ReadFromFront);

	static class dBOS_KERNELINTERFACE * m_pInstance;

	static class dBOS_KERNEL * m_pImpl;
	static class dBOS_TIMERMANAGER const * m_pTimerManager;

};

} // namespace

#endif /* DBOS_KERNELINTERFACE_HPP_ */
