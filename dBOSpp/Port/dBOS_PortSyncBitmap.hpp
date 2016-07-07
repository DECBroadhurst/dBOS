/**
 * @file dBOS_PortSyncBitmap.hpp
 * @author David Broadhurst
 * @date 18/04/2014
 *
 * \brief Definition of interface to synchronisation flags.
 *
 */

#ifndef PORT_DBOS_PORTSYNCBITMAP_HPP_
#define PORT_DBOS_PORTSYNCBITMAP_HPP_

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_PortTypes.hpp"
#include "dBOS_Config.hpp"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// EXTERNS (if a matching code file exists)

// -------------------------------------------------------------------------------------------------
// DEFINES

// -------------------------------------------------------------------------------------------------
// FORWARD DECLERATIONS

// -------------------------------------------------------------------------------------------------
// DATA TYPES

// -------------------------------------------------------------------------------------------------
// CONSTANTS
/**
 * \brief Total number of words in the array of words the hold the bits associated with each synchronisation object.
 *
 * @see S_SignaledFlags
 * @see S_BroadcastFlags
 */
#define TOTAL_NUM_ISR_FLAG_WORDS  (1U + ( (MAXNUMOFSYNCOBJECTS) / (CPU_BITS_PER_WORD + 1U) ) )

// -------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES

// -------------------------------------------------------------------------------------------------
// MACRO’S

// -------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
UNATIVECPU_t dBOS_PortSyncBitmap_CheckAndClearSignalledFlag(UNATIVECPU_t const Word);
void dBOS_PortSyncBitmap_SetSignalledFlag(UNATIVECPU_t const ObjectID);
UNATIVECPU_t dBOS_PortSyncBitmap_CheckAndClearBroadcastFlag(UNATIVECPU_t const Word);
void dBOS_PortSyncBitmap_SetBroadcastFlag(UNATIVECPU_t const ObjectID);

// -------------------------------------------------------------------------------------------------
// CLASSES

}// namespace

#endif /* PORT_DBOS_PORTSYNCBITMAP_HPP_ */
