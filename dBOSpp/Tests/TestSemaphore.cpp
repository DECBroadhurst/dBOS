/**
 * @file TestSemaphore.cpp
 * @author David Broadhurst
 * @date 13/07/2015
 *
 * \brief Implementation of unit tests for Semaphores.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "TestSemaphore.hpp"
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
static dBOS_TASKINTERFACE_P_t TaskFour;

static dBOS_SEMAPHOREINTERFACE_P_t SemaphoreOne;
static dBOS_SEMAPHOREINTERFACE_P_t SemaphoreTwo;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// Test 1 - Testing Basic Semaphore Functionality, Increments and Decrements as expected.
static void TestSemaphore_Test1_IdleTask(void * D);
static void TestSemaphore_Test1_TaskOneFunc(void * D);
static void TestSemaphore_Test1_TaskTwoFunc(void * D);

static void TestSemaphore_Test1_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestSemaphore_Test1_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	DBOS_TEST_INT32S(SemaphoreOne->GetCount(), 0);

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 5, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	DBOS_TEST_INT32S(SemaphoreOne->GetCount(), 1);

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 6, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	DBOS_TEST_INT32S(SemaphoreOne->GetCount(), 2);

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 7, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	DBOS_TEST_INT32S(SemaphoreOne->GetCount(), 2); // Already Reached Max Count.

	DBOS_Test_LogEvent('B');

	pK->Yield();

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 8, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('H');

	DBOS_TEST_INT32S(SemaphoreOne->GetCount(), 0);

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('I');

	DBOS_Test_CheckEventLog("ABCDEFGHI");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSemaphore_Test1_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	DBOS_TEST_INT32S(SemaphoreOne->GetCount(), 2);

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, 7);

	DBOS_Test_LogEvent('D');

	DBOS_TEST_INT32S(SemaphoreOne->GetCount(), 1);

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, 7);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_INT32S(SemaphoreOne->GetCount(), 0);

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 8);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_INT32S(SemaphoreOne->GetCount(), 0);

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestSemaphore_Test1(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSemaphore_Test1_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSemaphore_Test1_TaskTwoFunc, NULLPTR);

	SemaphoreOne = pK->CreateSemaphore("Semaphore One", 0, 2, PRIORITYLIST, 1U); // Initial Count 0, Max Count 2.

	pK->Start("Idle Task", 128, TestSemaphore_Test1_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 2 - Testing Two Tasks waiting to take the semaphore, CHECKS THEY TAKE THE SEMAPHORE IN TURN, THEY ARE THE SAME PRIORITY.
static void TestSemaphore_Test2_IdleTask(void * D);
static void TestSemaphore_Test2_TaskOneFunc(void * D);
static void TestSemaphore_Test2_TaskTwoFunc(void * D);

static void TestSemaphore_Test2_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestSemaphore_Test2_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	pK->Yield();

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // 1

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // 2

	DBOS_Test_LogEvent('H');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // 3

	DBOS_Test_LogEvent('J');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // 4

	DBOS_Test_LogEvent('L');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // 5

	DBOS_Test_LogEvent('N');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // 6

	DBOS_Test_LogEvent('P');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFGHIJKLMNOP");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSemaphore_Test2_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED); // 1
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED); // 3
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('I');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED); // 5
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('M');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSemaphore_Test2_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED); // 2
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED); // 4
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('K');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED); // 6
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('O');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestSemaphore_Test2(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSemaphore_Test2_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSemaphore_Test2_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 10, 196, TestSemaphore_Test2_TaskThreeFunc, NULLPTR);

	SemaphoreOne = pK->CreateSemaphore("Semaphore One", 0, 2, PRIORITYLIST, 1U); // Initial Count 0, Max Count 2.

	pK->Start("Idle Task", 128, TestSemaphore_Test2_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 3 - Testing Two Tasks waiting to take the semaphore, USE OF SIGNAL_ALL WHEN SIGNALLING THE SEMAPHORE.
// Task 3 should never get to take the semaphore.
// Task 2 is set waiting on the semaphore, task two is first in the wait list.
// Task 3 is set waiting on the semaphore, task three is second in the wait list.
// Semaphore is signaled by task 1.
// Task 2 takes the semaphore, runs and waits on the semaphore, it is first in the wait list.
// Task 3 runs, finds the semaphore count is 0, and is set waiting again, this happens internal to the OS, Task 3 is second in the waitlist.
// Process repeats.
static void TestSemaphore_Test3_IdleTask(void * D);
static void TestSemaphore_Test3_TaskOneFunc(void * D);
static void TestSemaphore_Test3_TaskTwoFunc(void * D);

static void TestSemaphore_Test3_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestSemaphore_Test3_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	pK->Yield();

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_ALL, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_ALL, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('H');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_ALL, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('J');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDE(FG(HI(J"); // ( Event is from the OS, see method dBOS_KERNEL::WaitCurrentTaskOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSemaphore_Test3_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('I');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());
	;

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSemaphore_Test3_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('Z'); // <- SHOULD NEVER GET HERE.

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestSemaphore_Test3(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSemaphore_Test3_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSemaphore_Test3_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 10, 196, TestSemaphore_Test3_TaskThreeFunc, NULLPTR);

	SemaphoreOne = pK->CreateSemaphore("Semaphore One", 0, 2, PRIORITYLIST, 1U); // Initial Count 0, Max Count 2.

	pK->Start("Idle Task", 128, TestSemaphore_Test3_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 4 - Testing Two Tasks waiting to take the semaphore, USE OF SIGNAL_ALL WHEN SIGNALLING THE SEMAPHORE.
// SAME AS TEST 3 BUT USING Object_WaitMultiple
// Task 3 should never get to take the semaphore.
// Task 2 is set waiting on the semaphore, task two is first in the wait list.
// Task 3 is set waiting on the semaphore, task three is second in the wait list.
// Semaphore is signaled by task 1.
// Task 2 takes the semaphore, runs and waits on the semaphore, it is first in the wait list.
// Task 3 runs, finds the semaphore count is 0, and is set waiting again, this happens internal to the OS, Task 3 is second in the waitlist.
// Process repeats.
static void TestSemaphore_Test4_IdleTask(void * D);
static void TestSemaphore_Test4_TaskOneFunc(void * D);
static void TestSemaphore_Test4_TaskTwoFunc(void * D);
static void TestSemaphore_Test4_TaskThreeFunc(void * D);

static void TestSemaphore_Test4_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestSemaphore_Test4_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	pK->Yield();

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_ALL, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_ALL, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('H');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_ALL, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('J');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDE^FG^HI^J"); // ^ Event is from the OS, see method dBOS_KERNEL::CheckAndWaitCurrentTaskOnMultipleObjects

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSemaphore_Test4_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('I');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());
	;

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSemaphore_Test4_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	dBOS_SYNCINTERFACE_P_t pObjects[] = { SemaphoreOne, SemaphoreTwo };
	SNATIVECPU_t Index;

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 2, TIMEWAITFOREVER, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)

	DBOS_Test_LogEvent('Z'); // <- SHOULD NEVER GET HERE.

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestSemaphore_Test4(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSemaphore_Test4_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSemaphore_Test4_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 10, 196, TestSemaphore_Test4_TaskThreeFunc, NULLPTR);

	SemaphoreOne = pK->CreateSemaphore("Semaphore One", 0, 2, PRIORITYLIST, 1U); // Initial Count 0, Max Count 2.
	SemaphoreTwo = pK->CreateSemaphore("Semaphore Two", 0, 2, PRIORITYLIST, 1U); // Initial Count 0, Max Count 2.

	pK->Start("Idle Task", 128, TestSemaphore_Test4_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 4 - Testing Two Tasks waiting to take the semaphore, USE OF SIGNAL_ALL WHEN SIGNALLING THE SEMAPHORE.
// SAME AS TEST 3 BUT USING Object_WaitMultiple
// Task 3 should never get to take the semaphore.
// Task 2 is set waiting on the semaphore, task two is first in the wait list.
// Task 3 is set waiting on the semaphore, task three is second in the wait list.
// Semaphore is signaled by task 1.
// Task 2 takes the semaphore, runs and waits on the semaphore, it is first in the wait list.
// Task 3 runs, finds the semaphore count is 0, and is set waiting again, this happens internal to the OS, Task 3 is second in the waitlist.
// Process repeats.
static void TestSemaphore_Test5_IdleTask(void * D);
static void TestSemaphore_Test5_TaskOneFunc(void * D);
static void TestSemaphore_Test5_TaskTwoFunc(void * D);
static void TestSemaphore_Test5_TaskThreeFunc(void * D);
static void TestSemaphore_Test5_TaskFourFunc(void * D);

static void TestSemaphore_Test5_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){
#if (DBOS_PREEMPTIONENABLED == 0U)
		pK->Yield();
#endif
	}

}

static void TestSemaphore_Test5_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('D');

	pK->Sleep(40);

	DBOS_Test_LogEvent('H');

	// Testing tasks are waiting in the order they were set waiting on the Semaphore.
	DBOS_TEST_PNT(SemaphoreOne->GetTaskAtWaitListPosition(0U), TaskTwo);
	DBOS_TEST_PNT(SemaphoreOne->GetTaskAtWaitListPosition(1U), TaskThree);
	DBOS_TEST_PNT(SemaphoreOne->GetTaskAtWaitListPosition(2U), TaskFour);

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 2, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('J');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 3, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('L');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 4, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('N');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFGHIJKLMN");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSemaphore_Test5_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	pK->Sleep(10);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 2);

	DBOS_Test_LogEvent('I');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSemaphore_Test5_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('B');

	pK->Sleep(20);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 3);

	DBOS_Test_LogEvent('K');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSemaphore_Test5_TaskFourFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	pK->Sleep(30);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 4);

	DBOS_Test_LogEvent('M');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestSemaphore_Test5(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSemaphore_Test5_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 11, 196, TestSemaphore_Test5_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 12, 196, TestSemaphore_Test5_TaskThreeFunc, NULLPTR);
	TaskFour = pK->CreateTask("Task Four", 13, 196, TestSemaphore_Test5_TaskFourFunc, NULLPTR);

	SemaphoreOne = pK->CreateSemaphore("Semaphore One", 0, 2, FIRSTTOWAITFIRSTTORUNLIST, 1U); // Initial Count 0, Max Count 2.

	pK->Start("Idle Task", 128, TestSemaphore_Test5_IdleTask);

	while(1){

	}

	return;
}

} // namespace
