/**
 * @file TestMutex.cpp
 * @author David Broadhurst
 * @date 28/06/2015
 *
 * \brief Implementation of unit tests for mutexes.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "TestMutex.hpp"
#include "dBOS_KernelInterface.hpp"
#include "TestSys.hpp"
#include "dBOS_DebugPrint.hpp"
#include "dBOS_Config.hpp"

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
static dBOS_TASKINTERFACE_P_t TaskTwoA;
static dBOS_TASKINTERFACE_P_t TaskTwoB;
static dBOS_TASKINTERFACE_P_t TaskThree;
static dBOS_TASKINTERFACE_P_t TaskFour;
static dBOS_TASKINTERFACE_P_t TaskFive;

static dBOS_MUTEXINTERFACE_P_t MutexOne;
static dBOS_MUTEXINTERFACE_P_t MutexTwo;
static dBOS_MUTEXINTERFACE_P_t MutexThree;
static dBOS_MUTEXINTERFACE_P_t MutexFour;
static dBOS_MUTEXINTERFACE_P_t MutexFive;

static dBOS_SIGNALINTERFACE_P_t SignalOne;
static dBOS_SIGNALINTERFACE_P_t SignalTwo;

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS

// ------------------------------------------------------------------------------------------------
// Mutex & Priority inheritance testing.
// Priority inheritance test through multiple owner mutex pairs.
// Task One (Priority 10) Acquires Mutex One
// Task Two (Priority 11) Acquires Mutex Two
// Task Two Set Waiting to Acquire Mutex One
// Task Three (Priority 12) Acquires Mutex Three
// Task Three Set Waiting to Acquire Mutex Two
// Task Four (Priority 13) Set Waiting to Acquire Mutex Three
//	Task One, Two and Three all have inherited priority of Task Four.
// Task Five (Priority 14) Set Waiting to Acquire Mutex One
//	Task One now has inherited priority 14
// Task One Releases Mutex One
//	Priority of Task One drops back to normal.
// Task Five Acquires Mutex One
// Task Five Releases Mutex One
// Task Five runs to completion
// Task Two (now in first position of Mutex One WaitList) runs and acquires Mutex One
// Task Two releases Mutex One.
// Task Two releases Mutex Two.
//	Priority of Task Two drops back to normal.
// Task Three Acquires Mutex Two.
// Task Three Releases Mutex Two
// Task Three Releases Mutex Three.
//	Priority of Task Two drops back to normal.
// Task Four runs and acquires Mutex Three
// Task Four runs to completion.
// Task Three runs to completion.
// Task Two runs to completion.
// Task One runs to completion.
void PriorityInheritanceTest1_IdleTask(void * D);
void PriorityInheritanceTest1_TaskOneFunc(void * D);
void PriorityInheritanceTest1_TaskTwoFunc(void * D);
void PriorityInheritanceTest1_TaskThreeFunc(void * D);
void PriorityInheritanceTest1_TaskFourFunc(void * D);
void PriorityInheritanceTest1_TaskFiveFunc(void * D);

void PriorityInheritanceTest1_IdleTask(void * D){

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

void PriorityInheritanceTest1_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('E');

	pK->Sleep(100U);

	DBOS_Test_LogEvent('F');

	// TaskOne acquires Mutex One before any of the others (they are still sleeping)
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10);

	DBOS_Test_LogEvent('G');

	pK->Sleep(200U);
	// During sleep time,
	// Task Two (priority 11) acquired mutex 2 and is waiting on mutex 1.
	// Task Three (priority 12) acquired mutex 3 and is waiting on mutex 2.
	// Task Four (priority 13) is waiting on mutex 3.
	// So they all have inherited priority of task 4.

	DBOS_Test_LogEvent('N');

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 13);
	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 13);
	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 13);
	DBOS_TEST_PRIORITY(TaskFour->GetCurrentPriority(), 13);

	pK->Sleep(200U);
	// Task 5 set waiting on Mutex One during this time.

	DBOS_Test_LogEvent('P');

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 14);
	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 13);
	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 13);
	DBOS_TEST_PRIORITY(TaskFour->GetCurrentPriority(), 13);

	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 5, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);
	// Task One signaling the mutex drops the priority of Task One back to normal.
	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10);

	DBOS_Test_LogEvent('Z');

	DBOS_Test_CheckEventLog("ABCDEFGHIJKLMNOP#QRST#UV#WXYZ"); // # events are from OS see method dBOS_TASK::LowerTaskPriority

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(20000U);

	while(1){

	}
}

void PriorityInheritanceTest1_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('D');

	pK->Sleep(125U);

	DBOS_Test_LogEvent('H');

	// Acquire Mutex Two
	DBOS_TEST_RESULT(pK->Object_Wait(MutexTwo, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('I');

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10);
	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 11);

	DBOS_Test_LogEvent('J');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 6);

	DBOS_Test_LogEvent('S');

	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 13);

	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 7, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 13);

	DBOS_Test_LogEvent('T');

	DBOS_TEST_RESULT(pK->Object_Signal(MutexTwo, 8, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);
	// Task Two Signaling Mutex Two drops the priority of Task Two back to normal.

	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 11); // Dropped back to normal priority.

	DBOS_Test_LogEvent('Y');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000U);

	while(1){

	}
}

void PriorityInheritanceTest1_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	pK->Sleep(150U);

	DBOS_Test_LogEvent('K');

	// Acquire Mutex Three
	DBOS_TEST_RESULT(pK->Object_Wait(MutexThree, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 11); // Task One priority raised.
	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 11);
	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 12);

	DBOS_Test_LogEvent('L');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexTwo, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 8);

	DBOS_Test_LogEvent('U');

	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 11); // Dropped back to normal priority.
	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 13);

	DBOS_TEST_RESULT(pK->Object_Signal(MutexTwo, 9, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 13);

	DBOS_Test_LogEvent('V');

	DBOS_TEST_RESULT(pK->Object_Signal(MutexThree, 10, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);
	// Task Three signaling Mutex Three drops the priority of Task Three back to normal.

	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 12); // Dropped back to normal priority.

	DBOS_Test_LogEvent('X');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000U);

	while(1){

	}
}

void PriorityInheritanceTest1_TaskFourFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	pK->Sleep(175U);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 12);
	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 12);
	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 12);
	DBOS_TEST_PRIORITY(TaskFour->GetCurrentPriority(), 13);

	DBOS_Test_LogEvent('M');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexThree, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 10);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10);
	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 11);
	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 12);
	DBOS_TEST_PRIORITY(TaskFour->GetCurrentPriority(), 13);

	DBOS_Test_LogEvent('W');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000U);

	while(1){

	}
}

void PriorityInheritanceTest1_TaskFiveFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	pK->Sleep(300U);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 13); // Task One priority raised again.
	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 13); // Task Two priority raised again.
	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 13); // Task Three priority raised.
	DBOS_TEST_PRIORITY(TaskFour->GetCurrentPriority(), 13);
	DBOS_TEST_PRIORITY(TaskFive->GetCurrentPriority(), 14);

	pK->Sleep(100U);

	DBOS_Test_LogEvent('O');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 5);

	DBOS_Test_LogEvent('Q');

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10);
	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 13);
	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 13);
	DBOS_TEST_PRIORITY(TaskFour->GetCurrentPriority(), 13);

	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 6, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10);
	DBOS_TEST_PRIORITY(TaskTwo->GetCurrentPriority(), 13);
	DBOS_TEST_PRIORITY(TaskThree->GetCurrentPriority(), 13);
	DBOS_TEST_PRIORITY(TaskFour->GetCurrentPriority(), 13);

	DBOS_Test_LogEvent('R');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000U);

	while(1){

	}
}

void PriorityInheritanceTest1(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()
	;

	TaskOne = pK->CreateTask("Task One", 10, 196, PriorityInheritanceTest1_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 11, 196, PriorityInheritanceTest1_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 12, 196, PriorityInheritanceTest1_TaskThreeFunc, NULLPTR);
	TaskFour = pK->CreateTask("Task Four", 13, 196, PriorityInheritanceTest1_TaskFourFunc, NULLPTR);
	TaskFive = pK->CreateTask("Task Five", 14, 196, PriorityInheritanceTest1_TaskFiveFunc, NULLPTR);

	MutexOne = pK->CreateNonRecursiveMutex("Mutex One", PRIORITYLIST);
	MutexTwo = pK->CreateNonRecursiveMutex("Mutex Two", PRIORITYLIST);
	MutexThree = pK->CreateNonRecursiveMutex("Mutex Three", PRIORITYLIST);

	pK->Start("Idle Task", 128, PriorityInheritanceTest1_IdleTask);

	while(1){

	}

	return;
}

// ------------------------------------------------------------------------------------------------
// Mutex & Priority inheritance testing.
// Should exercise test point 2 in dBOS_KERNEL::SignalObjectProcess
// Task 3 Set waiting on SignalOne
// Task 1 Acquires MutexOne
// Task 2 Set waiting on MutexOne and SignalOne
// Task 1 Inherits priority of Task 2
// Task 1 Signals SignalOne -> Task 2 no longer waiting on MutexOne, Task 1 Priority drops down.
void PriorityInheritanceTest2_IdleTask(void * D);
void PriorityInheritanceTest2_TaskOneFunc(void * D);
void PriorityInheritanceTest2_TaskTwoFunc(void * D);
void PriorityInheritanceTest2_TaskThreeFunc(void * D);

void PriorityInheritanceTest2_IdleTask(void * D){

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

void PriorityInheritanceTest2_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	SIGNALLED_DATA_t SignalData;

	pK->Sleep(100);

	DBOS_Test_LogEvent('D');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('E');

	pK->Sleep(100);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 11);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_RESULT(pK->Object_Signal(SignalOne, 5U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10);

	DBOS_Test_LogEvent('J');

	DBOS_Test_CheckEventLog("ABCDEFG#*HIJ"); 	// # event is from OS see method dBOS_TASK::LowerTaskPriority
												// * event is from OS see method dBOS_KERNEL::SignalObjectProcess

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest2_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('B');

	SIGNALLED_DATA_t SignalData;

	pK->Sleep(150);

	dBOS_SYNCINTERFACE_P_t pObjects[] = { MutexOne, SignalOne };
	SNATIVECPU_t Index;

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 2, TIMEWAITFOREVER, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_SIGNALDATA(SignalData, 5U);

	DBOS_TEST_INT32S(Index, 1);

	DBOS_Test_LogEvent('I');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest2_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(SignalOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_SIGNALDATA(SignalData, 5U);

	DBOS_Test_LogEvent('H');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest2(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()
	;

	TaskOne = pK->CreateTask("Task One", 10, 196, PriorityInheritanceTest2_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 11, 196, PriorityInheritanceTest2_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 12, 196, PriorityInheritanceTest2_TaskThreeFunc, NULLPTR);

	MutexOne = pK->CreateNonRecursiveMutex("Mutex One", PRIORITYLIST);

	SignalOne = pK->CreateSignal("Signal One", PRIORITYLIST);

	pK->Start("Idle Task", 128, PriorityInheritanceTest2_IdleTask);

	while(1){

	}

	return;
}

// ------------------------------------------------------------------------------------------------
// Mutex & Priority inheritance testing.
// Demos Task one being moved up and down the ready task list as its inherited priority changes.
// Task 4 (Priority 13) Set Waiting on SignalTwo
// Task 3 (Priority 12) Set Waiting on SignalOne
// Task 2 (both instances) Set Waiting on SignalOne
// Task 1 Acquires MutexOne
// Task 1 Signals SignalOne.
// Task 3 Runs
// Task 3 Set waiting on MutexOne and SignalOne
// Task 1 Inherits Priority of Task 3, Task One moved to first place in the ready task list (in front of both instances of task two).
// Task 1 Signals SignalTwo
// Task 4 Runs, checks Task One is first in ready list.
// Task 4 Signals SignalOne -> Task 3 no longer waiting on MutexOne -> Priority of Task 1 drops back to normal.
// Task 4 verifies Task1 is back to lowest priority position in ready list.
void PriorityInheritanceTest3_IdleTask(void * D);
void PriorityInheritanceTest3_TaskOneFunc(void * D);
void PriorityInheritanceTest3_TaskTwoFunc(void * D);
void PriorityInheritanceTest3_TaskThreeFunc(void * D);
void PriorityInheritanceTest3_TaskFourFunc(void * D);

void PriorityInheritanceTest3_IdleTask(void * D){

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

void PriorityInheritanceTest3_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\n");
	SignalOne->PrintDetails();

	DBOS_Test_LogEvent('D');

	// Acquire the mutex.
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT)
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE)

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Signal(SignalOne, 1U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED)

	DBOS_Test_LogEvent('G');

	DBOS_TEST_RESULT(pK->Object_Signal(SignalTwo, 2U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED)

	DBOS_Test_LogEvent('L');

	DBOS_Test_CheckEventLog("ABCCDEF@GH$IJKKL");

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest3_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Wait(SignalOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 3U)

	DBOS_Test_LogEvent('K');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest3_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('B');

	DBOS_TEST_RESULT(pK->Object_Wait(SignalOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 1U)

	pK->PrintReadyTaskList();

	dBOS_SYNCINTERFACE_P_t pObjects[] = { MutexOne, SignalOne };
	SNATIVECPU_t Index;

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 2, TIMEWAITFOREVER, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_INT32U(Index, 1U)
	DBOS_TEST_SIGNALDATA(SignalData, 3U)

	DBOS_Test_LogEvent('J');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest3_TaskFourFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_TEST_PNT(pK->GetTaskAtReadyListPosition(0U), TaskThree)
	DBOS_TEST_PNT(pK->GetTaskAtReadyListPosition(3U), TaskOne)

	pK->PrintReadyTaskList();

	DBOS_Test_LogEvent('A');

	DBOS_TEST_RESULT(pK->Object_Wait(SignalTwo, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_SIGNALDATA(SignalData, 2U)

	DBOS_Test_LogEvent('H');

	DBOS_TEST_PNT(pK->GetTaskAtReadyListPosition(0U), TaskOne)
	// Task One moved up to first position.
	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 12U);

	pK->PrintReadyTaskList();

	DBOS_TEST_RESULT(pK->Object_Signal(SignalOne, 3U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH)

	DBOS_TEST_PNT(pK->GetTaskAtReadyListPosition(3U), TaskOne)
	// Task One moved back down to last position (above idle task).
	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10U);

	pK->PrintReadyTaskList();

	DBOS_Test_LogEvent('I');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest3(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()
	;

	TaskOne = pK->CreateTask("Task One", 10, 196, PriorityInheritanceTest3_TaskOneFunc, NULLPTR);
	TaskTwoA = pK->CreateTask("Task Two A", 11, 196, PriorityInheritanceTest3_TaskTwoFunc, NULLPTR);
	TaskTwoB = pK->CreateTask("Task Two B", 11, 196, PriorityInheritanceTest3_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 12, 196, PriorityInheritanceTest3_TaskThreeFunc, NULLPTR);
	TaskFour = pK->CreateTask("Task Four", 13, 196, PriorityInheritanceTest3_TaskFourFunc, NULLPTR);

	MutexOne = pK->CreateNonRecursiveMutex("Mutex One", PRIORITYLIST);

	SignalOne = pK->CreateSignal("Signal One", PRIORITYLIST);
	SignalTwo = pK->CreateSignal("Signal Two", PRIORITYLIST);

	pK->Start("Idle Task", 128, PriorityInheritanceTest3_IdleTask);

	while(1){

	}

	return;
}

// ------------------------------------------------------------------------------------------------
// Mutex & Priority inheritance testing.
// Should exercise test point 1 in dBOS_KERNEL::SignalObjectProcess

// Task 1 Acquires MutexOne
// Task 2 Set Waiting on MutexOne and SignalOne
// Task 1 Inherits priority of Task 2
// Task 1 Signals SignalOne
// Task 2 no longer waiting on SignalOne or MutexOne
// Task 1 back to normal priority
void PriorityInheritanceTest4_IdleTask(void * D);
void PriorityInheritanceTest4_TaskOneFunc(void * D);
void PriorityInheritanceTest4_TaskTwoFunc(void * D);

void PriorityInheritanceTest4_IdleTask(void * D){

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

void PriorityInheritanceTest4_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('B');

	SIGNALLED_DATA_t SignalData;

	pK->Sleep(100);

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('D');

	pK->Sleep(100);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 11);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Signal(SignalOne, 5U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10);

	DBOS_Test_LogEvent('H');

	DBOS_Test_CheckEventLog("ABCDEF#&GH"); 	// # event is from OS see method dBOS_TASK::LowerTaskPriority
											// & event is from OS see method dBOS_KERNEL::SignalObjectProcess

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest4_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	SIGNALLED_DATA_t SignalData;

	pK->Sleep(150);

	dBOS_SYNCINTERFACE_P_t pObjects[] = { MutexOne, SignalOne };
	SNATIVECPU_t Index;

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_WaitMultiple(pObjects, 2, TIMEWAITFOREVER, &Index, &SignalData), DBOS_OBJECT_WAIT_WAITED)
	DBOS_TEST_SIGNALDATA(SignalData, 5U);
	DBOS_TEST_INT32S(Index, 1);

	DBOS_Test_LogEvent('G');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest4(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()
	;

	TaskOne = pK->CreateTask("Task One", 10, 196, PriorityInheritanceTest4_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 11, 196, PriorityInheritanceTest4_TaskTwoFunc, NULLPTR);

	MutexOne = pK->CreateNonRecursiveMutex("Mutex One", PRIORITYLIST);

	SignalOne = pK->CreateSignal("Signal One", PRIORITYLIST);

	pK->Start("Idle Task", 128, PriorityInheritanceTest4_IdleTask);

	while(1){

	}

	return;
}

// ------------------------------------------------------------------------------------------------
// Mutex & Priority inheritance testing.
// Demos task being moved up and down an object's waitlist based on changing inherited priority.
//
// Task 2 B (Priority 12) Set waiting on SignalOne
// Task 2 A (Priority 11) Set waiting on SignalOne
// Task 1 (Priority 10) Acquires MutexOne
// Task 1 Set waiting on SignalOne
// SignalOne Wait List: Task Two B, Task Two A, Task One
// Task 1 in last position (position 2) in SignalOne wait list.
// Task 3 (Priority 11) Set waiting on MutexOne
// Task 1 Inherits priority of Task 3 and moves to position 1 in SignalOne waitlist
// SignalOne Wait List: Task Two B, Task One, Task Two A
// Task 4 (Priority 12) Set waiting on MutexOne
// Task 1 Inherits priority of Task 4 and moves to position 0 in SignalOne waitlist
// SignalOne Wait List: Task One, Task Two B, Task Two A
// Task 4 Wait on MutexOne times out.
// Task 1 Priority drops to 11 (inherited priority from Task 3) and moves to position 1 in SignalOne waitlist
// SignalOne Wait List: Task Two B, Task One, Task Two A
//	Task 2 A is now at the last position in SignalOne waitlist, see dBOS_TASKBITQUEUE::RequeueTaskInQueue and dBOS_TASKLINKEDQUEUE::RequeueTaskInQueue
// Task 4 Signals SignalOne
// Task 2 B Runs to completion
// Task 4 Runs to completion
// Task 1 Runs and yields.
// Task 2 A Runs to completion
// Task 1 Releases MutexOne
// Task 3 Runs to completion
// Task 1 Runs to completion

void PriorityInheritanceTest5_IdleTask(void * D);
void PriorityInheritanceTest5_TaskOneFunc(void * D);
void PriorityInheritanceTest5_TaskTwoFunc(void * D);
void PriorityInheritanceTest5_TaskThreeFunc(void * D);
void PriorityInheritanceTest5_TaskFourFunc(void * D);
void PriorityInheritanceTest5_TaskFiveFunc(void * D);

void PriorityInheritanceTest5_IdleTask(void * D){

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

void PriorityInheritanceTest5_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('E');

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('F');

	DBOS_TEST_RESULT(pK->Object_Wait(SignalOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 1U);

	DBOS_Test_LogEvent('L');

	pK->Yield();

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 11);

	DBOS_Test_LogEvent('N');

	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 3U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10);

	DBOS_Test_LogEvent('P');

	DBOS_Test_CheckEventLog("ACBCDEFGHIJMKLMN#OP");

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest5_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('C');

	DBOS_TEST_RESULT(pK->Object_Wait(SignalOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 1U);

	DBOS_Test_LogEvent('M');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest5_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('D');

	pK->Sleep(50);

	DBOS_Test_LogEvent('G');

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 10);
	DBOS_TEST_PNT(SignalOne->GetTaskAtWaitListPosition(2U), TaskOne);

	DBOS_printf("\n");
	SignalOne->PrintDetails();

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_WAITED);
	DBOS_TEST_SIGNALDATA(SignalData, 3U);

	DBOS_Test_LogEvent('O');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest5_TaskFourFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	SIGNALLED_DATA_t SignalData;

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('B');

	pK->Sleep(100);

	DBOS_Test_LogEvent('H');

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 11);
	DBOS_TEST_PNT(SignalOne->GetTaskAtWaitListPosition(1U), TaskOne);

	DBOS_printf("\n");
	SignalOne->PrintDetails();

	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, 50), DBOS_OBJECT_WAIT_WAITED_TIMEOUT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('J');

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 11);
	DBOS_TEST_PNT(SignalOne->GetTaskAtWaitListPosition(1U), TaskOne);

	DBOS_printf("\n");
	SignalOne->PrintDetails();

	DBOS_TEST_RESULT(pK->Object_Signal(SignalOne, 1U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_TASK_SWITCHED);

	DBOS_Test_LogEvent('K');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest5_TaskFiveFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_LogEvent('A');

	pK->Sleep(125);

	DBOS_TEST_PRIORITY(TaskOne->GetCurrentPriority(), 12);
	DBOS_TEST_PNT(SignalOne->GetTaskAtWaitListPosition(0U), TaskOne);

	DBOS_printf("\n");
	SignalOne->PrintDetails();

	DBOS_Test_LogEvent('I');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void PriorityInheritanceTest5(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()
	;

	TaskOne = pK->CreateTask("Task One", 10, 196, PriorityInheritanceTest5_TaskOneFunc, NULLPTR);
	TaskTwoA = pK->CreateTask("Task Two A", 11, 196, PriorityInheritanceTest5_TaskTwoFunc, NULLPTR);
	TaskTwoB = pK->CreateTask("Task Two B", 12, 196, PriorityInheritanceTest5_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 11, 196, PriorityInheritanceTest5_TaskThreeFunc, NULLPTR);
	TaskFour = pK->CreateTask("Task Four", 12, 196, PriorityInheritanceTest5_TaskFourFunc, NULLPTR);
	TaskFive = pK->CreateTask("Task Five", 13, 196, PriorityInheritanceTest5_TaskFiveFunc, NULLPTR);

	MutexOne = pK->CreateNonRecursiveMutex("Mutex One", PRIORITYLIST);

	SignalOne = pK->CreateSignal("Signal One", PRIORITYLIST);

	pK->Start("Idle Task", 196, PriorityInheritanceTest5_IdleTask);

	while(1){

	}

	return;
}
// ------------------------------------------------------------------------------------------------
// Mutex & Priority inheritance testing.
// Testing return values using non recursive mutex.
void MutexTest1_IdleTask(void * D);
void MutexTest1_TaskOneFunc(void * D);
void MutexTest1_TaskTwoFunc(void * D);

void MutexTest1_IdleTask(void * D){

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

void MutexTest1_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	pK->Sleep(100);

	DBOS_Test_LogEvent('D');

	// Acquire Mutex One
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	// Release Mutex One
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 2U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	// Try releasing mutex one again.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 0U, SIGNAL_AUTO, TRUE), DBOS_MUTEX_ERROR_HAS_NO_OWNER);

	// Try releasing a mutex owned by another task.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexTwo, 0U, SIGNAL_AUTO, TRUE), DBOS_MUTEX_ERROR_THIS_TASK_NOT_THE_OWNER); // Mutex owned by another task.

	// Acquire Mutex One
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, 2U);

	// Try to Acquire Mutex One Again.
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_MUTEX_ERROR_NOT_A_RECURSIVE_MUTEX);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	// Release Mutex One.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 3U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	// Acquire Mutex One.
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, 3U);

	// Acquire Mutex Four.
	DBOS_TEST_RESULT(pK->Object_Wait(MutexFour, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	// Acquire Mutex Five.
	DBOS_TEST_RESULT(pK->Object_Wait(MutexFive, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	// Try out of order mutex release.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 0U, SIGNAL_AUTO, TRUE), DBOS_MUTEX_ERROR_IS_NOT_LAST_MUTEX_ACQUIRED);

	// Release mutex five.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexFive, 4U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	// Release mutex four.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexFour, 5U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	// Release mutex one.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 6U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	DBOS_Test_LogEvent('E');

	DBOS_Test_CheckEventLog("ABCDE");

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(20000);

	while(1){

	}
}

void MutexTest1_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	// Acquire Mutex Two
	DBOS_TEST_RESULT(pK->Object_Wait(MutexTwo, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('B');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void MutexTest1(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()
	;

	TaskOne = pK->CreateTask("Task One", 10, 196, MutexTest1_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 11, 196, MutexTest1_TaskTwoFunc, NULLPTR);

	MutexOne = pK->CreateNonRecursiveMutex("Mutex One", PRIORITYLIST);
	MutexTwo = pK->CreateNonRecursiveMutex("Mutex Two", PRIORITYLIST);
	MutexThree = pK->CreateNonRecursiveMutex("Mutex Three", PRIORITYLIST);
	MutexFour = pK->CreateNonRecursiveMutex("Mutex Four", PRIORITYLIST);
	MutexFive = pK->CreateNonRecursiveMutex("Mutex Five", PRIORITYLIST);

	pK->Start("Idle Task", 128, MutexTest1_IdleTask);

	while(1){

	}

	return;
}

// ------------------------------------------------------------------------------------------------
// Mutex & Priority inheritance testing.
// Testing return values with a recursive mutex.
void MutexTest2_IdleTask(void * D);
void MutexTest2_TaskOneFunc(void * D);
void MutexTest2_TaskTwoFunc(void * D);

void MutexTest2_IdleTask(void * D){

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

void MutexTest2_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('C');

	pK->Sleep(100);

	DBOS_Test_LogEvent('D');

	// Acquire Mutex One
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_TEST_INT32S(MutexOne->GetCount(), 0)

	// Acquire Mutex One Again
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_TEST_INT32S(MutexOne->GetCount(), -1)

	// Release Mutex One
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 2U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET);

	DBOS_TEST_INT32S(MutexOne->GetCount(), 0)

	// Release Mutex One Again
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 3U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	DBOS_TEST_INT32S(MutexOne->GetCount(), 1)

	// Acquire Mutex One
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, 3U);

	DBOS_TEST_INT32S(MutexOne->GetCount(), 0)

	DBOS_printf("\n");
	MutexOne->PrintDetails();

	// Acquire Mutex One Again
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, 3U);

	DBOS_TEST_INT32S(MutexOne->GetCount(), -1)

	// Acquire mutex 4.
	DBOS_TEST_RESULT(pK->Object_Wait(MutexFour, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	// Try to Acquire Mutex One Again
	DBOS_TEST_RESULT(pK->Object_Wait(MutexOne, &SignalData, TIMEWAITFOREVER), DBOS_MUTEX_ERROR_IS_NOT_LAST_MUTEX_ACQUIRED);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	// Acquire mutex 5.
	DBOS_TEST_RESULT(pK->Object_Wait(MutexFive, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	// Try Release mutex one.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 4U, SIGNAL_AUTO, TRUE), DBOS_MUTEX_ERROR_IS_NOT_LAST_MUTEX_ACQUIRED);

	// Release mutex five.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexFive, 5U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	// Release mutex four.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexFour, 6U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	DBOS_TEST_INT32S(MutexOne->GetCount(), -1)

	// Release mutex one.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 7U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET);

	DBOS_TEST_INT32S(MutexOne->GetCount(), 0)

	// Release mutex one.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 8U, SIGNAL_AUTO, TRUE), DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING);

	DBOS_TEST_INT32S(MutexOne->GetCount(), 1)

	// Try releasing mutex one again.
	DBOS_TEST_RESULT(pK->Object_Signal(MutexOne, 9U, SIGNAL_AUTO, TRUE), DBOS_MUTEX_ERROR_HAS_NO_OWNER);

	DBOS_Test_LogEvent('E');

	DBOS_Test_CheckEventLog("ABCDE");

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	DBOS_Test_PrintResults();

	pK->Sleep(20000);

	while(1){

	}
}

void MutexTest2_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	SIGNALLED_DATA_t SignalData;

	DBOS_Test_LogEvent('A');

	// Acquire Mutex Two
	DBOS_TEST_RESULT(pK->Object_Wait(MutexTwo, &SignalData, TIMEWAITFOREVER), DBOS_OBJECT_WAIT_NO_WAIT);
	DBOS_TEST_SIGNALDATA(SignalData, SIGNALDATANONE);

	DBOS_Test_LogEvent('B');

	DBOS_printf("\nTask End: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->Sleep(20000);

	while(1){

	}
}

void MutexTest2(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_TEST_START()
	;

	TaskOne = pK->CreateTask("Task One", 10, 196, MutexTest2_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 11, 196, MutexTest2_TaskTwoFunc, NULLPTR);

	MutexOne = pK->CreateRecursiveMutex("Mutex One", PRIORITYLIST);
	MutexTwo = pK->CreateRecursiveMutex("Mutex Two", PRIORITYLIST);
	MutexThree = pK->CreateRecursiveMutex("Mutex Three", PRIORITYLIST);
	MutexFour = pK->CreateRecursiveMutex("Mutex Four", PRIORITYLIST);
	MutexFive = pK->CreateRecursiveMutex("Mutex Five", PRIORITYLIST);

	pK->Start("Idle Task", 128, MutexTest2_IdleTask);

	while(1){

	}

	return;
}

} // namespace
