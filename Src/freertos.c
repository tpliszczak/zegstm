/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     

#include "gpio.h"
#include "usb_device.h"
#include "usart.h"
#include "string.h"
#include "tim.h"
#include "rtc.h"


/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */

osThreadId blueTaskHandle;
osSemaphoreId blueBinarySemHandle;

osThreadId taskLed7SegHandle;
uint8_t cyfra_1, cyfra_2, cyfra_3, cyfra_4, cyfra_5, cyfra_6, cyfra_7, cyfra_8, cyfra_9, cyfra_10, cyfra_11, cyfra_12;
uint8_t kropka_1, kropka_2, kropka_3, kropka_4, kropka_5, kropka_6, kropka_7, kropka_8, kropka_9, kropka_10, kropka_11, kropka_12;

RTC_TimeTypeDef getTime;
RTC_DateTypeDef getDate;
uint8_t blueRxFrame[256], blueTxFrame[256], Rx_data[256], txDone, Rx_indx;

uint32_t sciemniacz = 0;  //80noc 50 srednio 0 reszta

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

void taskLed7Seg(void const * argument);
void taskBlue(void const * argument);


/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void ){

	if(blueRxFrame[0] == 0){
		flagaKoncaRxt = 0;
		Rx_indx = 0;
		__HAL_TIM_SET_COUNTER(&htim3, 0); //reset timera 1,5T
	}else if (flagaKoncaRxt == 1){
		flagaKoncaRxt = 0;
		osSemaphoreRelease(blueBinarySemHandle);
	}

}
/* USER CODE END 2 */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	HAL_UART_Receive_DMA(&huart1, Rx_data, 1);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */

	  osSemaphoreDef(blueBinarySem);
	  blueBinarySemHandle = osSemaphoreCreate(osSemaphore(blueBinarySem), 1);



  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  osThreadDef(TaskLed7Seg, taskLed7Seg, osPriorityHigh, 0, 128);
  taskLed7SegHandle = osThreadCreate(osThread(TaskLed7Seg), NULL);

  osThreadDef(Taskblue, taskBlue, osPriorityNormal, 0, 128);
  blueTaskHandle = osThreadCreate(osThread(Taskblue), NULL);



  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	  uint8_t c[2] = {55,10};
	//  HAL_UART_Transmit_DMA(&huart1, c , 2 );  //DMA

   // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */

void taskLed7Seg(void const * argument)
{
	static uint8_t sekundy, minuty, godziny;
	static uint32_t milisec;

	uint32_t PreviousWakeTime = osKernelSysTick();

	for(;;)
	{


		if (HAL_RTC_GetTime(&hrtc, & getTime, RTC_FORMAT_BIN) != HAL_OK)
		  {
		    Error_Handler();
		  }
		if (HAL_RTC_GetDate(&hrtc, & getDate, RTC_FORMAT_BIN) != HAL_OK)
		  {
		    Error_Handler();
		  }


		milisec++;
		if (milisec>999) {
			milisec = 0;
			sekundy++;
		}
		if (sekundy>59) {
			sekundy = 0;
			minuty++;
		}
		if (minuty>59) {
				minuty = 0;
				godziny++;
		}
		if (godziny > 24){
			godziny = 0;
		}

		godziny = getTime.Hours;
		minuty = getTime.Minutes;
		sekundy = getTime.Seconds;


		cyfra_1 = godziny/10;
		if (cyfra_1 == 0) cyfra_1 = 10;
		cyfra_2 = godziny%10;
		cyfra_3 = minuty/10;
		cyfra_4 = minuty%10;





		cyfra_5 = sekundy/10;
		cyfra_6 = sekundy%10;
		kropka_6 = 1;

		if (++sciemniacz>99) {
			sciemniacz = 0;
		}

		cyfra_7 = sciemniacz/10 ;//(milisec%100) / 10;
		cyfra_8 = sciemniacz%10;





		cyfra_9 = getDate.Month/10;
		cyfra_10 = getDate.Month%10;
		kropka_10 = 1;
		cyfra_11 = getDate.Date/10;
		cyfra_12 = getDate.Date%10;

		if (sekundy%2) {
			kropka_4 = 1;
		}
		else{
			kropka_4 = 0;
		}
	    osDelayUntil(&PreviousWakeTime, 250);
	}
}


void taskBlue(void const * argument){

	  for(;;) {
		  osSemaphoreWait(blueBinarySemHandle, osWaitForever);
		 // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);


		  uint8_t buffTmp[256], len;
		  len = Rx_indx;
		  Rx_indx = 0;
		  uint8_t i = 0;
		  for ( i = 0;  i < 255; ++ i) {
			  buffTmp[i] = blueRxFrame[i];
			  blueRxFrame[i] = 0;
		  }

		  if(txDone == 0){
			 // txDone = 1;
			  HAL_UART_Transmit_DMA(&huart1, buffTmp , len );
		  }


		  //osDelay(100);

	  }
}




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if (huart->Instance == USART1) {

		__HAL_TIM_SET_COUNTER(&htim3, 0); //reset timera 3
		HAL_UART_DMAPause(huart);
		__HAL_UART_FLUSH_DRREGISTER(huart);

		blueRxFrame[Rx_indx++] = Rx_data[0];

		if ( Rx_indx > 255 ){
			Rx_indx = 0;
		}

		HAL_UART_DMAResume(huart);
	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	txDone = 0;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	txDone = 0;

	  __HAL_UART_CLEAR_OREFLAG(huart);
	  __HAL_UART_CLEAR_NEFLAG(huart);
	  __HAL_UART_CLEAR_FEFLAG(huart);
	  __HAL_UART_CLEAR_PEFLAG(huart);

	 if(huart->Instance == USART1)
	  {
		 __HAL_UART_FLUSH_DRREGISTER(&huart1);
		 HAL_UART_Receive_DMA(&huart1, Rx_data, 1);
	  }
}





/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
