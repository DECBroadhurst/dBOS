/**
 * @file TestConditionVariable.cpp
 * @author David Broadhurst
 * @date 6/09/2015
 *
 * \brief Implementation of unit tests for condition variables.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "TestConditionVariable.hpp"
#include "dBOS_KernelInterface.hpp"
#include "TestSys.hpp"
#include "dBOS_DebugPrint.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// LOCAL DEFINES

// -------------------------------------------------------------------------------------------------
// LOCAL CONSTANTS

// -------------------------------------------------------------------------------------------------
// LOCAL DATA TYPES

// -------------------------------------------------------------------------------------------------
// LOCAL TABLES

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES
static dBOS_TASKINTERFACE_P_t TaskOne;
static dBOS_TASKINTERFACE_P_t TaskTwo;

static dBOS_SIGNALINTERFACE_P_t SignalOne;
static dBOS_MUTEXINTERFACE_P_t MutexOne;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// Test 1
static void TestConditionVariable_Test1_IdleTask(void * D);
static void TestConditionVariable_Test1_TaskOneFunc(void * D);
static void TestConditionVariable_Test1_TaskTwoFunc(void * D);

static void TestConditionVariable_Test1_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestConditionVariable_Test1_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('B');

	pK->Yield();

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_WaitCV(SignalOne, MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_SIGNALDATA(SignalData, 0x03U);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestConditionVariable_Test1_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Signal(SignalOne, 0x03U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 0, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('H');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFGH");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestConditionVariable_Test1(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestConditionVariable_Test1_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestConditionVariable_Test1_TaskTwoFunc, NULLPTR);

	SignalOne = pK->CreateSignal("Signal One", PRIORITYLIST, 1U);
	MutexOne = pK->CreateNonRecursiveMutex("Mutex One", PRIORITYLIST);

	pK->Start("Idle Task", 128, TestConditionVariable_Test1_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 2
// Same as Test 1 but task two doesn't acquire the mutex.

static void TestConditionVariable_Test2_IdleTask(void * D);
static void TestConditionVariable_Test2_TaskOneFunc(void * D);
static void TestConditionVariable_Test2_TaskTwoFunc(void * D);

static void TestConditionVariable_Test2_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestConditionVariable_Test2_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_WaitCV(SignalOne, MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_SIGNALDATA(SignalData, 0x03U);

	DBOS_Test_LogEvent('D');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestConditionVariable_Test2_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Signal(SignalOne, 0x03U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('E');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDE");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestConditionVariable_Test2(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestConditionVariable_Test2_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestConditionVariable_Test2_TaskTwoFunc, NULLPTR);

	SignalOne = pK->CreateSignal("Signal One", PRIORITYLIST, 1U);
	MutexOne = pK->CreateNonRecursiveMutex("Mutex One", PRIORITYLIST);

	pK->Start("Idle Task", 128, TestConditionVariable_Test2_IdleTask);

	while(1){

	}

	return;
}

} // namespace
