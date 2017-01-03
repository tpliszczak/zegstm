/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "dma.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

#include "FreeRTOS.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

volatile uint32_t flagaDelay;


uint16_t maskaAnod = A1_Pin | A2_Pin | A3_Pin | A4_Pin | A5_Pin | A6_Pin| A7_Pin | A8_Pin | A9_Pin | A10_Pin | A11_Pin | A12_Pin;
uint16_t maskaSeg = A_Pin | B_Pin | C_Pin | D_Pin | E_Pin | F_Pin | G_Pin| DP_Pin;
extern uint32_t sciemniacz;
volatile  uint8_t idx = 1;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
void MX_FREERTOS_Init(void);

/* USER CODE BEGIN PFP */

void display7Seg(void);


/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */



  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();

  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim4);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USB;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* USER CODE BEGIN 4 */

//wyswietlacz
void display7Seg(void){

	//*********************************zmienne*******
	extern uint8_t cyfra_1, cyfra_2, cyfra_3, cyfra_4, cyfra_5, cyfra_6, cyfra_7, cyfra_8, cyfra_9, cyfra_10, cyfra_11, cyfra_12;
	extern uint8_t kropka_1, kropka_2, kropka_3, kropka_4, kropka_5, kropka_6, kropka_7, kropka_8, kropka_9, kropka_10, kropka_11, kropka_12;
	//wybieranie anody
	static uint8_t idxDodac=0, idxMain = 1, idxWgrac = 1;


	// definicje bitów dla poszczególnych segmentów LED
	#define SEG_A A_Pin
	#define SEG_B B_Pin
	#define SEG_C C_Pin
	#define SEG_D D_Pin
	#define SEG_E E_Pin
	#define SEG_F F_Pin
	#define SEG_G G_Pin
	#define SEG_DP DP_Pin
	#define NIC 10
	#define MIN 11
	#define STOP 12
	#define NAP 13

	// definicja tablicy zawieraj±cej definicje bitowe cyfr LED
	const uint16_t cyfry[14] = {
			(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),			// 0
			(SEG_B | SEG_C),						// 1
			(SEG_A | SEG_B | SEG_D | SEG_E | SEG_G),			// 2
			(SEG_A | SEG_B | SEG_C | SEG_D | SEG_G),			// 3
			(SEG_B | SEG_C | SEG_F | SEG_G),				// 4
			(SEG_A | SEG_C | SEG_D | SEG_F | SEG_G),			// 5
			(SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),			// 6
			(SEG_A | SEG_B | SEG_C),					// 7
			(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),        // 8
			(SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G),			// 9
			0,							// NIC (puste miejsce)
			SEG_G,                                                  // MIN
			(SEG_A | SEG_B | SEG_F | SEG_G),				//STOP
			(SEG_E | SEG_D | SEG_C)					//u
	};

	//***********************************************

//	cyfra_1 = NIC;
//	cyfra_2 = 2;
//	cyfra_3 = NIC;
//	cyfra_4 = 4;
//	cyfra_5 = 8;
//	cyfra_6 = 8;
//	cyfra_7 = 8;
//	cyfra_8 = 8;
//	cyfra_9 = 8;
//	cyfra_10 = 8;
//	cyfra_11 = 8;
//	cyfra_12 = 8;


	HAL_GPIO_WritePin(A_GPIO_Port, maskaSeg, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(A1_GPIO_Port, maskaAnod, GPIO_PIN_RESET);

		switch (idx) {
			case 1:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_1], GPIO_PIN_SET);
				if (kropka_1 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_SET);
				break;
			case 2:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_2], GPIO_PIN_SET);
				if (kropka_2 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_SET);
				break;
			case 3:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_3], GPIO_PIN_SET);
				if (kropka_3 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);
				break;
			case 4:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_4], GPIO_PIN_SET);
				if (kropka_4 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A4_GPIO_Port, A4_Pin, GPIO_PIN_SET);
				break;
			case 5:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_5], GPIO_PIN_SET);
				if (kropka_5 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A5_GPIO_Port, A5_Pin, GPIO_PIN_SET);
				break;
			case 6:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_6], GPIO_PIN_SET);
				if (kropka_6 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A6_GPIO_Port, A6_Pin, GPIO_PIN_SET);
				break;
			case 7:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_7], GPIO_PIN_SET);
				if (kropka_7 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A7_GPIO_Port, A7_Pin, GPIO_PIN_SET);
				break;
			case 8:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_8], GPIO_PIN_SET);
				if (kropka_8 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A8_GPIO_Port, A8_Pin, GPIO_PIN_SET);
				break;
			case 9:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_9], GPIO_PIN_SET);
				if (kropka_9 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A9_GPIO_Port, A9_Pin, GPIO_PIN_SET);
				break;
			case 10:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_10], GPIO_PIN_SET);
				if (kropka_10 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A10_GPIO_Port, A10_Pin, GPIO_PIN_SET);
				break;
			case 11:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_11], GPIO_PIN_SET);
				if (kropka_11 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A11_GPIO_Port, A11_Pin, GPIO_PIN_SET);
				break;
			case 12:
				HAL_GPIO_WritePin(A_GPIO_Port, cyfry[cyfra_12], GPIO_PIN_SET);
				if (kropka_12 == 1) {
					HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, GPIO_PIN_SET);
				}
				HAL_GPIO_WritePin(A12_GPIO_Port, A12_Pin, GPIO_PIN_SET);
				break;
		}


	if (idxMain == 5){
		idxWgrac = idxMain + idxDodac;
		++idxDodac;
		if (idxDodac>7) {
			idxDodac = 0;
		}
	}
	else{
		idxWgrac = idxMain;
	}

	++idxMain;  //index 1 do 5
	if (idxMain >5){
		idxMain = 1;
	}

	idx = idxWgrac;

}


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
/* USER CODE BEGIN Callback 0 */
/* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
/* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM3) {
	  flagaKoncaRxt = 1;
//	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

      if (flagaDelay >0) {
    	  --flagaDelay;
      }

  }

  if (htim->Instance == TIM4) {
	  __HAL_TIM_SET_COUNTER(&htim2, 0);
	   uint32_t sciemTmp;
	   if(idx <5 ){
		   sciemTmp = sciemniacz* 4;
	   }else{
		   sciemTmp = 0;
	   }

	   display7Seg();

	   __HAL_TIM_SET_COUNTER(&htim2, sciemTmp);
	   __HAL_TIM_SET_COUNTER(&htim4, 0);
  }

  if (htim->Instance == TIM2) {
	  HAL_GPIO_WritePin(A1_GPIO_Port, maskaAnod, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(A_GPIO_Port, maskaSeg, GPIO_PIN_RESET);
  }

/* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
	  break;
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
