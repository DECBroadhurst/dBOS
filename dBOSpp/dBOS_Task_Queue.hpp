/**
 * @file dBOS_Task_Queue.hpp
 * @author David Broadhurst
 * @date 14/01/2015
 *
 * \brief Definition of task queue manipulation services.
 *
 */

#ifndef DBOS_TASK_QUEUE_HPP_
#define DBOS_TASK_QUEUE_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_Core_DataTypes.hpp"
#include "dBOS_Config.hpp"
#include "dBOS_List.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES
//lint -emacro((778), TOTAL_NUM_TASKPRIORITY_FLAG_WORDS) Info 778: Constant expression evaluates to 0 in operation '/'
#define TOTAL_NUM_TASKPRIORITY_FLAG_WORDS  (((MAXTASKPRIORITY) / (CPU_BITS_PER_WORD)) + 1U)

// -------------------------------------------------------------------------------------------------
// DATA TYPES
typedef enum {
	dBOS_TASKQUEUE_ORDER_PRIORITY = 0U, dBOS_TASKQUEUE_ORDER_FIFO
} dBOS_TASKQUEUE_ORDER_t;
// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// CLASSES

class dBOS_TASKQUEUE {

public:

	dBOS_TASKQUEUE(){
	}

	virtual ~dBOS_TASKQUEUE(){
	}

	virtual dBOS_LISTNODE<dBOS_TASK> * DeQNextTaskFromQueue(void) = 0;
	virtual dBOS_LISTNODE<dBOS_TASK> * GetNextTaskFromQueue(void) const = 0;
	virtual void AddTaskToQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink) = 0;
	virtual void RemoveTaskFromQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink) = 0;
	virtual DBOS_RESULT_t RequeueTaskInQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink, DBOS_TASK_PRIORITY_t const NewPriority) = 0;
	virtual DBOS_TASK_CONST_P_t GetTaskAtPosition(UNATIVECPU_t const Position) const = 0;
	virtual BOOL TaskWouldBeFirstInQueue(dBOS_LISTNODE<dBOS_TASK> const * const pLink) const = 0;
	virtual dBOS_TASKQUEUE_ORDER_t GetOrderType(void) const = 0;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintQueue() const = 0;
#endif

protected:

	dBOS_TASKQUEUE(dBOS_TASKQUEUE const &) = delete;
	dBOS_TASKQUEUE& operator=(dBOS_TASKQUEUE const &) = delete;

private:

};

class dBOS_TASKLINKEDQUEUE : public dBOS_TASKQUEUE {

public:

	explicit dBOS_TASKLINKEDQUEUE(dBOS_LINKEDQUEUE<dBOS_TASK> * const m_pQueueIn);
	virtual ~dBOS_TASKLINKEDQUEUE();

	dBOS_TASKLINKEDQUEUE() = delete;
	dBOS_TASKLINKEDQUEUE(dBOS_TASKLINKEDQUEUE const &) = delete;
	dBOS_TASKLINKEDQUEUE& operator=(dBOS_TASKLINKEDQUEUE const &) = delete;

	virtual dBOS_LISTNODE<dBOS_TASK> * DeQNextTaskFromQueue(void) override;
	virtual dBOS_LISTNODE<dBOS_TASK> * GetNextTaskFromQueue(void) const override;
	virtual void AddTaskToQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink) override;
	virtual void RemoveTaskFromQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink) override;
	virtual DBOS_RESULT_t RequeueTaskInQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink, DBOS_TASK_PRIORITY_t const NewPriority) = 0;
	virtual DBOS_TASK_CONST_P_t GetTaskAtPosition(UNATIVECPU_t const Position) const override;
	virtual BOOL TaskWouldBeFirstInQueue(dBOS_LISTNODE<dBOS_TASK> const * const pLink) const override;
	virtual dBOS_TASKQUEUE_ORDER_t GetOrderType(void) const = 0;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintQueue() const override;
#endif

protected:

	dBOS_LINKEDQUEUE<dBOS_TASK> * const m_pQueue;

private:

};

class dBOS_TASKPRIORITYLINKEDQUEUE : public dBOS_TASKLINKEDQUEUE {

public:

	dBOS_TASKPRIORITYLINKEDQUEUE();
	virtual ~dBOS_TASKPRIORITYLINKEDQUEUE();

	dBOS_TASKPRIORITYLINKEDQUEUE(dBOS_TASKPRIORITYLINKEDQUEUE const &) = delete;
	dBOS_TASKPRIORITYLINKEDQUEUE& operator=(dBOS_TASKPRIORITYLINKEDQUEUE const &) = delete;

	virtual DBOS_RESULT_t RequeueTaskInQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink, DBOS_TASK_PRIORITY_t const NewPriority) override;
	virtual dBOS_TASKQUEUE_ORDER_t GetOrderType(void) const override;

protected:

private:

};

class dBOS_TASKFIFOLINKEDQUEUE : public dBOS_TASKLINKEDQUEUE {

public:

	dBOS_TASKFIFOLINKEDQUEUE();
	virtual ~dBOS_TASKFIFOLINKEDQUEUE();

	dBOS_TASKFIFOLINKEDQUEUE(dBOS_TASKFIFOLINKEDQUEUE const &) = delete;
	dBOS_TASKFIFOLINKEDQUEUE& operator=(dBOS_TASKFIFOLINKEDQUEUE const &) = delete;

	virtual DBOS_RESULT_t RequeueTaskInQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink, DBOS_TASK_PRIORITY_t const NewPriority) override;
	virtual dBOS_TASKQUEUE_ORDER_t GetOrderType(void) const override;

protected:

private:

};

class dBOS_TASKBITQUEUE : public dBOS_TASKQUEUE {

public:

	dBOS_TASKBITQUEUE();
	virtual ~dBOS_TASKBITQUEUE();

	dBOS_TASKBITQUEUE(dBOS_TASKBITQUEUE const &) = delete;
	dBOS_TASKBITQUEUE& operator=(dBOS_TASKBITQUEUE const &) = delete;

	virtual dBOS_LISTNODE<dBOS_TASK> * DeQNextTaskFromQueue(void) override;
	virtual dBOS_LISTNODE<dBOS_TASK> * GetNextTaskFromQueue(void) const override;
	virtual void AddTaskToQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink) override;
	virtual void RemoveTaskFromQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink) override;
	virtual DBOS_RESULT_t RequeueTaskInQueue(dBOS_LISTNODE<dBOS_TASK> * const pLink, DBOS_TASK_PRIORITY_t const NewPriority) override;
	virtual DBOS_TASK_CONST_P_t GetTaskAtPosition(UNATIVECPU_t Position) const override;
	virtual BOOL TaskWouldBeFirstInQueue(dBOS_LISTNODE<dBOS_TASK> const * const pLink) const override;
	virtual dBOS_TASKQUEUE_ORDER_t GetOrderType(void) const override;

#if (DBOS_DEBUGOUTPUTENABLED == 1U)
	virtual void PrintQueue() const override;
#endif

protected:

private:

	UNATIVECPU_t m_Flags[TOTAL_NUM_TASKPRIORITY_FLAG_WORDS];
	dBOS_FIFOLINKEDQUEUE<dBOS_TASK> m_Queue[MAXTASKPRIORITY + 1U];				// +1 for idle task which has priority 0

};

}// namespace

#endif /* DBOS_TASK_QUEUE_HPP_ */
