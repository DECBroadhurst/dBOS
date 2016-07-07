/**
 * @file dBOS_PortSyncBitmap.cpp
 * @author David Broadhurst
 * @date 18/04/2015
 *
 * \brief Implementation of synchronisation flags.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_PortSyncBitmap.hpp"
#include "dBOS_PortFunctions.hpp"
#include "BitBand.h"
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
/**
 * \brief Array of signalled flags.
 *
 * Each synchronisation object (that inherits from dBOS_SYNCBASE) has an associated bit in the
 * array S_Signalled flags. The flag is set when the object has been signalled from an ISR.
 */
static UNATIVECPU_t S_SignalledFlags[TOTAL_NUM_ISR_FLAG_WORDS] = {0U};

/**
 * \brief Array of broadcast flags.
 *
 * Each synchronisation object (that inherits from dBOS_SYNCBASE) has an associated bit in the
 * array S_Broadcast flags. The flag is set when the object has been set broadcast from an ISR.
 */
static UNATIVECPU_t S_BroadcastFlags[TOTAL_NUM_ISR_FLAG_WORDS] = {0U};

// -------------------------------------------------------------------------------------------------
// LOCAL VARIABLES SHARED WITH INTERRUPT SERVICE ROUTINES

// -------------------------------------------------------------------------------------------------
// LOCAL FUNCTION PROTOTYPES

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
/**
 * \brief Finds the set bit of least significance of the specified word, clears the bit and returns the bit number.
 *
 * @param[in] Word Word number to use. Between 0 and (TOTAL_NUM_ISR_FLAG_WORDS - 1) inclusive.
 * @return Bit number of the set bit, 0 if the LSb was set. Width of word if no bits were set.
 */
UNATIVECPU_t dBOS_PortSyncBitmap_CheckAndClearSignalledFlag(UNATIVECPU_t const Word){

	UNATIVECPU_t Bit;

	Bit = DBOS_Port_CountTrailingZeros(S_SignalledFlags[Word]);

	if(Bit < CPU_BITS_PER_WORD){
		Var_ResetBit_BB(&S_SignalledFlags[Word], Bit);
	}

	return Bit;
}

/**
 * \brief Sets the signalled bit associated with the specified object.
 *
 * @param[in] ObjectID 0 to (MAXNUMOFSYNCOBJECTS - 1) inclusive.
 * @retval None.
 */
void dBOS_PortSyncBitmap_SetSignalledFlag(UNATIVECPU_t const ObjectID){

	UNATIVECPU_t FlagWord;
	UNATIVECPU_t BitNumber;

	FlagWord = ObjectID / CPU_BITS_PER_WORD;

	ASSERT_FROM_OSCS(FlagWord < TOTAL_NUM_ISR_FLAG_WORDS)

	BitNumber = ObjectID - (FlagWord * CPU_BITS_PER_WORD);

	Var_SetBit_BB(&S_SignalledFlags[FlagWord], BitNumber);

	return;
}

/**
 * \brief Finds the set bit of least significance of the specified word, clears the bit and returns the bit number.
 *
 * @param[in] Word Word number to use. Between 0 and (TOTAL_NUM_ISR_FLAG_WORDS - 1) inclusive.
 * @return Bit number of the set bit, 0 if the LSb was set. Width of word if no bits were set.
 */
UNATIVECPU_t dBOS_PortSyncBitmap_CheckAndClearBroadcastFlag(UNATIVECPU_t const Word){

	UNATIVECPU_t Bit;

	Bit = DBOS_Port_CountTrailingZeros(S_BroadcastFlags[Word]);

	if(Bit < CPU_BITS_PER_WORD){
		Var_ResetBit_BB(&S_BroadcastFlags[Word], Bit);
	}

	return Bit;
}

/**
 * \brief Sets the broadcast bit associated with the specified object.
 *
 * @param[in] ObjectID 0 to (MAXNUMOFSYNCOBJECTS - 1) inclusive.
 * @retval None.
 */
void dBOS_PortSyncBitmap_SetBroadcastFlag(UNATIVECPU_t const ObjectID){

	UNATIVECPU_t FlagWord;
	UNATIVECPU_t BitNumber;

	FlagWord = ObjectID / CPU_BITS_PER_WORD;

	ASSERT_FROM_OSCS(FlagWord < TOTAL_NUM_ISR_FLAG_WORDS)

	BitNumber = ObjectID - (FlagWord * CPU_BITS_PER_WORD);

	Var_SetBit_BB(&S_BroadcastFlags[FlagWord], BitNumber);

	return;
}
// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
