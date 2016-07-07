/*
 * BitBand.h
 *
 *  Created on: 4/10/2009
 *      Author: David Broadhurst
 */

#ifndef BITBAND_H_
#define BITBAND_H_

#include "Device.h"

#ifdef __cplusplus
extern "C" {
#endif

// - Note 923: cast from unsigned long to pointer [MISRA C++ Rule 5-2-7], [MISRA C++ Rule 5-2-8]
// - Note 1924: C-style cast -- More Effective C++ #2, [MISRA C++ Rule 5-2-4]
// - Note 923: cast from pointer to unsigned long [MISRA C++ Rule 5-2-7], [MISRA C++ Rule 5-2-8]

//lint -emacro((923), Var_ResetBit_BB)
//lint -emacro((923), Var_SetBit_BB)
//lint -emacro((923), Var_GetBit_BB)
//lint -emacro((1924), Var_ResetBit_BB)
//lint -emacro((1924), Var_SetBit_BB)
//lint -emacro((1924), Var_GetBit_BB)

//lint -emacro((923), Per_ResetBit_BB)
//lint -emacro((923), Per_SetBit_BB)
//lint -emacro((923), Per_GetBit_BB)
//lint -emacro((1924), Per_ResetBit_BB)
//lint -emacro((1924), Per_SetBit_BB)
//lint -emacro((1924), Per_GetBit_BB)

#define  Var_ResetBit_BB(VarAddr, BitNumber)    \
          (*(__IO uint32_t *) (SRAM_BB_BASE | (( ((uint32_t)(VarAddr)) - SRAM_BASE) << 5U) | ( ((uint32_t)(BitNumber)) << 2U)) = 0U)

#define Var_SetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (SRAM_BB_BASE | (( ((uint32_t)(VarAddr)) - SRAM_BASE) << 5U) | ( ((uint32_t)(BitNumber)) << 2U)) = 1U)

#define Var_GetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (SRAM_BB_BASE | (( ((uint32_t)(VarAddr)) - SRAM_BASE) << 5U) | ( ((uint32_t)(BitNumber)) << 2U)))

#define  Per_ResetBit_BB(VarAddr, BitNumber)    \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | (( ((uint32_t)(VarAddr)) - PERIPH_BASE) << 5U) | ( ((uint32_t)(BitNumber)) << 2U)) = 0U)

#define Per_SetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | (( ((uint32_t)(VarAddr)) - PERIPH_BASE) << 5U) | ( ((uint32_t)(BitNumber)) << 2U)) = 1U)

#define Per_GetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | (( ((uint32_t)(VarAddr)) - PERIPH_BASE) << 5U) | ( ((uint32_t)(BitNumber)) << 2U)))

#ifdef __cplusplus
}
#endif

#endif /* BITBAND_H_ */
