/**
 * @file main.cpp
 * @author David Broadhurst
 * @date 27/05/2015
 *
 * \brief Demonstrates time slicing between 3 tasks of equal priority none of which yield.
 *
 */
#include "Device.h"
#include "ProjUARTs.h"
#include "dBOS_DebugPrint.hpp"
#include "stm32f1xx_hal.h"

#include "dBOS_KernelInterface.hpp"

using namespace dBOS;

void SystemClock_Config(void);

extern "C" {

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void SysTick_Handler(void);

}

static dBOS_TASKINTERFACE_P_t TaskOne;
static dBOS_TASKINTERFACE_P_t TaskTwo;
static dBOS_TASKINTERFACE_P_t TaskThree;

void TimeSliceTest(void);

int main(void){

	SCB->CCR = SCB->CCR | 0x200; /* Set STKALIGN in NVIC */

	SystemClock_Config();

	HAL_Init();

	NVIC_SetPriorityGrouping(0U);

	RCC->APB2RSTR |= RCC_APB2RSTR_IOPCRST;
	RCC->APB2RSTR &= ~((uint32_t) RCC_APB2RSTR_IOPCRST);
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

	RCC->APB2RSTR |= RCC_APB2RSTR_IOPARST;
	RCC->APB2RSTR &= ~((uint32_t) RCC_APB2RSTR_IOPARST);
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	GPIOA->CRL &= ~GPIO_CRL_CNF2;
	GPIOA->CRL |= GPIO_CRL_CNF2 & GPIO_CRL_CNF2_1;	//alternate function push-pull.
	GPIOA->CRL &= ~GPIO_CRL_MODE2;
	GPIOA->CRL |= GPIO_CRL_MODE2_1; //2MHz output mode.

	GPIOA->CRL &= ~GPIO_CRL_CNF3;
	GPIOA->CRL |= GPIO_CRL_CNF3_1;	//Input Pull.
	GPIOA->CRL &= ~GPIO_CRL_MODE3;

	ProjUARTs_InitCOMM2();

	DBOS_printf("\n\n -- Start Up --");

	DBOS_printf("\nSys Clk: %u", HAL_RCC_GetSysClockFreq());
	DBOS_printf("\nHClk: %u", HAL_RCC_GetHCLKFreq());
	DBOS_printf("\nPClk1: %u", HAL_RCC_GetPCLK1Freq());
	DBOS_printf("\nPClk2: %u", HAL_RCC_GetPCLK2Freq());

	TimeSliceTest();

	while(1){

	}

	return 0;
}

static void TimeSliceTest_IdleTask(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	pK->IdleTaskStarted();

	while(1){

	}

}

static void TimeSliceTest_TaskOneFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	while(1){
		DBOS_printf("\nTask 1 %u", (INT32U) pK->GetElapsedTime());

		volatile INT32U i = 10000;
		while(i > 0U){
			i--;
		}
	}

}

static void TimeSliceTest_TaskTwoFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	while(1){
		DBOS_printf("\nTask 2 %u", (INT32U) pK->GetElapsedTime());

		volatile INT32U i = 10000;
		while(i > 0U){
			i--;
		}
	}

}

static void TimeSliceTest_TaskThreeFunc(void * D){

	(void) D; // Suppress unused parameter warning

	dBOS_KERNELINTERFACE * const pK = dBOS_KERNELINTERFACE::GetInstance();

	DBOS_printf("\nTask Started: %s - Time: %u", __func__, (INT32U) pK->GetElapsedTime());

	while(1){
		DBOS_printf("\nTask 3 %u", (INT32U) pK->GetElapsedTime());

		volatile INT32U i = 10000;
		while(i > 0U){
			i--;
		}
	}

}

void TimeSliceTest(void){

	dBOS_KERNELINTERFACE::Init();

	dBOS_KERNELINTERFACE * pK = dBOS_KERNELINTERFACE::GetInstance();

	TaskOne = pK->CreateTask("Task One", 10, 196, TimeSliceTest_TaskOneFunc, NULLPTR);
	TaskTwo = pK->CreateTask("Task Two", 10, 196, TimeSliceTest_TaskTwoFunc, NULLPTR);
	TaskThree = pK->CreateTask("Task Three", 10, 196, TimeSliceTest_TaskThreeFunc, NULLPTR);

	pK->Start("Idle Task", 128, TimeSliceTest_IdleTask);

	while(1){

	}

	return;
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 72000000
 *            HCLK(Hz)                       = 72000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            HSE Frequency(Hz)              = 8000000
 *            HSE PREDIV1                    = 1
 *            PLLMUL                         = 9
 *            Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
void SystemClock_Config(void){
	RCC_ClkInitTypeDef clkinitstruct;
	RCC_OscInitTypeDef oscinitstruct;

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscinitstruct.HSEState = RCC_HSE_ON;
	oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	oscinitstruct.PLL.PLLState = RCC_PLL_ON;
	oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if(HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK){
		/* Initialization Error */
		while(1)
			;
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
	clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
	if(HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2) != HAL_OK){
		/* Initialization Error */
		while(1)
			;
	}
}

extern "C" {
/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void){
	while(1){

	}
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void){
	/* Go to infinite loop when Hard Fault exception occurs */
	while(1){
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void){
	/* Go to infinite loop when Memory Manage exception occurs */
	while(1){
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void){
	/* Go to infinite loop when Bus Fault exception occurs */
	while(1){
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void){
	/* Go to infinite loop when Usage Fault exception occurs */
	while(1){
	}
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void){
	while(1){

	}
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void){
	HAL_IncTick();
}

}
