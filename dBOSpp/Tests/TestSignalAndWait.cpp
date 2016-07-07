/**
 * @file TestSignalAndWait.cpp
 * @author David Broadhurst
 * @date 13/07/2015
 *
 * \brief Implementation of unit tests for Signal And Wait.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "TestSignalAndWait.hpp"
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

static dBOS_REGISTERINTERFACE_P_t RegisterOne;
static dBOS_SEMAPHOREINTERFACE_P_t SemaphoreOne;
static dBOS_SIGNALINTERFACE_P_t SignalOne;
static dBOS_MUTEXINTERFACE_P_t MutexOne;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// Test 1 - Testing Signal And Wait Functionality.
// All tasks waiting until all required bits are set.
static void TestSignalAndWait_Test1_IdleTask(void * D);
static void TestSignalAndWait_Test1_TaskOneFunc(void * D);
static void TestSignalAndWait_Test1_TaskTwoFunc(void * D);
static void TestSignalAndWait_Test1_TaskThreeFunc(void * D);
static void TestSignalAndWait_Test1_TaskFourFunc(void * D);

static void TestSignalAndWait_Test1_IdleTask(void * D){

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

static void TestSignalAndWait_Test1_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	RegisterOne->SetRegisterANDMask(0x0FU);

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(RegisterOne, 0x01U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x0F);

	DBOS_Test_LogEvent('E');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test1_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(RegisterOne, 0x02U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x0F);

	DBOS_Test_LogEvent('F');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test1_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(RegisterOne, 0x04U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x0F);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test1_TaskFourFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(RegisterOne, 0x08U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x0F);

	DBOS_Test_LogEvent('H');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("A>B>C>DEFG{H"); // > and { are written by the OS, see method dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test1(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test1_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSignalAndWait_Test1_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 10, 196, TestSignalAndWait_Test1_TaskThreeFunc, NULLPTR);
	TaskFour = pK->CreateTask("Task Four", 10, 196, TestSignalAndWait_Test1_TaskFourFunc, NULLPTR);

	RegisterOne = pK->CreateRegister("Register One", PRIORITYLIST, 1U); // Testing with ISR Mask to make sure no assertions fail.

	pK->Start("Idle Task", 128, TestSignalAndWait_Test1_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 2 - Testing Signal And Wait Functionality.
// Testing with a semaphore.
static void TestSignalAndWait_Test2_IdleTask(void * D);
static void TestSignalAndWait_Test2_TaskOneFunc(void * D);
static void TestSignalAndWait_Test2_TaskTwoFunc(void * D);

static void TestSignalAndWait_Test2_IdleTask(void * D){

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

static void TestSignalAndWait_Test2_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x02U);

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 0x04U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('E');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test2_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(SemaphoreOne, 0x02U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x04U);

	DBOS_Test_LogEvent('D');

	pK->Yield();

	DBOS_Test_LogEvent('F');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABC]DEF"); // ] is written by the OS, see method dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test2(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test2_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSignalAndWait_Test2_TaskTwoFunc, NULLPTR);

	SemaphoreOne = pK->CreateSemaphore("Semaphore One", 0, 1, PRIORITYLIST, 1U); // Testing with ISR Mask to make sure no assertions fail.

	pK->Start("Idle Task", 128, TestSignalAndWait_Test2_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 3 - Testing Signal And Wait Functionality.
// Testing with a NonRecursive mutex.
static void TestSignalAndWait_Test3_IdleTask(void * D);
static void TestSignalAndWait_Test3_TaskOneFunc(void * D);
static void TestSignalAndWait_Test3_TaskTwoFunc(void * D);

static void TestSignalAndWait_Test3_IdleTask(void * D){

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

static void TestSignalAndWait_Test3_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('B');

	pK->Yield();

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(MutexOne, 0x02U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x05U);

	DBOS_Test_LogEvent('F');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test3_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x02U);

	DBOS_Test_LogEvent('E');

	pK->Object_Signal(MutexOne, 0x05U, SIGNAL_AUTO, TRUE);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDE]FG"); // ] is written by the OS, see method dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test3(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test3_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSignalAndWait_Test3_TaskTwoFunc, NULLPTR);

	MutexOne = pK->CreateNonRecursiveMutex("Mutex One", PRIORITYLIST);

	pK->Start("Idle Task", 128, TestSignalAndWait_Test3_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 4 - Testing Signal And Wait Functionality.
// Testing with a Recursive mutex, same test as Test 3.
static void TestSignalAndWait_Test4_IdleTask(void * D);
static void TestSignalAndWait_Test4_TaskOneFunc(void * D);
static void TestSignalAndWait_Test4_TaskTwoFunc(void * D);

static void TestSignalAndWait_Test4_IdleTask(void * D){

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

static void TestSignalAndWait_Test4_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('B');

	pK->Yield();

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(MutexOne, 0x02U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x05U);

	DBOS_Test_LogEvent('F');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test4_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x02U);

	DBOS_Test_LogEvent('E');

	pK->Object_Signal(MutexOne, 0x05U, SIGNAL_AUTO, TRUE);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDE]FG"); // ] is written by the OS, see method dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test4(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test4_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSignalAndWait_Test4_TaskTwoFunc, NULLPTR);

	MutexOne = pK->CreateRecursiveMutex("Mutex One", PRIORITYLIST);

	pK->Start("Idle Task", 128, TestSignalAndWait_Test4_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 5 - Testing Signal And Wait Functionality.
// Testing SIGNAL_ALL with a semaphore.
static void TestSignalAndWait_Test5_IdleTask(void * D);
static void TestSignalAndWait_Test5_TaskOneFunc(void * D);
static void TestSignalAndWait_Test5_TaskTwoFunc(void * D);
static void TestSignalAndWait_Test5_TaskThreeFunc(void * D);

static void TestSignalAndWait_Test5_IdleTask(void * D){

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

static void TestSignalAndWait_Test5_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x02U);

	DBOS_Test_LogEvent('D');

	pK->PrintReadyTaskList();

	pK->Yield(); // Yield here, tasks two and three are on the ready task list, they will now run and try to take the semaphore and fail as this task has taken it.

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 0x04U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test5_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x04U);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 0x05U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('H');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCD()EFGH"); // ( and ) are written by the OS, see methods dBOS_KERNEL::WaitCurrentTaskOnObject and dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test5_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(SemaphoreOne, 0x02U, SIGNAL_ALL, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x05U);

	DBOS_Test_LogEvent('F');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test5(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test5_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSignalAndWait_Test5_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 10, 196, TestSignalAndWait_Test5_TaskThreeFunc, NULLPTR);

	SemaphoreOne = pK->CreateSemaphore("Semaphore One", 0, 10, PRIORITYLIST, 1U); // Testing with ISR Mask to make sure no assertions fail.

	pK->Start("Idle Task", 128, TestSignalAndWait_Test5_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 6 - Testing Signal And Wait Functionality.
// Testing with a semaphore where signaling task has higher priority than waiting tasks.
static void TestSignalAndWait_Test6_IdleTask(void * D);
static void TestSignalAndWait_Test6_TaskOneFunc(void * D);
static void TestSignalAndWait_Test6_TaskTwoFunc(void * D);
static void TestSignalAndWait_Test6_TaskThreeFunc(void * D);

static void TestSignalAndWait_Test6_IdleTask(void * D){

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

static void TestSignalAndWait_Test6_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x08U);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test6_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x08U);

	DBOS_Test_LogEvent('H');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABC[DEFGH"); // [ is written by the OS, see method dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test6_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	pK->Sleep(10);

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(SemaphoreOne, 0x02U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH);
	DBOS_TEST_SIGNALDATA(SignalData, 0x02U);

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 0x04U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 0x08U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH);

	DBOS_Test_LogEvent('F');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test6(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test6_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSignalAndWait_Test6_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 11, 196, TestSignalAndWait_Test6_TaskThreeFunc, NULLPTR);

	SemaphoreOne = pK->CreateSemaphore("Semaphore One", 0, 10, PRIORITYLIST, 1U); // Testing with ISR Mask to make sure no assertions fail.

	pK->Start("Idle Task", 128, TestSignalAndWait_Test6_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 7 - Testing Signal And Wait Functionality.
// Testing with a signal object where signaling task has higher priority than waiting tasks.
static void TestSignalAndWait_Test7_IdleTask(void * D);
static void TestSignalAndWait_Test7_TaskOneFunc(void * D);
static void TestSignalAndWait_Test7_TaskTwoFunc(void * D);
static void TestSignalAndWait_Test7_TaskThreeFunc(void * D);

static void TestSignalAndWait_Test7_IdleTask(void * D){

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

static void TestSignalAndWait_Test7_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(SignalOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x08U);

	DBOS_Test_LogEvent('E');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test7_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(SignalOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x08U);

	DBOS_Test_LogEvent('F');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABC}DEF"); // } is written by the OS, see method dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test7_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	pK->Sleep(10);

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(SignalOne, 0x08U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH);
	DBOS_TEST_SIGNALDATA(SignalData, 0x08U);

	DBOS_Test_LogEvent('D');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test7(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test7_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSignalAndWait_Test7_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 11, 196, TestSignalAndWait_Test7_TaskThreeFunc, NULLPTR);

	SignalOne = pK->CreateSignal("Signal One", PRIORITYLIST, 1U); // Testing with ISR Mask to make sure no assertions fail.

	pK->Start("Idle Task", 128, TestSignalAndWait_Test7_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 8 - Testing Signal And Wait Functionality.
// Testing with a semaphore with zero wait time.
static void TestSignalAndWait_Test8_IdleTask(void * D);
static void TestSignalAndWait_Test8_TaskOneFunc(void * D);
static void TestSignalAndWait_Test8_TaskTwoFunc(void * D);
static void TestSignalAndWait_Test8_TaskThreeFunc(void * D);

static void TestSignalAndWait_Test8_IdleTask(void * D){

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

static void TestSignalAndWait_Test8_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x02U);

	DBOS_Test_LogEvent('D');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test8_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x03U);

	DBOS_Test_LogEvent('F');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test8_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(SemaphoreOne, 0x02U, SIGNAL_AUTO, TRUE, &SignalData, 0), DBOS_OBJECT_WAIT_TIMEOUT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(SemaphoreOne, 0x03U, SIGNAL_AUTO, TRUE, &SignalData, 0), DBOS_OBJECT_WAIT_TIMEOUT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABC]<DE]<FG"); // ] and < are written by the OS, see method dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test8(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test8_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSignalAndWait_Test8_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 10, 196, TestSignalAndWait_Test8_TaskThreeFunc, NULLPTR);

	SemaphoreOne = pK->CreateSemaphore("Semaphore One", 0, 1, PRIORITYLIST, 1U); // Testing with ISR Mask to make sure no assertions fail.

	pK->Start("Idle Task", 128, TestSignalAndWait_Test8_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 9 - Testing Signal And Wait Functionality.
// Task One Acquires Mutex.
// Task Two Set Waiting on Mutex, Task One inherits priority of Task Two.
// Task Three Set Waiting on Mutex.
// Task One signals and waits on Mutex.
// Task One priority drops to normal.
// Task Two runs and acquires the Mutex.
// Task Two The signals the Mutex.
// Task Three runs and acquires the Mutex.
// Task Three signals the Mutex.
// Task One added to ready list.
// Task Three runs to completion.
// Task Two runs to completion.
// Task One runs and acquires the Mutex.
static void TestSignalAndWait_Test9_IdleTask(void * D);
static void TestSignalAndWait_Test9_TaskOneFunc(void * D);
static void TestSignalAndWait_Test9_TaskTwoFunc(void * D);
static void TestSignalAndWait_Test9_TaskThreeFunc(void * D);

static void TestSignalAndWait_Test9_IdleTask(void * D){

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

static void TestSignalAndWait_Test9_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('D');

	pK->Sleep(75);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\n");
	MutexOne->PrintDetails();

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(MutexOne, 1U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 3U);

	DBOS_Test_LogEvent('K');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFG#HIJ]K"); // # and ] are written by the OS, see methods dBOS_TASK::LowerTaskPriority and dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test9_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	pK->Sleep(50);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 1);

	DBOS_Test_LogEvent('H');

	DBOS_printf("\n");
	MutexOne->PrintDetails();

	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 2U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('J');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test9_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	pK->Sleep(50);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 2U);

	DBOS_Test_LogEvent('I');

	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 3U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH);

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test9(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test9_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 11, 196, TestSignalAndWait_Test9_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 11, 196, TestSignalAndWait_Test9_TaskThreeFunc, NULLPTR);

	MutexOne = pK->CreateNonRecursiveMutex("Mutex One", PRIORITYLIST);

	pK->Start("Idle Task", 128, TestSignalAndWait_Test9_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 10 - Testing Signal And Wait Functionality.
// SAME AS TEST 9 BUT WITH RECURSIVE MUTEX
// Task One Acquires Mutex.
// Task Two Set Waiting on Mutex, Task One inherits priority of Task Two.
// Task Three Set Waiting on Mutex.
// Task One signals and waits on Mutex.
// Task One priority drops to normal.
// Task Two runs and acquires the Mutex.
// Task Two The signals the Mutex.
// Task Three runs and acquires the Mutex.
// Task Three signals the Mutex.
// Task One added to ready list.
// Task Three runs to completion.
// Task Two runs to completion.
// Task One runs and acquires the Mutex.
static void TestSignalAndWait_Test10_IdleTask(void * D);
static void TestSignalAndWait_Test10_TaskOneFunc(void * D);
static void TestSignalAndWait_Test10_TaskTwoFunc(void * D);
static void TestSignalAndWait_Test10_TaskThreeFunc(void * D);

static void TestSignalAndWait_Test10_IdleTask(void * D){

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

static void TestSignalAndWait_Test10_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('D');

	pK->Sleep(75);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\n");
	MutexOne->PrintDetails();

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(MutexOne, 1U, SIGNAL_AUTO, TRUE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 3U);

	DBOS_Test_LogEvent('K');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFG#HIJ]K"); // # and ] are written by the OS, see methods dBOS_TASK::LowerTaskPriority and dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test10_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	pK->Sleep(50);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 1);

	DBOS_Test_LogEvent('H');

	DBOS_printf("\n");
	MutexOne->PrintDetails();

	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 2U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('J');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test10_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	pK->Sleep(50);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 2U);

	DBOS_Test_LogEvent('I');

	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 3U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH);

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test10(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test10_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 11, 196, TestSignalAndWait_Test10_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 11, 196, TestSignalAndWait_Test10_TaskThreeFunc, NULLPTR);

	MutexOne = pK->CreateRecursiveMutex("Mutex One", PRIORITYLIST);

	pK->Start("Idle Task", 128, TestSignalAndWait_Test10_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 11 - Testing Signal And Wait Functionality.
// Testing with a semaphore, SAME AS TEST 2 BUT TASK SWITCH SET TO FALSE.
static void TestSignalAndWait_Test11_IdleTask(void * D);
static void TestSignalAndWait_Test11_TaskOneFunc(void * D);
static void TestSignalAndWait_Test11_TaskTwoFunc(void * D);

static void TestSignalAndWait_Test11_IdleTask(void * D){

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

static void TestSignalAndWait_Test11_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(SemaphoreOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x02U);

	DBOS_Test_LogEvent('D');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestSignalAndWait_Test11_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_SignalAndWait(SemaphoreOne, 0x04U, SIGNAL_AUTO, FALSE, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH); // Task does not switch here.
	DBOS_TEST_SIGNALDATA(SignalData, 0x04U);

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Signal(SemaphoreOne, 0x02U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED); // TASK SWITCHES HERE

	DBOS_Test_LogEvent('E');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("AB@CDE"); // @ is written by the OS, see method dBOS_KERNEL::SignalAndWaitOnObject

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestSignalAndWait_Test11(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestSignalAndWait_Test11_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestSignalAndWait_Test11_TaskTwoFunc, NULLPTR);

	SemaphoreOne = pK->CreateSemaphore("Semaphore One", 0, 1, PRIORITYLIST, 1U); // Testing with ISR Mask to make sure no assertions fail.

	pK->Start("Idle Task", 128, TestSignalAndWait_Test11_IdleTask);

	while(1){

	}

	return;
}

} // namespace
