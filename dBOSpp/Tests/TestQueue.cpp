/**
 * @file TestQueue.cpp
 * @author David Broadhurst
 * @date 1/03/2013
 *
 * \brief Implementation of unit tests for Queues.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "TestQueue.hpp"
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

static dBOS_QUEUEINTERFACE_P_t QueueOne;
static dBOS_QUEUEINTERFACE_P_t QueueTwo;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// Test 1 - Tests write to back and read from front.
static void TestQueue_Test1_IdleTask(void * D);
static void TestQueue_Test1_TaskOneFunc(void * D);
static void TestQueue_Test1_TaskTwoFunc(void * D);

static void TestQueue_Test1_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestQueue_Test1_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	INT32U QData = 1U;

	QueueOne->PrintDetails();

	DBOS_TEST_INT32U(QueueOne->GetLevel(), 0U);

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);
	DBOS_TEST_INT32U(QueueOne->GetLevel(), 1U);

	DBOS_Test_LogEvent('B');

	QData = 10U;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);
	DBOS_TEST_INT32U(QueueOne->GetLevel(), 2U);

	DBOS_Test_LogEvent('C');

	QData = 100U;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);
	DBOS_TEST_INT32U(QueueOne->GetLevel(), 3U);

	DBOS_Test_LogEvent('D');

	QData = 1000U;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);
	DBOS_TEST_INT32U(QueueOne->GetLevel(), 4U);

	DBOS_Test_LogEvent('E');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestQueue_Test1_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	DBOS_Test_LogEvent('F');

	DBOS_TEST_INT32U(QueueOne->GetLevel(), 4U);

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 1U);
	DBOS_TEST_INT32U(QueueOne->GetLevel(), 3U);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 10U);
	DBOS_TEST_INT32U(QueueOne->GetLevel(), 2U);

	DBOS_Test_LogEvent('H');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 100U);
	DBOS_TEST_INT32U(QueueOne->GetLevel(), 1U);

	DBOS_Test_LogEvent('I');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 1000U);
	DBOS_TEST_INT32U(QueueOne->GetLevel(), 0U);

	DBOS_Test_LogEvent('J');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFGHIJ");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestQueue_Test1(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestQueue_Test1_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestQueue_Test1_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, 4U);

	pK->Start("Idle Task", 128, TestQueue_Test1_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 2 - Tests write to front and read from front.
static void TestQueue_Test2_IdleTask(void * D);
static void TestQueue_Test2_TaskOneFunc(void * D);
static void TestQueue_Test2_TaskTwoFunc(void * D);

static void TestQueue_Test2_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestQueue_Test2_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	INT32U QData = 1U;

	DBOS_TEST_RESULT(pK->Queue_WriteToFront(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	DBOS_Test_LogEvent('B');

	QData = 10U;

	DBOS_TEST_RESULT(pK->Queue_WriteToFront(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	DBOS_Test_LogEvent('C');

	QData = 100U;

	DBOS_TEST_RESULT(pK->Queue_WriteToFront(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	DBOS_Test_LogEvent('D');

	QData = 1000U;

	DBOS_TEST_RESULT(pK->Queue_WriteToFront(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	DBOS_Test_LogEvent('E');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestQueue_Test2_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 1000U);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 100U);

	DBOS_Test_LogEvent('H');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 10U);

	DBOS_Test_LogEvent('I');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 1U);

	DBOS_Test_LogEvent('J');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFGHIJ");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestQueue_Test2(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestQueue_Test2_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestQueue_Test2_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, 4U);

	pK->Start("Idle Task", 128, TestQueue_Test2_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 3 - Tests write to back and read from front and read from back.
static void TestQueue_Test3_IdleTask(void * D);
static void TestQueue_Test3_TaskOneFunc(void * D);
static void TestQueue_Test3_TaskTwoFunc(void * D);

static void TestQueue_Test3_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestQueue_Test3_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	INT32U QData = 1U;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	DBOS_Test_LogEvent('B');

	QData = 10U;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	DBOS_Test_LogEvent('C');

	QData = 100U;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	DBOS_Test_LogEvent('D');

	QData = 1000U;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	DBOS_Test_LogEvent('E');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestQueue_Test3_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 1U);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_RESULT(pK->Queue_ReadFromBack(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 1000U);

	DBOS_Test_LogEvent('H');

	DBOS_TEST_RESULT(pK->Queue_ReadFromBack(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 100U);

	DBOS_Test_LogEvent('I');

	DBOS_TEST_RESULT(pK->Queue_ReadFromBack(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 10U);

	DBOS_Test_LogEvent('J');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFGHIJ");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestQueue_Test3(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestQueue_Test3_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestQueue_Test3_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, 4U);

	pK->Start("Idle Task", 128, TestQueue_Test3_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 4 - Tests full and empty messages.
static void TestQueue_Test4_IdleTask(void * D);
static void TestQueue_Test4_TaskOneFunc(void * D);

static void TestQueue_Test4_IdleTask(void * D){

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

static void TestQueue_Test4_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	DBOS_TEST_RESULT(pK->Queue_ReadFromBack(QueueOne, (void * )&QData, 0), DBOS_Q_READ_FAIL_EMPTY);

	DBOS_TEST_RESULT(pK->Queue_ReadFromBack(QueueOne, (void * )&QData, 10), DBOS_Q_READ_FAIL_EMPTY_TIMEOUT);

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);
	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);
	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);
	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, 10), DBOS_Q_WRITE_FAIL_FULL_TIMEOUT);

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, 0), DBOS_Q_WRITE_FAIL_FULL);

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestQueue_Test4(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestQueue_Test4_TaskOneFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, 4U);

	pK->Start("Idle Task", 128, TestQueue_Test4_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 5 - Tests reading from two different queues.
static void TestQueue_Test5_IdleTask(void * D);
static void TestQueue_Test5_TaskOneFunc(void * D);
static void TestQueue_Test5_TaskTwoFunc(void * D);

static void TestQueue_Test5_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestQueue_Test5_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	SIGNALLED_DATA_t SignalData;

	dBOS_SYNCINTERFACE_P_t pObjects[] = { QueueOne->GetFullCountSemaphore(), QueueTwo->GetFullCountSemaphore() };
	SNATIVECPU_t Index;

	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 2, TIMEWAITFOREVER, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_INT32S(Index, 0);

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 1U);

	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 2, TIMEWAITFOREVER, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_INT32S(Index, 1);

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueTwo, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 10U);

	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 2, TIMEWAITFOREVER, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_INT32S(Index, 0);

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 100U);

	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 2, TIMEWAITFOREVER, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_INT32S(Index, 1);

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueTwo, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 1000U);

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestQueue_Test5_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	QData = 1U;
	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	QData = 10U;
	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueTwo, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	QData = 100U;
	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	QData = 1000U;
	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueTwo, (void const * )&QData, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestQueue_Test5(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestQueue_Test5_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestQueue_Test5_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, 4U);
	QueueTwo = pK->CreateQueue("Queue One", 4U, 4U);

	pK->Start("Idle Task", 128, TestQueue_Test5_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 6 - Tests writing to two different queues.
static void TestQueue_Test6_IdleTask(void * D);
static void TestQueue_Test6_TaskOneFunc(void * D);
static void TestQueue_Test6_TaskTwoFunc(void * D);
static void TestQueue_Test6_TaskThreeFunc(void * D);

static void TestQueue_Test6_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestQueue_Test6_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	dBOS_QUEUEINTERFACE_P_t Queues[] = { QueueOne, QueueTwo };

	DBOS_Test_LogEvent('A');

	QData = 1U;
	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, TIMEWAITFOREVER), DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN);

	DBOS_Test_LogEvent('B');

	pK->Yield(); // Yield, other two tasks haven't started yet so not waiting on the queues.

	DBOS_Test_LogEvent('G');

	QData = 10U;
	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, TIMEWAITFOREVER), DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN);

	pK->Yield();

	DBOS_Test_LogEvent('J');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEFGHIJ");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestQueue_Test6_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 1U);

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ_WAITED);
	DBOS_TEST_INT32U(QData, 10U);

	DBOS_Test_LogEvent('H');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestQueue_Test6_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueTwo, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ);
	DBOS_TEST_INT32U(QData, 1U);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueTwo, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ_WAITED);
	DBOS_TEST_INT32U(QData, 10U);

	DBOS_Test_LogEvent('I');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

void TestQueue_Test6(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestQueue_Test6_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestQueue_Test6_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 10, 196, TestQueue_Test6_TaskThreeFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, 4U);
	QueueTwo = pK->CreateQueue("Queue Two", 4U, 4U);

	pK->Start("Idle Task", 128, TestQueue_Test6_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 7 - Test Queue_WriteMultipleToBack return value.
static void TestQueue_Test7_IdleTask(void * D);
static void TestQueue_Test7_TaskOneFunc(void * D);

static void TestQueue_Test7_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestQueue_Test7_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	dBOS_QUEUEINTERFACE_P_t Queues[] = { QueueOne, QueueTwo };

	QData = 1U;

	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, 0), DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN);

	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, 0), DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN);

	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, 0), DBOS_Q_WRITE_MULTIPLE_SOME_WRITTEN);

	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, 0), DBOS_Q_WRITE_MULTIPLE_SOME_WRITTEN);

	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, 0), DBOS_Q_WRITE_MULTIPLE_NONE_WRITTEN);

	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, 0), DBOS_Q_WRITE_MULTIPLE_NONE_WRITTEN);

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestQueue_Test7(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestQueue_Test7_TaskOneFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 2U, 4U);
	QueueTwo = pK->CreateQueue("Queue Two", 4U, 4U);

	pK->Start("Idle Task", 128, TestQueue_Test7_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Test 8 - Testing Task Switch When Writing
static void TestQueue_Test8_IdleTask(void * D);
static void TestQueue_Test8_TaskOneFunc(void * D);

static void TestQueue_Test8_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestQueue_Test8_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	INT32U QData;

	dBOS_QUEUEINTERFACE_P_t Queues[] = { QueueOne, QueueTwo };

	QData = 1U;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, TIMEWAITFOREVER), DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN);

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, TIMEWAITFOREVER), DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN);

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Queue_WriteMultipleToBack(Queues, 2U, &QData, TIMEWAITFOREVER), DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN);

	DBOS_Test_LogEvent('E');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(5000);

	while(1){

	}

}

static void TestQueue_Test8_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	INT32U QData;

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&QData, TIMEWAITFOREVER), DBOS_Q_READ); // Task One should be waiting to write to QueueOne
	DBOS_TEST_INT32U(QData, 1U);

	DBOS_Test_LogEvent('F');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("ABCDEF");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

void TestQueue_Test8(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 11, 196, TestQueue_Test8_TaskOneFunc, NULLPTR);
	TaskOne = pK->CreateTask("Task Two", 10, 196, TestQueue_Test8_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 2U, 4U);
	QueueTwo = pK->CreateQueue("Queue Two", 4U, 4U);

	pK->Start("Idle Task", 128, TestQueue_Test8_IdleTask);

	while(1){

	}

	return;
}
// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
