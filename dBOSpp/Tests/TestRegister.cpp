/**
 * @file TestRegister.cpp
 * @author David Broadhurst
 * @date 13/07/2015
 *
 * \brief Implementation of unit tests for Registers.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "TestRegister.hpp"
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

static dBOS_REGISTERINTERFACE_P_t RegisterOne;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// Test 1 - Testing Basic Register Functionality
static void TestRegister_Test1_IdleTask(void * D);
static void TestRegister_Test1_TaskOneFunc(void * D);
static void TestRegister_Test1_TaskTwoFunc(void * D);

static void TestRegister_Test1_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TestRegister_Test1_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	RegisterOne->SetRegisterANDMask(0x0FU);
	RegisterOne->SetRegisterORMask(0xF0U);

	pK->Yield();

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Signal(RegisterOne, 0x01, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET);

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_Signal(RegisterOne, 0x02, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET);

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Signal(RegisterOne, 0x04, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Signal(RegisterOne, 0x08, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('I');

	DBOS_TEST_RESULT(pK->Object_Signal(RegisterOne, 0x20, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('L');

	DBOS_Test_CheckEventLog("ABCDEFGHIJKL");

	DBOS_Test_PrintResults();

	pK->Sleep(5000);

	while(1){

	}

}

static void TestRegister_Test1_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(RegisterOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x0F);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_INT32U(RegisterOne->GetRegisterState(), 0x00); // Test Register Reset

	DBOS_Test_LogEvent('H');

	DBOS_TEST_RESULT(pK->Object_Wait(RegisterOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 0x20);

	DBOS_Test_LogEvent('J');

	DBOS_TEST_INT32U(RegisterOne->GetRegisterState(), 0x00); // Test Register Reset

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('K');

	pK->Sleep(5000);

	while(1){

	}

}

void TestRegister_Test1(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()

	TaskOne = pK->CreateTask("Task One", 10, 196, TestRegister_Test1_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TestRegister_Test1_TaskTwoFunc, NULLPTR);

	RegisterOne = pK->CreateRegister("Register One", PRIORITYLIST, 1U);

	pK->Start("Idle Task", 128, TestRegister_Test1_IdleTask);

	while(1){

	}

	return;
}

} // namespace
