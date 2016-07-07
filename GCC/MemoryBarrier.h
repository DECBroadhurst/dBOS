/*
 * MemoryBarrier.h
 *
 *  Created on: 29/09/2013
 *      Author: David Broadhurst
 */

#ifndef MEMORYBARRIER_H_
#define MEMORYBARRIER_H_

										// TODO FREERTOS uses DSB and ISB barrier instructions instead.
										// DMB recommended here http://mcuoneclipse.com/2014/01/26/entercritical-and-exitcritical-why-things-are-failing-badly/
#define MEMORY_BARRIER	asm volatile (	"DMB	\n"		\
										:				\
										:				\
										: "memory");
#endif /* MEMORYBARRIER_H_ */
