/**
 * @file TestFlag.cpp
 * @author David Broadhurst
 * @date 13/07/2015
 *
 * \brief Implementation of unit tests for flags.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "TestFlag.hpp"
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
static dBOS_TASKINTERFACE_P_t TaskThree;

static dBOS_FLAGINTERFACE_P_t FlagOne;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// Test 1 - Testing Basic Flag Functionality
static void TestFlag_Test1_IdleTask(void * D);
static void TestFlag_Test1_TaskOneFunc(void * D);
static void TestFlag_Test1_TaskTwoFunc(void * D);

static void TestFlag_Test1_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestFlag_Test1_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	pK->Yield();

	DBOS_Test_LogEvent('C');

	DBOS_TEST_INT32U(FlagOne->GetFlagState(), 1U)

	DBOS_TEST_RESULT(pK->Object_Wait(FlagOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, 5);

	DBOS_TEST_INT32U(FlagOne->GetFlagState(), 0U)
	// Test flag taken

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_Wait(FlagOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 7);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_INT32U(FlagOne->GetFlagState(), 0U)
	// Test flag taken

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('G');

	pK->Sleep(5000);

	while(1){

	}

}

static void TestFlag_Test1_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Signal(FlagOne, 5, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	pK->Yield();

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Signal(FlagOne, 7, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('H');

	DBOS_Test_CheckEventLog("ABCDEFGH");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestFlag_Test1(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestFlag_Test1_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestFlag_Test1_TaskTwoFunc, NULLPTR);

	FlagOne = pK->CreateFlag("Flag One", PRIORITYLIST, 1U);

	pK->Start("Idle Task", 128, TestFlag_Test1_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 2 - Uses SIGNAL_AUTO as opposed to Test 3 below which uses SIGNAL_ONE
// Tasks 1 and 2 both run to completion on the first signal of FlagOne.
static void TestFlag_Test2_IdleTask(void * D);
static void TestFlag_Test2_TaskOneFunc(void * D);
static void TestFlag_Test2_TaskTwoFunc(void * D);
static void TestFlag_Test2_TaskThreeFunc(void * D);

static void TestFlag_Test2_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestFlag_Test2_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(FlagOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 8);

	DBOS_Test_LogEvent('D');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestFlag_Test2_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(FlagOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 8);

	DBOS_Test_LogEvent('E');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestFlag_Test2_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Signal(FlagOne, 8, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Signal(FlagOne, 9, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFG");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestFlag_Test2(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestFlag_Test2_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestFlag_Test2_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 10, 196, TestFlag_Test2_TaskThreeFunc, NULLPTR);

	FlagOne = pK->CreateFlag("Flag One", PRIORITYLIST, 1U);

	pK->Start("Idle Task", 128, TestFlag_Test2_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 3 - Uses SIGNAL_ONE as opposed to Test 2 above which uses SIGNAL_AUTO
// Tasks 1 and 2 run in turn, only one of the two running when flag one is signaled.
static void TestFlag_Test3_IdleTask(void * D);
static void TestFlag_Test3_TaskOneFunc(void * D);
static void TestFlag_Test3_TaskTwoFunc(void * D);
static void TestFlag_Test3_TaskThreeFunc(void * D);

static void TestFlag_Test3_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestFlag_Test3_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(FlagOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED); // 1
	DBOS_TEST_SIGNALDATA(SignalData, 8);

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_Wait(FlagOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED); // 3
	DBOS_TEST_SIGNALDATA(SignalData, 10);

	DBOS_Test_LogEvent('H');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestFlag_Test3_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(FlagOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED); // 2
	DBOS_TEST_SIGNALDATA(SignalData, 9);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Wait(FlagOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED); // 4
	DBOS_TEST_SIGNALDATA(SignalData, 11);

	DBOS_Test_LogEvent('J');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestFlag_Test3_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Signal(FlagOne, 8, SIGNAL_ONE, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // 1

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Signal(FlagOne, 9, SIGNAL_ONE, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // 2

	DBOS_Test_LogEvent('G');

	DBOS_TEST_RESULT(pK->Object_Signal(FlagOne, 10, SIGNAL_ONE, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // 3

	DBOS_Test_LogEvent('I');

	DBOS_TEST_RESULT(pK->Object_Signal(FlagOne, 11, SIGNAL_ONE, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // 4

	DBOS_Test_LogEvent('K');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFGHIJK");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestFlag_Test3(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestFlag_Test3_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestFlag_Test3_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 10, 196, TestFlag_Test3_TaskThreeFunc, NULLPTR);

	FlagOne = pK->CreateFlag("Flag One", PRIORITYLIST, 1U);

	pK->Start("Idle Task", 128, TestFlag_Test3_IdleTask);

	while(1){

	}

	return;
}

} // namespace
