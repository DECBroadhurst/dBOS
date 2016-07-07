/**
 * @file TestWaitOnMultiple.cpp
 * @author David Broadhurst
 * @date 28/06/2015
 *
 * \brief Implementation of unit tests for WaitOnMultiple.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "TestWaitOnMultiple.hpp"
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
static dBOS_SIGNALINTERFACE_P_t SignalTwo;
static dBOS_SIGNALINTERFACE_P_t SignalThree;
static dBOS_SIGNALINTERFACE_P_t SignalFour;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// Test 1 - Testing Correct Object is reported as signaled. Checks timeouts are reported correctly.
static void TestWaitOnMultiple_Test1_IdleTask(void * D);
static void TestWaitOnMultiple_Test1_TaskOneFunc(void * D);
static void TestWaitOnMultiple_Test1_TaskTwoFunc(void * D);
static void TestWaitOnMultiple_Test1_TaskThreeFunc(void * D);

static void TestWaitOnMultiple_Test1_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){
#if (DBOS_PREEMPTIONENABLED == 0U)
		pK->Yield();
#endif
	}

}

static void TestWaitOnMultiple_Test1_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	dBOS_SYNCINTERFACE_P_t pObjects[] = { SignalOne, SignalTwo, SignalThree, SignalFour };
	SNATIVECPU_t Index;

	DBOS_Test_LogEvent('A');

	// Check the object which is signaled is reported correctly
	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 4, 50, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_SIGNALDATA(SignalData, 5U);
	DBOS_TEST_INT32S(Index, 0);

	DBOS_Test_LogEvent('D');

	// Check the object which is signaled is reported correctly
	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 4, 50, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_SIGNALDATA(SignalData, 8U);
	DBOS_TEST_INT32S(Index, 2);

	DBOS_Test_LogEvent('G');

	// Check timeout is reported correctly
	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 4, 50, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED_TIMEOUT)
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);
	DBOS_TEST_INT32S(Index, -1);

	DBOS_Test_LogEvent('J');

	// Check timeout is reported correctly
	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 4, 0, &Index, &SignalData), DBOS_OBJECT_WAIT_TIMEOUT)
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);
	DBOS_TEST_INT32S(Index, -1);

	DBOS_Test_LogEvent('K');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestWaitOnMultiple_Test1_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	dBOS_SYNCINTERFACE_P_t pObjects[] = { SignalOne, SignalTwo, SignalThree, SignalFour };
	SNATIVECPU_t Index;

	DBOS_Test_LogEvent('B');

	// Check the object which is signaled is reported correctly
	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 4, 50, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_SIGNALDATA(SignalData, 5U);
	DBOS_TEST_INT32S(Index, 0);

	DBOS_Test_LogEvent('E');

	// Check the object which is signaled is reported correctly
	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 4, 50, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_SIGNALDATA(SignalData, 8U);
	DBOS_TEST_INT32S(Index, 2);

	DBOS_Test_LogEvent('H');

	// Check timeout is reported correctly
	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 4, 50, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED_TIMEOUT)
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);
	DBOS_TEST_INT32S(Index, -1);

	DBOS_Test_LogEvent('L');

	// Check timeout is reported correctly
	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 4, 0, &Index, &SignalData), DBOS_OBJECT_WAIT_TIMEOUT)
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);
	DBOS_TEST_INT32S(Index, -1);

	DBOS_Test_LogEvent('M');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestWaitOnMultiple_Test1_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Signal(SignalOne, 5U, SIGNAL_ALL, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED)

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Signal(SignalThree, 8U, SIGNAL_ALL, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED)

	DBOS_Test_LogEvent('I');

	pK->Sleep(500);

	DBOS_Test_LogEvent('N');

	DBOS_Test_CheckEventLog("ABCDEFGHIJKLMN");

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(20000);

	while(1){

	}

}

void TestWaitOnMultiple_Test1(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestWaitOnMultiple_Test1_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestWaitOnMultiple_Test1_TaskTwoFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Three", 10, 196, TestWaitOnMultiple_Test1_TaskThreeFunc, NULLPTR);

	SignalOne = pK->CreateSignal("Signal One", PRIORITYLIST, 1U);
	SignalTwo = pK->CreateSignal("Signal Two", PRIORITYLIST, 1U);
	SignalThree = pK->CreateSignal("Signal Three", PRIORITYLIST, 1U);
	SignalFour = pK->CreateSignal("Signal Four", PRIORITYLIST, 1U);

	pK->Start("Idle Task", 128, TestWaitOnMultiple_Test1_IdleTask);

	while(1){

	}

	return;
}

} // namespace
