/**
 * @file dBOS_Task_Queue.cpp
 * @author David Broadhurst
 * @date 13/01/2015
 *
 * \brief Implementation of task queue manipulation services.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Task_Queue.hpp"
#include "dBOS_Task.hpp"
#include "dBOS_PortFunctions.hpp"
#include "dBOS_Assert.hpp"

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

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
dBOS_TASKLINKEDQUEUE::dBOS_TASKLINKEDQUEUE(dBOS_LINKEDQUEUE<dBOS_TASK> * const m_pQueueIn) :
		dBOS_TASKQUEUE(), m_pQueue(m_pQueueIn){

		ASSERT_FROM_OSCS(m_pQueueIn != NULLPTR)
}

dBOS_TASKLINKEDQUEUE::~dBOS_TASKLINKEDQUEUE(){

}

/**
 * \brief Removes the first task from the queue and returns a pointer to that task.
 *
 * @retval Pointer to the task that was removed from the queue.
 * @retval NULLPTR Queue was empty.
 */
dBOS_LISTNODE<dBOS_TASK> * dBOS_TASKLINKEDQUEUE::DeQNextTaskFromQueue(void){

	return m_pQueue->RemoveHead();
}

/**
 * \brief Returns a pointer to the first task in the queue.
 *
 * @retval Pointer to the first task in the queue.
 * @retval NULL Queue was empty.
 */
dBOS_LISTNODE<dBOS_TASK> * dBOS_TASKLINKEDQUEUE::GetNextTaskFromQueue(void) const{

	return m_pQueue->GetHead();
}

/**
 * \brief Adds a task into the queue.
 *
 * If the queue is a priority based queue then the task added is positioned as the <B>LAST</B> link of the priority level.
 * If the queue is not a priority based queue then the task is added at the end of the queue.
 *
 * @param[in] pNode pointer to the link to add to the queue.
 */
void dBOS_TASKLINKEDQUEUE::AddTaskToQueue(dBOS_LISTNODE<dBOS_TASK> * const pNode){

	m_pQueue->AddItem_FIFO(pNode);

	return;
}

/**
 * \brief Removes a task from the queue.
 *
 * @param[in] pNode pointer to the task to remove from the queue.
 */
void dBOS_TASKLINKEDQUEUE::RemoveTaskFromQueue(dBOS_LISTNODE<dBOS_TASK> * const pNode){

	m_pQueue->RemoveItem(pNode);

	return;
}

/**
 * \brief Returns a pointer to the task at the given position.
 *
 * @param[in] Position Position in queue of task to get. First task is at position 0.
 * @retval Pointer to task.
 * @retval NULLPTR Position is beyond the end of queue.
 */
DBOS_TASK_CONST_P_t dBOS_TASKLINKEDQUEUE::GetTaskAtPosition(UNATIVECPU_t const Position) const{

	DBOS_TASK_CONST_P_t pResult = NULLPTR;

	dBOS_LISTNODE<dBOS_TASK> const * const pItem = m_pQueue->GetItemAtPosition(Position);

	if(pItem != NULLPTR){
		pResult = pItem->GetItem();
	}
	else{
		pResult = NULLPTR;
	}

	return pResult;
}

BOOL dBOS_TASKLINKEDQUEUE::TaskWouldBeFirstInQueue(dBOS_LISTNODE<dBOS_TASK> const * const pNode) const{

	return m_pQueue->ItemWouldBeFirstInQueue(pNode);
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

/**
 * \brief Prints names of the tasks in the queue starting from the head of the queue.
 *
 */
void dBOS_TASKLINKEDQUEUE::PrintQueue() const{

	dBOS_LISTNODE<dBOS_TASK> const * pCurrent;

	pCurrent = m_pQueue->GetHead();

	if(pCurrent == NULLPTR){
		DBOS_Intprintf("Queue Empty");
	}
	else{

		ASSERT_FROM_OSCS(pCurrent->GetItem() != NULLPTR)

		pCurrent->GetItem()->PrintID();

		pCurrent = m_pQueue->GetNext(pCurrent);

		while(pCurrent != NULLPTR){

			ASSERT_FROM_OSCS(pCurrent->GetItem() != NULLPTR)

			DBOS_Intprintf(", ");
			pCurrent->GetItem()->PrintID();

			pCurrent = m_pQueue->GetNext(pCurrent);
		}

	}

	return;
}

#endif

dBOS_TASKPRIORITYLINKEDQUEUE::dBOS_TASKPRIORITYLINKEDQUEUE() :
		dBOS_TASKLINKEDQUEUE(new dBOS_PRIORITYLINKEDQUEUE<dBOS_TASK>){

}

dBOS_TASKPRIORITYLINKEDQUEUE::~dBOS_TASKPRIORITYLINKEDQUEUE(){

}

/**
 * \brief Called when the priority of the task has changed, the task is repositioned in the queue based on the new task priority level.
 *
 * The task is removed from the queue and added back in at a position determined by the new task priority.\n
 * The task is positioned as the <B>FIRST</B> link of the priority level.
 *
 * @param[in] pNode Pointer to link that is to be repositioned in the queue.
 * @param[in] NewPriority Priority which determines the new position in the queue.
 * @retval DBOS_DONE Link has been repositioned.
 */
DBOS_RESULT_t dBOS_TASKPRIORITYLINKEDQUEUE::RequeueTaskInQueue(dBOS_LISTNODE<dBOS_TASK> * const pNode, DBOS_TASK_PRIORITY_t const NewPriority){

	DBOS_RESULT_t Result;

	m_pQueue->RemoveItem(pNode);

	m_pQueue->AddItem_LIFO(pNode, NewPriority);

	Result = DBOS_DONE;

	return Result;
}

dBOS_TASKQUEUE_ORDER_t dBOS_TASKPRIORITYLINKEDQUEUE::GetOrderType(void) const{

	return dBOS_TASKQUEUE_ORDER_PRIORITY;
}

dBOS_TASKFIFOLINKEDQUEUE::dBOS_TASKFIFOLINKEDQUEUE() :
		dBOS_TASKLINKEDQUEUE(new dBOS_FIFOLINKEDQUEUE<dBOS_TASK>){

}

dBOS_TASKFIFOLINKEDQUEUE::~dBOS_TASKFIFOLINKEDQUEUE(){

}

/**
 * \brief Called when the priority of the task has changed, the task is repositioned in the queue based on the new task priority level.
 *
 * The task is removed from the queue and added back in at a position determined by the new task priority.\n
 * The task is positioned as the <B>FIRST</B> link of the priority level.
 *
 * @param[in] pNode pointer to task that is to be repositioned in the queue.
 * @param[in] NewPriority priority which determines the new position in the queue.
 * @retval DBOS_DONE Link has been repositioned.
 * @retval DBOS_NOT_A_PRIORITY_BASED_WAITLIST the queue is not priority based, the link can not be repositioned.
 */
DBOS_RESULT_t dBOS_TASKFIFOLINKEDQUEUE::RequeueTaskInQueue(dBOS_LISTNODE<dBOS_TASK> * const pNode, DBOS_TASK_PRIORITY_t const NewPriority){

	(void) NewPriority; // Unused parameter.

	DBOS_RESULT_t Result;

	ASSERT_FROM_OSCS(pNode != NULLPTR)
	ASSERT_FROM_OSCS(pNode->GetItem() != NULLPTR)

	Result = DBOS_NOT_A_PRIORITY_BASED_WAITLIST;

	return Result;
}

dBOS_TASKQUEUE_ORDER_t dBOS_TASKFIFOLINKEDQUEUE::GetOrderType(void) const{

	return dBOS_TASKQUEUE_ORDER_FIFO;
}

dBOS_TASKBITQUEUE::dBOS_TASKBITQUEUE() :
		dBOS_TASKQUEUE(){

	UNATIVECPU_t index;

	for(index = 0U; index < TOTAL_NUM_TASKPRIORITY_FLAG_WORDS ; index++){
		m_Flags[index] = 0U;
	}

}

dBOS_TASKBITQUEUE::~dBOS_TASKBITQUEUE(){

}

/**
 * \brief Removes the first task from the queue and returns a pointer to that task.
 *
 * @retval Pointer to the task that was removed from the queue.
 * @retval NULLPTR Queue was empty.
 */
dBOS_LISTNODE<dBOS_TASK> * dBOS_TASKBITQUEUE::DeQNextTaskFromQueue(void){

	UNATIVECPU_t index;
	UNATIVECPU_t LeadingZeros;

	UNATIVECPU_t PriorityLevel;

	dBOS_LISTNODE<dBOS_TASK> * pResult;

#if(TOTAL_NUM_TASKPRIORITY_FLAG_WORDS == 1U)
	index = 0U;
#else
	index = TOTAL_NUM_TASKPRIORITY_FLAG_WORDS - 1U;	// Start search from highest priority task down.
	while( (index > 0U) && (m_Flags[index] == 0U) ){
		index--;
	}
#endif

	if(m_Flags[index] != 0U){

		LeadingZeros = DBOS_Port_CountLeadingZeros(m_Flags[index]);

		PriorityLevel = ((index * CPU_BITS_PER_WORD) + (CPU_BITS_PER_WORD - 1U)) - LeadingZeros;
		// Leading zeros can never be equal to CPU_BITS_PER_WORD since pList->Flags[index] is not equal to 0.

		ASSERT_FROM_OSCS(PriorityLevel < (MAXTASKPRIORITY + 1U))

		pResult = m_Queue[PriorityLevel].RemoveHead();

		ASSERT_FROM_OSCS(pResult != NULLPTR)

		if(m_Queue[PriorityLevel].GetHead() == NULLPTR){
			// There was only one task waiting at this priority level.

			m_Flags[index] &= ~((static_cast<UNATIVECPU_t>(0x01U)) << ((CPU_BITS_PER_WORD - 1U) - LeadingZeros));
		}

	}
	else{
		pResult = NULLPTR;	// List empty
	}

	return pResult;
}

/**
 * \brief Returns a pointer to the first task in the queue.
 *
 * @retval Pointer to the first task in the queue.
 * @retval NULLPTR Queue was empty.
 */
dBOS_LISTNODE<dBOS_TASK> * dBOS_TASKBITQUEUE::GetNextTaskFromQueue(void) const{

	UNATIVECPU_t index;
	UNATIVECPU_t LeadingZeros;

	UNATIVECPU_t PriorityLevel;

	dBOS_LISTNODE<dBOS_TASK> * pResult;

#if(TOTAL_NUM_TASKPRIORITY_FLAG_WORDS == 1U)
	index = 0U;
#else
	index = TOTAL_NUM_TASKPRIORITY_FLAG_WORDS - 1U;	// Start search from highest priority task down.
	while( (index > 0U) && (m_Flags[index] == 0U) ){
		index--;
	}
#endif

	if(m_Flags[index] != 0U){

		LeadingZeros = DBOS_Port_CountLeadingZeros(m_Flags[index]);

		PriorityLevel = ((index * CPU_BITS_PER_WORD) + (CPU_BITS_PER_WORD - 1U)) - LeadingZeros;
		// Leading zeros can never be equal to CPU_BITS_PER_WORD since pList->Flags[index] is not equal to 0.

		ASSERT_FROM_OSCS(PriorityLevel < (MAXTASKPRIORITY + 1U))

		pResult = m_Queue[PriorityLevel].GetHead();

		ASSERT_FROM_OSCS(pResult != NULLPTR)

	}
	else{
		pResult = NULLPTR;	// List empty
	}

	return pResult;
}

/**
 * \brief Adds a task into the queue.
 *
 * The task added is positioned as the <B>LAST</B> link of the priority level.
 *
 * @param[in] pNode pointer to the task to add to the queue.
 */
void dBOS_TASKBITQUEUE::AddTaskToQueue(dBOS_LISTNODE<dBOS_TASK> * const pNode){

	UNATIVECPU_t FlagWord;
	UNATIVECPU_t BitNumber;

	DBOS_TASK_PRIORITY_t Priority;

	ASSERT_FROM_OSCS(pNode != NULLPTR)

	Priority = pNode->GetItem()->GetPriority();

	FlagWord = (static_cast<UNATIVECPU_t>(Priority)) / CPU_BITS_PER_WORD;

	ASSERT_FROM_OSCS(FlagWord < TOTAL_NUM_TASKPRIORITY_FLAG_WORDS)

	BitNumber = Priority - (FlagWord * CPU_BITS_PER_WORD);

	m_Flags[FlagWord] |= ((static_cast<UNATIVECPU_t>(0x01U)) << BitNumber);

	m_Queue[Priority].AddItem_FIFO(pNode);

	return;
}

/**
 * \brief Removes a task from a queue.
 *
 * \note CurrentPriority of the Task being removed from the queue must not have change since it was added to or repositioned in the queue.
 *
 * @param[in] pNode pointer to the link to remove from the queue.
 */
void dBOS_TASKBITQUEUE::RemoveTaskFromQueue(dBOS_LISTNODE<dBOS_TASK> * const pNode){

	UNATIVECPU_t FlagWord;
	UNATIVECPU_t BitNumber;

	DBOS_TASK_PRIORITY_t CurrentPriority;

	ASSERT_FROM_OSCS(pNode != NULLPTR)

	CurrentPriority = pNode->GetItem()->GetPriority();

	FlagWord = (static_cast<UNATIVECPU_t>(CurrentPriority)) / CPU_BITS_PER_WORD;

	ASSERT_FROM_OSCS(FlagWord < TOTAL_NUM_TASKPRIORITY_FLAG_WORDS)

	BitNumber = CurrentPriority - (FlagWord * CPU_BITS_PER_WORD);

	m_Queue[CurrentPriority].RemoveItem(pNode);

	if(m_Queue[CurrentPriority].GetHead() == NULLPTR){
		m_Flags[FlagWord] &= ~((static_cast<UNATIVECPU_t>(0x01U)) << BitNumber);
	}

	return;
}

/**
 * \brief Called when the priority of the task has changed, the task is repositioned in the queue based on the specified new task priority.
 *
 * The task is removed from the queue and added back in at a position determined by the new task priority.\n
 * The task is positioned as the <B>FIRST</B> link of the priority level.
 *
 * \note CurrentPriority of the Task must not have change since it was added to or repositioned in the queue.
 *
 * @param[in] pNode pointer to the link that is to be repositioned in the queue.
 * @param[in] NewPriority priority which determines new position in the queue.
 */
DBOS_RESULT_t dBOS_TASKBITQUEUE::RequeueTaskInQueue(dBOS_LISTNODE<dBOS_TASK> * const pNode, DBOS_TASK_PRIORITY_t const NewPriority){

	UNATIVECPU_t FlagWord;
	UNATIVECPU_t BitNumber;

	ASSERT_FROM_OSCS(pNode != NULLPTR)

	dBOS_TASKBITQUEUE::RemoveTaskFromQueue(pNode);

	FlagWord = (static_cast<UNATIVECPU_t>(NewPriority)) / CPU_BITS_PER_WORD;

	ASSERT_FROM_OSCS(FlagWord < TOTAL_NUM_TASKPRIORITY_FLAG_WORDS)

	BitNumber = NewPriority - (FlagWord * CPU_BITS_PER_WORD);

	m_Flags[FlagWord] |= ((static_cast<UNATIVECPU_t>(0x01U)) << BitNumber);

	m_Queue[NewPriority].AddItem_LIFO(pNode);

	return DBOS_DONE;
}

/**
 * \brief Returns a pointer to the task at the given position.
 *
 * @param[in] Position Position in queue of task to get. First task is position 0.
 * @retval Pointer to task.
 * @retval NULLPTR Position is beyond end of queue.
 */
DBOS_TASK_CONST_P_t dBOS_TASKBITQUEUE::GetTaskAtPosition(UNATIVECPU_t Position) const{

	UNATIVECPU_t FlagWordIndex;
	UNATIVECPU_t BitNumber;
	UNATIVECPU_t Priority;

	UNATIVECPU_t LocalFlagWord;

	DBOS_TASK_CONST_P_t pResult = NULLPTR;
	UNATIVECPU_t TaskCount = 0U;

	dBOS_LISTNODE<dBOS_TASK> const * pNode;

	FlagWordIndex = TOTAL_NUM_TASKPRIORITY_FLAG_WORDS;

	while((FlagWordIndex > 0U) && (pResult == NULLPTR)){
		FlagWordIndex--;

		LocalFlagWord = m_Flags[FlagWordIndex];

		while(LocalFlagWord != 0U){

			BitNumber = DBOS_Port_CountLeadingZeros(LocalFlagWord);

			BitNumber = ((CPU_BITS_PER_WORD - 1U) - BitNumber);

			LocalFlagWord &= ~((static_cast<UNATIVECPU_t>(0x01U)) << BitNumber);

			Priority = BitNumber + (FlagWordIndex * CPU_BITS_PER_WORD);

			ASSERT_FROM_OSCS(Priority < (MAXTASKPRIORITY + 1U))

			pNode = m_Queue[Priority].GetHead();

			if(pNode != NULLPTR){
				while((pNode != NULLPTR) && (pResult == NULLPTR)){

					ASSERT_FROM_OSCS(pNode->GetItem() != NULLPTR)

					if(TaskCount == Position){
						pResult = pNode->GetItem();
					}

					TaskCount++;

					pNode = m_Queue[Priority].GetNext(pNode);

				}
			}

		}

	}

	return pResult;
}

BOOL dBOS_TASKBITQUEUE::TaskWouldBeFirstInQueue(dBOS_LISTNODE<dBOS_TASK> const * const pNode) const{

	BOOL Result;

	dBOS_LISTNODE<dBOS_TASK> const * const pHead = GetNextTaskFromQueue();

	if(pHead != NULLPTR){

		if(pNode->GetItem()->GetPriority() > pHead->GetItem()->GetPriority()){
			Result = TRUE;
		}
		else{
			Result = FALSE;
		}

	}
	else{
		Result = TRUE;
	}

	return Result;
}

dBOS_TASKQUEUE_ORDER_t dBOS_TASKBITQUEUE::GetOrderType(void) const{

	return dBOS_TASKQUEUE_ORDER_PRIORITY;
}

#if (DBOS_DEBUGOUTPUTENABLED == 1U)

/**
 * \brief Prints names of tasks the links in queue are pointing to starting from head of queue.
 *
 */
void dBOS_TASKBITQUEUE::PrintQueue() const{

	UNATIVECPU_t FlagWordIndex;
	UNATIVECPU_t BitNumber;
	UNATIVECPU_t Priority;

	UNATIVECPU_t LocalFlagWord;

	UNATIVECPU_t ListEmpty;

	dBOS_LISTNODE<dBOS_TASK> const * pNode;

	ListEmpty = 1U;

	FlagWordIndex = TOTAL_NUM_TASKPRIORITY_FLAG_WORDS;

	while(FlagWordIndex > 0U){
		FlagWordIndex--;

		LocalFlagWord = m_Flags[FlagWordIndex];

		while(LocalFlagWord != 0U){

			BitNumber = DBOS_Port_CountLeadingZeros(LocalFlagWord);

			BitNumber = ((CPU_BITS_PER_WORD - 1U) - BitNumber);

			LocalFlagWord &= ~((static_cast<UNATIVECPU_t>(0x01U)) << BitNumber);

			Priority = BitNumber + (FlagWordIndex * CPU_BITS_PER_WORD);

			ASSERT_FROM_OSCS(Priority < (MAXTASKPRIORITY + 1U))

			DBOS_Intprintf("\n  Priority: %u -> ", Priority);

			pNode = m_Queue[Priority].GetHead();

			if(pNode != NULLPTR){
				while(pNode != NULLPTR){

					ASSERT_FROM_OSCS(pNode->GetItem() != NULLPTR)

					pNode->GetItem()->PrintID();

					pNode = m_Queue[Priority].GetNext(pNode);

					if(pNode != NULLPTR){
						DBOS_Intprintf(", ");
					}
				}
			}
			else{
				DBOS_Intprintf("NONE");
			}

			ListEmpty = 0U;

		}

	}

	if(ListEmpty == 1U){
		DBOS_Intprintf("List Empty");
	}

	return;
}

#endif

// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
