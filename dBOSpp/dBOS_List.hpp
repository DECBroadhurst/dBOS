/**
 * @file dBOS_List.hpp
 * @author David Broadhurst
 * @date 21/03/2015
 *
 * \brief Implementation of dBOS List.
 *
 */

#ifndef DBOS_LIST_HPP_
#define DBOS_LIST_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_DataTypes.hpp"
#include "dBOS_Assert.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// FORWARD DECLERATIONS
template<class Item>
class dBOS_LISTNODE;

// -------------------------------------------------------------------------------------------------
// DATA TYPES

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// CLASSES

template<class Item>
class dBOS_LINKEDLIST {

public:

	dBOS_LINKEDLIST();
	virtual ~dBOS_LINKEDLIST();

	dBOS_LINKEDLIST(dBOS_LINKEDLIST const &) = delete;
	dBOS_LINKEDLIST& operator=(dBOS_LINKEDLIST const &) = delete;

	void AddItemToFront(dBOS_LISTNODE<Item> * const pNode);
	void AddItemToBack(dBOS_LISTNODE<Item> * const pNode);

	void RemoveItem(dBOS_LISTNODE<Item> * const pNode);

	dBOS_LISTNODE<Item> * RemoveHead();
	dBOS_LISTNODE<Item> * RemoveTail();

	dBOS_LISTNODE<Item> * GetHead() const;
	dBOS_LISTNODE<Item> * GetTail() const;

	dBOS_LISTNODE<Item> const * GetNext(dBOS_LISTNODE<Item> const * const pNode) const;
	dBOS_LISTNODE<Item> const * GetPrev(dBOS_LISTNODE<Item> const * const pNode) const;

protected:

	dBOS_LISTNODE<Item> * m_pHead;
	dBOS_LISTNODE<Item> * m_pTail;

private:

};

// ------------------------------------------------------------------------------------------------
template<class Item>
class dBOS_LINKEDQUEUE : public dBOS_LINKEDLIST<Item> {

public:

	dBOS_LINKEDQUEUE();
	virtual ~dBOS_LINKEDQUEUE();

	dBOS_LINKEDQUEUE(dBOS_LINKEDQUEUE const &) = delete;
	dBOS_LINKEDQUEUE& operator=(dBOS_LINKEDQUEUE const &) = delete;

	virtual void AddItem(dBOS_LISTNODE<Item> * const pNode) = 0;
	virtual void AddItem_FIFO(dBOS_LISTNODE<Item> * const pNode) = 0;
	virtual void AddItem_FIFO(dBOS_LISTNODE<Item> * const pNode, INT32S const Priority) = 0;
	virtual void AddItem_LIFO(dBOS_LISTNODE<Item> * const pNode) = 0;
	virtual void AddItem_LIFO(dBOS_LISTNODE<Item> * const pNode, INT32S const Priority) = 0;
	virtual BOOL ItemWouldBeFirstInQueue(dBOS_LISTNODE<Item> const * const pNode) const = 0;

	dBOS_LISTNODE<Item> const * GetItemAtPosition(UNATIVECPU_t const Position) const;

protected:

private:

};

// ------------------------------------------------------------------------------------------------
template<class Item>
class dBOS_PRIORITYLINKEDQUEUE : public dBOS_LINKEDQUEUE<Item> {

public:

	dBOS_PRIORITYLINKEDQUEUE();
	virtual ~dBOS_PRIORITYLINKEDQUEUE();

	dBOS_PRIORITYLINKEDQUEUE(dBOS_PRIORITYLINKEDQUEUE const &) = delete;
	dBOS_PRIORITYLINKEDQUEUE& operator=(dBOS_PRIORITYLINKEDQUEUE const &) = delete;

	virtual void AddItem(dBOS_LISTNODE<Item> * const pNode) override;
	virtual void AddItem_FIFO(dBOS_LISTNODE<Item> * const pNode) override;
	virtual void AddItem_FIFO(dBOS_LISTNODE<Item> * const pNode, INT32S const Priority) override;
	virtual void AddItem_LIFO(dBOS_LISTNODE<Item> * const pNode) override;
	virtual void AddItem_LIFO(dBOS_LISTNODE<Item> * const pNode, INT32S const Priority) override;
	virtual BOOL ItemWouldBeFirstInQueue(dBOS_LISTNODE<Item> const * const pNode) const override;

protected:

private:

};

// ------------------------------------------------------------------------------------------------
template<class Item>
class dBOS_FIFOLINKEDQUEUE : public dBOS_LINKEDQUEUE<Item> {

public:

	dBOS_FIFOLINKEDQUEUE();
	virtual ~dBOS_FIFOLINKEDQUEUE();

	dBOS_FIFOLINKEDQUEUE(dBOS_FIFOLINKEDQUEUE const &) = delete;
	dBOS_FIFOLINKEDQUEUE& operator=(dBOS_FIFOLINKEDQUEUE const &) = delete;

	virtual void AddItem(dBOS_LISTNODE<Item> * const pNode) override;
	virtual void AddItem_FIFO(dBOS_LISTNODE<Item> * const pNode) override;
	virtual void AddItem_FIFO(dBOS_LISTNODE<Item> * const pNode, INT32S const Priority) override;
	virtual void AddItem_LIFO(dBOS_LISTNODE<Item> * const pNode) override;
	virtual void AddItem_LIFO(dBOS_LISTNODE<Item> * const pNode, INT32S const Priority) override;
	virtual BOOL ItemWouldBeFirstInQueue(dBOS_LISTNODE<Item> const * const pNode) const override;

protected:

private:

};

// ------------------------------------------------------------------------------------------------
template<class Item>
class dBOS_LISTNODE {

public:

	dBOS_LISTNODE();
	explicit dBOS_LISTNODE(Item * const pItem);
	~dBOS_LISTNODE();

	dBOS_LISTNODE(dBOS_LISTNODE const &) = delete;
	dBOS_LISTNODE& operator=(dBOS_LISTNODE const &) = delete;

	Item * GetItem(void) const;
	void SetItem(Item * const pItem);
	BOOL CheckUnlinked(void) const;

protected:

private:

	friend class dBOS_LINKEDLIST<Item> ;
	friend class dBOS_LINKEDQUEUE<Item> ;
	friend class dBOS_PRIORITYLINKEDQUEUE<Item> ;
	friend class dBOS_FIFOLINKEDQUEUE<Item> ;

	Item * m_pItem;

	dBOS_LISTNODE * m_pNext;
	dBOS_LISTNODE * m_pPrev;

	dBOS_LINKEDLIST<Item> const * m_pManager;

};

// ------------------------------------------------------------------------------------------------
template<class Item>
dBOS_LINKEDLIST<Item>::dBOS_LINKEDLIST() :
		m_pHead(NULLPTR), m_pTail(NULLPTR){

}

template<class Item>
dBOS_LINKEDLIST<Item>::~dBOS_LINKEDLIST(){

}

template<class Item>
void dBOS_LINKEDLIST<Item>::AddItemToFront(dBOS_LISTNODE<Item> * const pNode){

	ASSERT_FROM_OSCS(pNode != NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pNext == NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pPrev == NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pManager == NULLPTR)
	ASSERT_FROM_OSCS(pNode != m_pHead)
	ASSERT_FROM_OSCS(pNode != m_pTail)

	if(m_pHead != NULLPTR){
		m_pHead->m_pPrev = pNode;

		pNode->m_pNext = m_pHead;
	}
	else{
		// List is empty.
		ASSERT_FROM_OSCS(m_pTail == NULLPTR)

		m_pTail = pNode;
	}

	pNode->m_pManager = this;

	m_pHead = pNode;

	return;
}

template<class Item>
void dBOS_LINKEDLIST<Item>::AddItemToBack(dBOS_LISTNODE<Item> * const pNode){

	ASSERT_FROM_OSCS(pNode != NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pNext == NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pPrev == NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pManager == NULLPTR)
	ASSERT_FROM_OSCS(pNode != m_pHead)
	ASSERT_FROM_OSCS(pNode != m_pTail)

	if(m_pTail != NULLPTR){
		m_pTail->m_pNext = pNode;

		pNode->m_pPrev = m_pTail;
	}
	else{
		// List is empty.
		ASSERT_FROM_OSCS(m_pHead == NULLPTR)

		m_pHead = pNode;
	}

	pNode->m_pManager = this;

	m_pTail = pNode;

	return;
}

template<class Item>
void dBOS_LINKEDLIST<Item>::RemoveItem(dBOS_LISTNODE<Item> * const pNode){

	ASSERT_FROM_OSCS(pNode != NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pManager == this)
	ASSERT_FROM_OSCS((pNode->m_pNext != NULLPTR) || (pNode->m_pPrev != NULLPTR) || ((pNode == m_pHead) && (pNode == m_pTail)))

	if(pNode->m_pNext != NULLPTR){
		pNode->m_pNext->m_pPrev = pNode->m_pPrev;
	}

	if(pNode->m_pPrev != NULLPTR){
		pNode->m_pPrev->m_pNext = pNode->m_pNext;
	}

	if(pNode == m_pHead){
		ASSERT_FROM_OSCS(pNode->m_pPrev == NULLPTR)

		m_pHead = pNode->m_pNext;
	}

	if(pNode == m_pTail){
		ASSERT_FROM_OSCS(pNode->m_pNext == NULLPTR)

		m_pTail = pNode->m_pPrev;
	}

	pNode->m_pNext = NULLPTR;
	pNode->m_pPrev = NULLPTR;

	pNode->m_pManager = NULLPTR;

	return;
}

template<class Item>
dBOS_LISTNODE<Item> * dBOS_LINKEDLIST<Item>::RemoveHead(){

	dBOS_LISTNODE<Item> * pResult;

	pResult = m_pHead;

	if(pResult != NULLPTR){

		m_pHead = pResult->m_pNext;

		pResult->m_pNext = NULLPTR;
		ASSERT_FROM_OSCS(pResult->m_pPrev == NULLPTR)

		if(m_pHead != NULLPTR){
			m_pHead->m_pPrev = NULLPTR;
		}
		else{
			m_pTail = NULLPTR;
		}

		pResult->m_pManager = NULLPTR;
	}

	return pResult;
}

template<class Item>
dBOS_LISTNODE<Item> * dBOS_LINKEDLIST<Item>::RemoveTail(){

	dBOS_LISTNODE<Item> * pResult;

	pResult = m_pTail;

	if(m_pTail != NULLPTR){

		m_pTail = m_pTail->m_pPrev;

		pResult->m_pPrev = NULLPTR;
		ASSERT_FROM_OSCS(pResult->m_pNext == NULLPTR)

		if(m_pTail != NULLPTR){
			m_pTail->m_pNext = NULLPTR;
		}
		else{
			m_pHead = NULLPTR;
		}

		pResult->m_pManager = NULLPTR;
	}

	return pResult;
}

template<class Item>
dBOS_LISTNODE<Item> * dBOS_LINKEDLIST<Item>::GetHead() const{

	return m_pHead;
}

template<class Item>
dBOS_LISTNODE<Item> * dBOS_LINKEDLIST<Item>::GetTail() const{

	return m_pTail;
}

template<class Item>
dBOS_LISTNODE<Item> const * dBOS_LINKEDLIST<Item>::GetNext(dBOS_LISTNODE<Item> const * const pNode) const{

	dBOS_LISTNODE<Item> * Result;

	if(pNode == NULLPTR){
		Result = NULLPTR;
	}
	else{
		Result = pNode->m_pNext;
	}

	return Result;
}

template<class Item>
dBOS_LISTNODE<Item> const * dBOS_LINKEDLIST<Item>::GetPrev(dBOS_LISTNODE<Item> const * const pNode) const{

	dBOS_LISTNODE<Item> * Result;

	if(pNode == NULLPTR){
		Result = NULLPTR;
	}
	else{
		Result = pNode->m_pPrev;
	}

	return Result;
}

// ------------------------------------------------------------------------------------------------
template<class Item>
dBOS_LINKEDQUEUE<Item>::dBOS_LINKEDQUEUE(){

}

template<class Item>
dBOS_LINKEDQUEUE<Item>::~dBOS_LINKEDQUEUE(){

}

template<class Item>
dBOS_LISTNODE<Item> const * dBOS_LINKEDQUEUE<Item>::GetItemAtPosition(UNATIVECPU_t const Position) const{

	dBOS_LISTNODE<Item> const * pResult = NULLPTR;

	UNATIVECPU_t Count = 0U;

	dBOS_LISTNODE<Item> const * pCurrent;

	pCurrent = this->m_pHead;

	while((pCurrent != NULLPTR) && (pResult == NULLPTR)){

		if(Count == Position){
			pResult = pCurrent;
		}
		Count++;

		pCurrent = pCurrent->m_pNext;
	}

	return pResult;
}

// ------------------------------------------------------------------------------------------------
template<class Item>
dBOS_PRIORITYLINKEDQUEUE<Item>::dBOS_PRIORITYLINKEDQUEUE(){

}

template<class Item>
dBOS_PRIORITYLINKEDQUEUE<Item>::~dBOS_PRIORITYLINKEDQUEUE(){

}

template<class Item>
void dBOS_PRIORITYLINKEDQUEUE<Item>::AddItem(dBOS_LISTNODE<Item> * const pNode){

	AddItem_FIFO(pNode);

	return;
}

template<class Item>
void dBOS_PRIORITYLINKEDQUEUE<Item>::AddItem_FIFO(dBOS_LISTNODE<Item> * const pNode){

	ASSERT_FROM_OSCS(pNode != NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pItem != NULLPTR)

	this->AddItem_FIFO(pNode, pNode->m_pItem->GetPriority());

	return;
}

template<class Item>
void dBOS_PRIORITYLINKEDQUEUE<Item>::AddItem_FIFO(dBOS_LISTNODE<Item> * const pNode, INT32S const Priority){

	dBOS_LISTNODE<Item> * pCurrentObject;

	ASSERT_FROM_OSCS(pNode != NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pPrev == NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pNext == NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pManager == NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pItem != NULLPTR)

	if(this->m_pHead == NULLPTR){ // No objects in this list.

		ASSERT_FROM_OSCS(this->m_pTail == NULLPTR)
		// List tail should also be NULL if head is NULL

		this->m_pHead = pNode;
		this->m_pTail = pNode;
	}
	else{

		ASSERT_FROM_OSCS(this->m_pTail != NULLPTR)
		// List tail should also be NULL if head is NULL

		pCurrentObject = this->m_pHead;

		if(Priority > (pCurrentObject->m_pItem->GetPriority())){
			// Object to be added is to be the first in the list.
			pCurrentObject->m_pPrev = pNode;
			pNode->m_pNext = pCurrentObject;
			this->m_pHead = pNode;
		}
		else{
			while(pNode->m_pPrev == NULLPTR){ // Already asserted pLink->pPrev == NULL to start with.
				if(pCurrentObject->m_pNext == NULLPTR){
					// End of the list.
					pCurrentObject->m_pNext = pNode;
					pNode->m_pPrev = pCurrentObject;
					this->m_pTail = pNode;
				}
				else if(Priority > (pCurrentObject->m_pNext->m_pItem->GetPriority())){
					// using > since this link is placed as LAST link at its priority level.
					pNode->m_pNext = pCurrentObject->m_pNext; // Insert after Current Object (before next object with lower priority)
					pNode->m_pPrev = pCurrentObject;
					pCurrentObject->m_pNext = pNode;
					pNode->m_pNext->m_pPrev = pNode;
				}
				else{
					pCurrentObject = pCurrentObject->m_pNext;
				}
			}
		}

	}

	pNode->m_pManager = this;

	return;
}

template<class Item>
void dBOS_PRIORITYLINKEDQUEUE<Item>::AddItem_LIFO(dBOS_LISTNODE<Item> * const pNode){

	ASSERT_FROM_OSCS(pNode != NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pItem != NULLPTR)

	this->AddItem_LIFO(pNode, pNode->m_pItem->GetPriority());

	return;
}

template<class Item>
void dBOS_PRIORITYLINKEDQUEUE<Item>::AddItem_LIFO(dBOS_LISTNODE<Item> * const pNode, INT32S const Priority){

	dBOS_LISTNODE<Item> * pCurrentObject;

	ASSERT_FROM_OSCS(pNode != NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pPrev == NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pNext == NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pManager == NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pItem != NULLPTR)

	if(this->m_pHead == NULLPTR){ // No objects in this list.

		ASSERT_FROM_OSCS(this->m_pTail == NULLPTR)
		// List tail should also be NULL if head is NULL

		this->m_pHead = pNode;
		this->m_pTail = pNode;
	}
	else{

		ASSERT_FROM_OSCS(this->m_pTail != NULLPTR)
		// List tail should also be NULL if head is NULL

		pCurrentObject = this->m_pHead;

		if(Priority >= (pCurrentObject->m_pItem->GetPriority())){
			// Object to be added is to be the first in the list.
			pCurrentObject->m_pPrev = pNode;
			pNode->m_pNext = pCurrentObject;
			this->m_pHead = pNode;
		}
		else{
			while(pNode->m_pPrev == NULLPTR){ // Already asserted pLink->pPrev == NULL to start with.
				if(pCurrentObject->m_pNext == NULLPTR){
					// End of the list.
					pCurrentObject->m_pNext = pNode;
					pNode->m_pPrev = pCurrentObject;
					this->m_pTail = pNode;
				}
				else if(Priority >= (pCurrentObject->m_pNext->m_pItem->GetPriority())){
					// using >= since this link is placed as FIRST link at its priority level.
					pNode->m_pNext = pCurrentObject->m_pNext; // Insert after Current Object (before next object with lower priority)
					pNode->m_pPrev = pCurrentObject;
					pCurrentObject->m_pNext = pNode;
					pNode->m_pNext->m_pPrev = pNode;
				}
				else{
					pCurrentObject = pCurrentObject->m_pNext;
				}
			}
		}

	}

	pNode->m_pManager = this;

	return;
}

template<class Item>
BOOL dBOS_PRIORITYLINKEDQUEUE<Item>::ItemWouldBeFirstInQueue(dBOS_LISTNODE<Item> const * const pNode) const{

	BOOL Result;

	ASSERT_FROM_OSCS(pNode != NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pItem != NULLPTR)
	ASSERT_FROM_OSCS(pNode->m_pManager == NULLPTR)

	if(this->m_pHead != NULLPTR){

		if(pNode->m_pItem->GetPriority() > this->m_pHead->m_pItem->GetPriority()){
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

// ------------------------------------------------------------------------------------------------
template<class Item>
dBOS_FIFOLINKEDQUEUE<Item>::dBOS_FIFOLINKEDQUEUE(){

}

template<class Item>
dBOS_FIFOLINKEDQUEUE<Item>::~dBOS_FIFOLINKEDQUEUE(){

}

template<class Item>
void dBOS_FIFOLINKEDQUEUE<Item>::AddItem(dBOS_LISTNODE<Item> * const pNode){

	this->AddItemToBack(pNode);

	return;
}

template<class Item>
void dBOS_FIFOLINKEDQUEUE<Item>::AddItem_FIFO(dBOS_LISTNODE<Item> * const pNode){

	this->AddItemToBack(pNode);

	return;
}

template<class Item>
void dBOS_FIFOLINKEDQUEUE<Item>::AddItem_FIFO(dBOS_LISTNODE<Item> * const pNode, INT32S const Priority){

	(void) Priority;

	this->AddItemToBack(pNode);

	return;
}

template<class Item>
void dBOS_FIFOLINKEDQUEUE<Item>::AddItem_LIFO(dBOS_LISTNODE<Item> * const pNode){

	this->AddItemToFront(pNode);

	return;
}

template<class Item>
void dBOS_FIFOLINKEDQUEUE<Item>::AddItem_LIFO(dBOS_LISTNODE<Item> * const pNode, INT32S const Priority){

	(void) Priority;

	this->AddItemToFront(pNode);

	return;
}

template<class Item>
BOOL dBOS_FIFOLINKEDQUEUE<Item>::ItemWouldBeFirstInQueue(dBOS_LISTNODE<Item> const * const pNode) const{

	(void) pNode; // Unused parameter.

	BOOL Result;

	if(this->m_pHead != NULLPTR){
		Result = FALSE;
	}
	else{
		Result = TRUE;
	}

	return Result;
}

// ------------------------------------------------------------------------------------------------
template<class Item>
dBOS_LISTNODE<Item>::dBOS_LISTNODE() :
		m_pItem(NULLPTR), m_pNext(NULLPTR), m_pPrev(NULLPTR), m_pManager(NULLPTR){

}

template<class Item>
dBOS_LISTNODE<Item>::dBOS_LISTNODE(Item * const pItem) :
		m_pItem(pItem), m_pNext(NULLPTR), m_pPrev(NULLPTR), m_pManager(NULLPTR){

}

template<class Item>
dBOS_LISTNODE<Item>::~dBOS_LISTNODE(){

}

template<class Item>
Item * dBOS_LISTNODE<Item>::GetItem(void) const{

	ASSERT_FROM_OSCS(m_pItem != NULLPTR)

	return m_pItem;
}

template<class Item>
void dBOS_LISTNODE<Item>::SetItem(Item * const pItem){

	ASSERT_FROM_OSCS(m_pItem == NULLPTR)

	m_pItem = pItem;
}

template<class Item>
BOOL dBOS_LISTNODE<Item>::CheckUnlinked(void) const{

	BOOL Result;

	if(m_pManager == NULLPTR){
		Result = TRUE;
	}
	else{
		Result = FALSE;
	}

	return Result;
}

}// namespace

#endif /* DBOS_LIST_HPP_ */
