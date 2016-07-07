/**
 * @file dBOS_HSMTestApp.cpp
 * @author David Broadhurst
 * @date 21/02/2014
 *
 * \brief Unit testing of dBOS HSM.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_HSMTestApp.hpp"
#include "dBOS_KernelInterface.hpp"
#include "TestSys.hpp"
#include "dBOS_DebugPrint.hpp"
#include "dBOS_HSMTestBase.hpp"
#include "dBOS_HSMTestDerived.hpp"
#include "dBOS_HSMTestEvents.hpp"

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

static dBOS_QUEUEINTERFACE_P_t QueueOne;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
// Tests Entry to initial state, transition from child to parent and back to child.
static void TestHSM_Test1_IdleTask(void * D);
static void TestHSM_Test1_TaskOneFunc(void * D);
static void TestHSM_Test1_TaskTwoFunc(void * D);

static void TestHSM_Test1_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestHSM_Test1_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	class dBOS_HSMTESTBASE * pHSM = new dBOS_HSMTESTDERIVED("Test HSM", HSM_DEBUG_ALL_EVENTS);

	pHSM->Init();

	dBOS_HSMEVENT Event;

	while(1){
		DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&Event, TIMEWAITFOREVER), DBOS_Q_READ_WAITED);
		pHSM->ProcessEvent(&Event);
	}

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	while(1){

	}
}

static void TestHSM_Test1_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	dBOS_HSMEVENT Event;

	Event.m_pProp = &EVENT8; // Should cause transition to parent.
	Event.m_Data.Uint32 = 4U; // Event Data is check in state machine.

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	Event.m_pProp = &EVENT8; // Should cause transition to child.
	Event.m_Data.Uint32 = 5U; // Event Data is check in state machine.

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	Event.m_pProp = &EVENT9; // Should cause transition to same state.
	Event.m_Data.Uint32 = 6U; // Event Data is check in state machine.

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("1E3C3E5C5E6C6E6X6C6E");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}
}

void TestHSM_Test1(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestHSM_Test1_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestHSM_Test1_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, sizeof(dBOS_HSMEVENT));

	pK->Start("Idle Task", 128, TestHSM_Test1_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Tests Entry to initial state, transistion from state 1221 to 131 with correct exit, entry check and entry calls.
static void TestHSM_Test2_IdleTask(void * D);
static void TestHSM_Test2_TaskOneFunc(void * D);
static void TestHSM_Test2_TaskTwoFunc(void * D);

static void TestHSM_Test2_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestHSM_Test2_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	class dBOS_HSMTESTBASE * pHSM = new dBOS_HSMTESTDERIVED("Test HSM", HSM_DEBUG_ALL_EVENTS);

	pHSM->Init();

	dBOS_HSMEVENT Event;

	while(1){
		DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&Event, TIMEWAITFOREVER), DBOS_Q_READ_WAITED);
		pHSM->ProcessEvent(&Event);
	}

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	while(1){

	}
}

static void TestHSM_Test2_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	dBOS_HSMEVENT Event;

	Event.m_pProp = &EVENT10;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("1E3C3E5C5E6C6E6X5X3X7C7E8C8E");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}
}

void TestHSM_Test2(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestHSM_Test2_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestHSM_Test2_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, sizeof(dBOS_HSMEVENT));

	pK->Start("Idle Task", 128, TestHSM_Test2_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Tests Entry to initial state, checks events are handled by the expected state in the hierarchy.
static void TestHSM_Test3_IdleTask(void * D);
static void TestHSM_Test3_TaskOneFunc(void * D);
static void TestHSM_Test3_TaskTwoFunc(void * D);

static void TestHSM_Test3_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestHSM_Test3_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	class dBOS_HSMTESTBASE * pHSM = new dBOS_HSMTESTDERIVED("Test HSM", HSM_DEBUG_ALL_EVENTS);

	pHSM->Init();

	dBOS_HSMEVENT Event;

	while(1){
		DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&Event, TIMEWAITFOREVER), DBOS_Q_READ_WAITED);
		pHSM->ProcessEvent(&Event);
	}

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	while(1){

	}
}

static void TestHSM_Test3_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	dBOS_HSMEVENT Event;

	Event.m_pProp = &EVENT1; // Handled by state 1221

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	Event.m_pProp = &EVENT3; // Not handled by any state

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	Event.m_pProp = &EVENT4; // Handled by state 1

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("1E3C3E5C5E6C6E6H11H4");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}
}

void TestHSM_Test3(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestHSM_Test3_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestHSM_Test3_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, sizeof(dBOS_HSMEVENT));

	pK->Start("Idle Task", 128, TestHSM_Test3_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Tests Entry to initial state, checks events are passed to parent states.
static void TestHSM_Test4_IdleTask(void * D);
static void TestHSM_Test4_TaskOneFunc(void * D);
static void TestHSM_Test4_TaskTwoFunc(void * D);

static void TestHSM_Test4_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestHSM_Test4_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	class dBOS_HSMTESTBASE * pHSM = new dBOS_HSMTESTDERIVED("Test HSM", HSM_DEBUG_ALL_EVENTS);

	pHSM->Init();

	dBOS_HSMEVENT Event;

	while(1){
		DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&Event, TIMEWAITFOREVER), DBOS_Q_READ_WAITED);
		pHSM->ProcessEvent(&Event);
	}

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	while(1){

	}
}

static void TestHSM_Test4_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	dBOS_HSMEVENT Event;

	Event.m_pProp = &EVENT2; // Causes transition to state 121

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	Event.m_pProp = &EVENT5;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("1E3C3E5C5E6C6E6X5X4C4E4H53H51H5");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}
}

void TestHSM_Test4(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestHSM_Test4_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestHSM_Test4_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, sizeof(dBOS_HSMEVENT));

	pK->Start("Idle Task", 128, TestHSM_Test4_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Tests Entry to initial state, tests entry check failure is handled correctly.
static void TestHSM_Test5_IdleTask(void * D);
static void TestHSM_Test5_TaskOneFunc(void * D);
static void TestHSM_Test5_TaskTwoFunc(void * D);

static void TestHSM_Test5_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestHSM_Test5_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	class dBOS_HSMTESTBASE * pHSM = new dBOS_HSMTESTDERIVED("Test HSM", HSM_DEBUG_ALL_EVENTS);

	pHSM->Init();

	dBOS_HSMEVENT Event;

	while(1){
		DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&Event, TIMEWAITFOREVER), DBOS_Q_READ_WAITED);
		pHSM->ProcessEvent(&Event);
	}

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	while(1){

	}
}

static void TestHSM_Test5_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	dBOS_HSMEVENT Event;

	Event.m_pProp = &EVENT7;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("1E3C3E5C5E6C6E6X5X3X7C7E9C7X2C3C3E");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}
}

void TestHSM_Test5(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestHSM_Test5_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestHSM_Test5_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, sizeof(dBOS_HSMEVENT));

	pK->Start("Idle Task", 128, TestHSM_Test5_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// Tests Entry to initial state, tests entry check failure is handled correctly.
static void TestHSM_Test6_IdleTask(void * D);
static void TestHSM_Test6_TaskOneFunc(void * D);
static void TestHSM_Test6_TaskTwoFunc(void * D);

static void TestHSM_Test6_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestHSM_Test6_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	class dBOS_HSMTESTBASE * pHSM = new dBOS_HSMTESTDERIVED("Test HSM", HSM_DEBUG_ALL_EVENTS);

	pHSM->Init();

	dBOS_HSMEVENT Event;

	while(1){
		DBOS_TEST_RESULT(pK->Queue_ReadFromFront(QueueOne, (void * )&Event, TIMEWAITFOREVER), DBOS_Q_READ_WAITED);
		pHSM->ProcessEvent(&Event);
	}

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	while(1){

	}
}

static void TestHSM_Test6_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Start: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	dBOS_HSMEVENT Event;

	Event.m_pProp = &EVENT11;

	DBOS_TEST_RESULT(pK->Queue_WriteToBack(QueueOne, (void const * )&Event, TIMEWAITFOREVER), DBOS_Q_WRITTEN);

	pK->Yield();

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_CheckEventLog("1E3C3E5C5E6C6E6X5XBCBE");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}
}

void TestHSM_Test6(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestHSM_Test6_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestHSM_Test6_TaskTwoFunc, NULLPTR);

	QueueOne = pK->CreateQueue("Queue One", 4U, sizeof(dBOS_HSMEVENT));

	pK->Start("Idle Task", 128, TestHSM_Test6_IdleTask);

	while(1){

	}

	return;
}

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
