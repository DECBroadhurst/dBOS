/**
 * @file minicpp.cpp
 * @author David Broadhurst
 * @date 21/10/2014
 *
 * \brief Implementation of dBOS mini cpp.
 *
 */

#include <stdlib.h>
#include <new>
#include "DataTypes.h"
#include "ProjConfig.h"
#include "Port\dBOS_Port.hpp"

#include "dBOS_IntDebugPrint.hpp"

using namespace dBOS;

static INT8U Heap[HEAPSIZE] __attribute__((aligned(HEAPALIGN)));

static INT8U *HeapPnt = Heap;
static INT32U S_Count = 0;

void HeapReset(void){

	HeapPnt = Heap;
	S_Count = 0U;

	return;
}

void *operator new(size_t size) noexcept{ //throw(){

	void * Result;

	INTERRUPT_MASK_t PrevISRMask;

	if(size & (HEAPALIGN - 1U)){

		size &= ~(HEAPALIGN - 1U);

		size += HEAPALIGN;

	}

	// TODO Add Out of Memory message.

	PrevISRMask = DBOS_Port_IncreaseInterruptMaskingLevel(ISRMASK_OSCRITICAL);	// Enter critical section
	// This maybe called from with in the OS CS or directly from the application.

//	DBOS_Intprintf("\n\rNew: %X, Size: %u", (INT32U) HeapPnt, size);

	if(S_Count + size > HEAPSIZE){
		Result = NULLPTR;
	}
	else{

		Result = (void *) HeapPnt;

		HeapPnt += size;
		S_Count += size;
	}

	if(Result == NULLPTR){
		DBOS_Intprintf("\n\r\n\rOUT OF MEMORY");

		while(1){

		}
	}
//	else{
//		DBOS_Intprintf("\n\rNew: %X, Size: %u, Total: %u", (INT32U) HeapPnt, size, S_Count);
//	}

	DBOS_Port_DecreaseInterruptMaskingLevel(PrevISRMask);			// Exit critical section

	return Result;
}

//............................................................................
void operator delete(void *p) noexcept{			//throw(){

	return;
}

void* operator new[](size_t size) noexcept
{
	return operator new(size); // Same as regular new
}

void operator delete[](void *p) noexcept
{
	operator delete(p); // Same as regular delete
}

void* operator new(size_t size, std::nothrow_t) noexcept
{
	return operator new(size); // Same as regular new
}

void operator delete(void *p, std::nothrow_t) noexcept
{
	operator delete(p); // Same as regular delete
}

void* operator new[](size_t size, std::nothrow_t) noexcept
{
	return operator new(size); // Same as regular new
}

void operator delete[](void *p, std::nothrow_t) noexcept
{
	operator delete(p); // Same as regular delete
}

//............................................................................
extern "C" int __aeabi_atexit(void *object, void (*destructor)(void *), void *dso_handle){
	return 0;
}

extern "C" void __cxa_pure_virtual(){
	while(1)
		;
}

