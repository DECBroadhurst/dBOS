// DATATYPES.h
//
//  Created on: 4/10/2009
//      Author: David Broadhurst
//-------------------------------------------------------------------------------------------------

#ifndef DATATYPES_H_
#define DATATYPES_H_

//-------------------------------------------------------------------------------------------------
//INCLUDE FILES

//-------------------------------------------------------------------------------------------------
//DEFINES

#define NULLPTR					nullptr

#define TRUE					1U

#define FALSE					0U


//-------------------------------------------------------------------------------------------------
//DATA TYPES
typedef unsigned char  			BOOLEAN;
typedef unsigned char			BOOL;
typedef char					CHAR;
typedef unsigned char  			INT8U;			// Unsigned  8 bit
typedef signed   char  			INT8S;       	// Signed    8 bit
typedef unsigned short  		INT16U;      	// Unsigned 16 bit
typedef signed   short 			INT16S;      	// Signed   16 bit
typedef unsigned int  			INT32U;      	// Unsigned 32 bit
typedef signed   int  			INT32S;      	// Signed   32 bit
typedef signed long long int	INT64S;

typedef INT32S 					SNATIVECPU_t;
typedef INT32U 					UNATIVECPU_t;
//-------------------------------------------------------------------------------------------------
//GLOBAL VARIABLES

//-------------------------------------------------------------------------------------------------
//FUNCTION PROTOTYPES

#endif /* DATATYPES_H_ */
