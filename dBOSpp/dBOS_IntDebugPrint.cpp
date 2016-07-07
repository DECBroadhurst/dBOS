/**
 * @file dBOS_IntDebugPrint.cpp
 * @author David Broadhurst
 * @date 11/04/2014
 *
 * \brief Implementation of print output functionality.
 *
 */

// -------------------------------------------------------------------------------------------------
// INCLUDE FILES
#include "dBOS_IntDebugPrint.hpp"
#include "ProjUARTs.h"

namespace dBOS {
// -------------------------------------------------------------------------------------------------
// LOCAL DEFINES
#define MAX_BCD_LENGTH		10
#define PRINT_BUFFER_LENGTH	12

#define WIDTHSTATEINT		0
#define WIDTHSTATELONG		1
#define WIDTHSTATELONGLONG	2
#define WIDTHSTATESHORT		3
#define WIDTHSTATECHAR		4

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
static void S_DBOS_DebugPrintINT32UHex(INT32U V, BOOL LowerCase);
static void S_DBOS_DebugPrint_INT32ToBCD(INT32U V, CHAR *S);

// -------------------------------------------------------------------------------------------------
// PUBLIC FUNCTIONS
void DBOS_Print_IntReturnValue(DBOS_RESULT_t Result){

	switch(Result){
	case DBOS_DONE:
		DBOS_Intprintf("DBOS_DONE");
		break;
	case DBOS_TASK_SWITCHED:
		DBOS_Intprintf("DBOS_TASK_SWITCHED");
		break;
	case DBOS_TASK_NOT_SWITCHED:
		DBOS_Intprintf("DBOS_TASK_NOT_SWITCHED");
		break;
	case DBOS_RUNNING_TASK_PRIORITY_LOWERED:
		DBOS_Intprintf("DBOS_RUNNING_TASK_PRIORITY_LOWERED");
		break;
	case DBOS_RUNNING_TASK_PRIORITY_RAISED:
		DBOS_Intprintf("DBOS_RUNNING_TASK_PRIORITY_RAISED");
		break;
	case DBOS_TASK_READY_TASK_PRIORITY_LOWERED:
		DBOS_Intprintf("DBOS_TASK_READY_TASK_PRIORITY_LOWERED");
		break;
	case DBOS_TASK_READY_TASK_PRIORITY_RAISED:
		DBOS_Intprintf("DBOS_TASK_READY_TASK_PRIORITY_RAISED");
		break;
	case DBOS_MUTEX_ERROR_NOT_A_RECURSIVE_MUTEX:
		DBOS_Intprintf("DBOS_MUTEX_ERROR_NOT_A_RECURSIVE_MUTEX");
		break;
	case DBOS_MUTEX_ERROR_THIS_TASK_NOT_THE_OWNER:
		DBOS_Intprintf("DBOS_MUTEX_ERROR_THIS_TASK_NOT_THE_OWNER");
		break;
	case DBOS_MUTEX_ERROR_HAS_NO_OWNER:
		DBOS_Intprintf("DBOS_MUTEX_ERROR_HAS_NO_OWNER");
		break;
	case DBOS_MUTEX_ERROR_IS_NOT_LAST_MUTEX_ACQUIRED:
		DBOS_Intprintf("DBOS_MUTEX_ERROR_IS_NOT_LAST_MUTEX_ACQUIRED");
		break;
	case DBOS_OBJECT_NOT_AVAILABLE:
		DBOS_Intprintf("DBOS_OBJECT_NOT_AVAILABLE");
		break;
	case DBOS_OBJECT_SIGNALLED_SIGNALONETASK:
		DBOS_Intprintf("DBOS_OBJECT_SIGNALLED_SIGNALONETASK");
		break;
	case DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS:
		DBOS_Intprintf("DBOS_OBJECT_SIGNALLED_SIGNALALLTASKS");
		break;
	case DBOS_OBJECT_SIGNAL_NO_TASKS:
		DBOS_Intprintf("DBOS_OBJECT_SIGNAL_NO_TASKS");
		break;
	case DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH:
		DBOS_Intprintf("DBOS_OBJECT_SIGNALLED_NO_TASK_SWITCH");
		break;
	case DBOS_OBJECT_SIGNALLED_TASK_SWITCHED:
		DBOS_Intprintf("DBOS_OBJECT_SIGNALLED_TASK_SWITCHED");
		break;
	case DBOS_OBJECT_SIGNALLED_TASK_SWITCH_REQUIRED:
		DBOS_Intprintf("DBOS_OBJECT_SIGNALLED_TASK_SWITCH_REQUIRED");
		break;
	case DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING:
		DBOS_Intprintf("DBOS_OBJECT_SIGNALLED_NO_TASKS_WAITING");
		break;
	case DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET:
		DBOS_Intprintf("DBOS_OBJECT_SIGNAL_CRITERIA_NOT_MET");
		break;
	case DBOS_OBJECT_WAIT_NO_WAIT:
		DBOS_Intprintf("DBOS_OBJECT_WAIT_NO_WAIT");
		break;
	case DBOS_OBJECT_WAIT_WAITED:
		DBOS_Intprintf("DBOS_OBJECT_WAIT_WAITED");
		break;
	case DBOS_OBJECT_WAIT_WAITED_TIMEOUT:
		DBOS_Intprintf("DBOS_OBJECT_WAIT_WAITED_TIMEOUT");
		break;
	case DBOS_OBJECT_WAIT_TIMEOUT:
		DBOS_Intprintf("DBOS_OBJECT_WAIT_TIMEOUT");
		break;
	case DBOS_Q_WRITE_FAIL_FULL_TIMEOUT:
		DBOS_Intprintf("DBOS_Q_WRITE_FAIL_FULL_TIMEOUT");
		break;
	case DBOS_Q_WRITE_FAIL_FULL:
		DBOS_Intprintf("DBOS_Q_WRITE_FAIL_FULL");
		break;
	case DBOS_Q_WRITTEN:
		DBOS_Intprintf("DBOS_Q_WRITTEN");
		break;
	case DBOS_Q_WRITTEN_WAITED:
		DBOS_Intprintf("DBOS_Q_WRITTEN_WAITED");
		break;
	case DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN:
		DBOS_Intprintf("DBOS_Q_WRITE_MULTIPLE_ALL_WRITTEN");
		break;
	case DBOS_Q_WRITE_MULTIPLE_NONE_WRITTEN:
		DBOS_Intprintf("DBOS_Q_WRITE_MULTIPLE_NONE_WRITTEN");
		break;
	case DBOS_Q_WRITE_MULTIPLE_SOME_WRITTEN:
		DBOS_Intprintf("DBOS_Q_WRITE_MULTIPLE_SOME_WRITTEN");
		break;
	case DBOS_Q_READ_FAIL_EMPTY_TIMEOUT:
		DBOS_Intprintf("DBOS_Q_READ_FAIL_EMPTY_TIMEOUT");
		break;
	case DBOS_Q_READ_FAIL_EMPTY:
		DBOS_Intprintf("DBOS_Q_READ_FAIL_EMPTY");
		break;
	case DBOS_Q_READ:
		DBOS_Intprintf("DBOS_Q_READ");
		break;
	case DBOS_Q_READ_WAITED:
		DBOS_Intprintf("DBOS_Q_READ_WAITED");
		break;
	case DBOS_SYNC_OBJECT_NO_WAIT_REQD:
		DBOS_Intprintf("DBOS_SYNC_OBJECT_NO_WAIT_REQD");
		break;
	case DBOS_SYNC_OBJECT_WAIT_REQUIRED:
		DBOS_Intprintf("DBOS_SYNC_OBJECT_WAIT_REQUIRED");
		break;
	case DBOS_SYNC_OBJECT_INVALID_OBJECT_TYPE:
		DBOS_Intprintf("DBOS_SYNC_OBJECT_INVALID_OBJECT_TYPE");
		break;
	case DBOS_ERROR_INVALID_HANDLE:
		DBOS_Intprintf("DBOS_ERROR_INVALID_HANDLE");
		break;
	case DBOS_ERROR_INVALID_ARGUMENT:
		DBOS_Intprintf("DBOS_ERROR_INVALID_ARGUMENT");
		break;
	case DBOS_NOT_A_PRIORITY_BASED_WAITLIST:
		DBOS_Intprintf("DBOS_NOT_A_PRIORITY_BASED_WAITLIST");
		break;
	case DBOS_OUT_OF_MEMORY:
		DBOS_Intprintf("DBOS_OUT_OF_MEMORY");
		break;
	case DBOS_MAX_NUM_OF_SYNCOBJECTS_REACHED:
		DBOS_Intprintf("DBOS_MAX_NUM_OF_SYNCOBJECTS_REACHED");
		break;
	case DBOS_INTERNAL_ERROR:
		DBOS_Intprintf("DBOS_INTERNAL_ERROR");
		break;
	default:
		DBOS_Intprintf("UNKNOWN");
		break;
	}

}

/**
 * \brief printf function to be called from within OSCS or from within ISR.
 *
 */
void DBOS_Intprintf(CHAR const * const S, ...){

	va_list args;

	va_start(args, S);

	DBOS_DebugPrint_printf(S, args);

	return;
}

void DBOS_DebugPrint_printf(CHAR const * const S, va_list vl){

	UNATIVECPU_t i, j, k, Wp;

	UNATIVECPU_t WidthState, Preceding;

	BOOL PrecedingZeros, Negative, Hash;

	INT32S VS;
	INT32U VU;

	const CHAR * St;

	void *pVoid;

	CHAR Sp[MAX_BCD_LENGTH];

	CHAR String[PRINT_BUFFER_LENGTH];

	Wp = 0;

	i = 0;
	while(S[i] != 0){

		if(S[i] == '%'){

			if(Wp != 0){
				ProjUARTs_DebugUARTTxMutipleBytes(String, Wp);
				Wp = 0;
			}

			i++;

			if(S[i] == '#'){
				i++;
				Hash = TRUE;
			}
			else{
				Hash = FALSE;
			}

			if(S[i] == '0'){
				i++;
				PrecedingZeros = TRUE;
			}
			else{
				PrecedingZeros = FALSE;
			}

			if((S[i] >= '0') && (S[i] <= '9')){
				Preceding = (UNATIVECPU_t) (S[i] - '0');
				i++;

				while((S[i] >= '0') && (S[i] <= '9')){
					Preceding = Preceding * 10;
					Preceding += (UNATIVECPU_t) (S[i] - '0');
					i++;
				}
			}
			else{
				Preceding = 0;
			}

			if(S[i] == 'l'){
				i++;
				WidthState = WIDTHSTATELONG;
				if(S[i] == 'l'){
					i++;
					WidthState = WIDTHSTATELONGLONG;
				}
			}
			else if(S[i] == 'h'){
				i++;
				WidthState = WIDTHSTATESHORT;
				if(S[i] == 'h'){
					i++;
					WidthState = WIDTHSTATECHAR;
				}
			}
			else{
				WidthState = WIDTHSTATEINT;
			}

			switch(S[i]){
			case 0:

				break;
			case 'u':		// unsigned int

				switch(WidthState){		// All of these widths are just cast to 32 bit unsigned.
				case WIDTHSTATESHORT:
					VU = (INT32U) va_arg(vl, int);
					break;
				case WIDTHSTATECHAR:
					VU = (INT32U) va_arg(vl, int);
					break;
				case WIDTHSTATELONGLONG:
					VU = (INT32U) va_arg(vl, unsigned long long);
					break;
				case WIDTHSTATELONG:
					VU = (INT32U) va_arg(vl, unsigned long);
					break;
				case WIDTHSTATEINT:
				default:
					VU = (INT32U) va_arg(vl, unsigned int);
					break;
				}

				S_DBOS_DebugPrint_INT32ToBCD((INT32U) VU, Sp);

				j = 0;
				while((Sp[j] == 0) && (j < MAX_BCD_LENGTH - 1)){
					j++;
				}

				k = j;
				while(k < MAX_BCD_LENGTH){
					Sp[k] = (CHAR) (Sp[k] + 48u);
					k++;
				}

				while(Preceding > (MAX_BCD_LENGTH - j)){
					if(PrecedingZeros == TRUE){
						ProjUARTs_DebugUARTTxSingleByte('0');
					}
					else{
						ProjUARTs_DebugUARTTxSingleByte(' ');
					}
					Preceding--;
				}

				ProjUARTs_DebugUARTTxMutipleBytes(&Sp[j], MAX_BCD_LENGTH - j);

				break;
			case 'd':		// signed int
			case 'i':

				switch(WidthState){		// All of these widths are just cast to 32 bit signed.
				case WIDTHSTATESHORT:
					VS = (INT32S) va_arg(vl, int);
					break;
				case WIDTHSTATECHAR:
					VS = (INT32S) va_arg(vl, int);
					break;
				case WIDTHSTATELONGLONG:
					VS = (INT32S) va_arg(vl, signed long long);
					break;
				case WIDTHSTATELONG:
					VS = (INT32S) va_arg(vl, signed long);
					break;
				case WIDTHSTATEINT:
				default:
					VS = (INT32S) va_arg(vl, signed int);
					break;
				}

				if(VS < 0){

					Negative = TRUE;

					VS = ~VS;
					VS += 1;
					if(Preceding > 0){
						Preceding--;
					}
				}
				else{
					Negative = FALSE;
				}

				S_DBOS_DebugPrint_INT32ToBCD((INT32U) VS, Sp);

				j = 0;
				while((Sp[j] == 0) && (j < MAX_BCD_LENGTH - 1)){
					j++;
				}

				k = j;
				while(k < MAX_BCD_LENGTH){
					Sp[k] = (CHAR) (Sp[k] + 48u);
					k++;
				}

				while(Preceding > (MAX_BCD_LENGTH - j)){
					if(PrecedingZeros == TRUE){
						ProjUARTs_DebugUARTTxSingleByte('0');
					}
					else{
						ProjUARTs_DebugUARTTxSingleByte(' ');
					}
					Preceding--;
				}

				if(Negative){
					ProjUARTs_DebugUARTTxSingleByte('-');
				}

				ProjUARTs_DebugUARTTxMutipleBytes(&Sp[j], MAX_BCD_LENGTH - j);

				break;
			case 'x':		// Hex

				VU = (INT32U) va_arg(vl, unsigned int);

				if(Hash == TRUE){
					ProjUARTs_DebugUARTTxSingleByte('0');
					ProjUARTs_DebugUARTTxSingleByte('x');
				}

				S_DBOS_DebugPrintINT32UHex(VU, TRUE);

				break;
			case 'X':		// Hex

				VU = (INT32U) va_arg(vl, unsigned int);

				if(Hash == TRUE){
					ProjUARTs_DebugUARTTxSingleByte('0');
					ProjUARTs_DebugUARTTxSingleByte('x');
				}

				S_DBOS_DebugPrintINT32UHex(VU, FALSE);

				break;
			case 's':		// String

				St = (const CHAR *) va_arg(vl, CHAR *);

				j = 0;
				while(St[j] != 0){
					j++;
				}

				while(Preceding > j){
					if(PrecedingZeros == TRUE){
						ProjUARTs_DebugUARTTxSingleByte('0');
					}
					else{
						ProjUARTs_DebugUARTTxSingleByte(' ');
					}
					Preceding--;
				}

				ProjUARTs_DebugUARTTxMutipleBytes(St, j);

				break;
			case 'c':		// Character

				ProjUARTs_DebugUARTTxSingleByte((char) va_arg(vl, int));

				break;
			case 'p':		// pointer to void

				pVoid = (void *) va_arg(vl, void *);

				ProjUARTs_DebugUARTTxSingleByte('0');
				ProjUARTs_DebugUARTTxSingleByte('x');

				S_DBOS_DebugPrintINT32UHex((INT32U) pVoid, FALSE);

				break;
			case '%':		// literal %
				ProjUARTs_DebugUARTTxSingleByte('%');

				break;
			default:

				break;
			}
		}
		else{
			String[Wp] = S[i];
			Wp++;
			if(Wp >= PRINT_BUFFER_LENGTH){
				ProjUARTs_DebugUARTTxMutipleBytes(String, Wp);
				Wp = 0;
			}
		}

		i++;
	}

	va_end(vl);

	if(Wp != 0){
		ProjUARTs_DebugUARTTxMutipleBytes(String, Wp);
		Wp = 0;
	}

}
// -------------------------------------------------------------------------------------------------
// PROTECTED FUNCTIONS

// -------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
static void S_DBOS_DebugPrintINT32UHex(INT32U V, BOOL LowerCase){	// Print UnSigned 32 bit in hex format.

	INT8U X;
	UNATIVECPU_t i;

	for(i = 0; i < 8 ; i++){
		X = (INT8U) (V >> 28);

		if(X > 9){
			if(LowerCase == TRUE){
				X = (INT8U) (X + 55u + 32u);
			}
			else{
				X = (INT8U) (X + 55u);
			}
		}
		else{
			X = (INT8U) (X + 48u);
		}

		ProjUARTs_DebugUARTTxSingleByte(X);

		V <<= 4;
	}

	return;
}

static void S_DBOS_DebugPrint_INT32ToBCD(INT32U V, CHAR *S){

	// S should be a pointer to character array with 10 characters.

	UNATIVECPU_t i;
	UNATIVECPU_t j;

	for(j = 0; j < MAX_BCD_LENGTH ; j++){
		S[j] = 0;
	}

	for(i = 32; i > 0 ; i--){		// Loop through all the bits.

		for(j = 0; j < 9 ; j++){	// Shift Left.
			S[j] = (CHAR) (S[j] << 1);
			S[j] &= 0b00001111;
			if(S[j + 1] & 0b00001000){
				S[j] |= 0b00000001;
			}
		}

		S[9] = (CHAR) (S[9] << 1);
		S[9] &= 0b00001111;

		if(V & 0b10000000000000000000000000000000){
			S[9] |= 0b00000001;
		}

		V <<= 1;
		// All Shifted Left 1.
		if(i != 1){
			for(j = 0; j < MAX_BCD_LENGTH ; j++){
				if(S[j] >= 5){
					S[j] = (CHAR) (S[j] + 3);
				}
				S[j] &= 0b00001111;
			}
		}
	}

	return;
}

// -------------------------------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINES

}// namespace
