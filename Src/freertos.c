/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include <string.h>
#include "oneWire.h"

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

RTC_TimeTypeDef setTime;
RTC_DateTypeDef setDate;

uint8_t blueRxFrame[256], blueTxFrame[256], Rx_data[256], Rx_indx;
volatile uint8_t txDone = 0;
 uint8_t sekundy, minuty, godziny;

uint32_t sciemniacz = 0;  //80noc 50 srednio 0 reszta
uint8_t lenth;
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
  osThreadDef(TaskLed7Seg, taskLed7Seg, osPriorityNormal, 0, 128);
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

//		if (++sciemniacz>99) {
//			sciemniacz = 0;
//		}

//		cyfra_7 = sciemniacz/10 ;//(milisec%100) / 10;
//		cyfra_8 = sciemniacz%10;

cyfra_7 = getDate.Year / 10;
cyfra_8 = getDate.Year %10;



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

		  uint8_t buffTmp[256], len;
		  len = Rx_indx;
		  Rx_indx = 0;
		  uint8_t i = 0;
		  for ( i = 0;  i < 255; ++ i) {
			  buffTmp[i] = blueRxFrame[i];
			  blueRxFrame[i] = 0;
		  }
		  for ( i = 0;  i < 255; ++ i) {
			  blueTxFrame[i] = 0;
		  }


		  if( (buffTmp[0]=='a') && (buffTmp[1]=='t') && (buffTmp[2]=='+')   ){

			  //komenda at+

			  //odczyt t
			  if( (buffTmp[3]=='t') && (buffTmp[4]!='=')  ){
				  uint8_t tmp2[10], len;

				  if(txDone == 0){
					  strcpy( blueTxFrame, "Czas w zegarze ISO 8601: " );

					  if(getDate.Year < 10){
						  strcat( blueTxFrame, "200" );
					  }else{
						  strcat( blueTxFrame, "20" );
					  }
					  itoa(getDate.Year ,tmp2,10);
					  strcat( blueTxFrame, tmp2 );

					  if(getDate.Month < 10){
						  strcat( blueTxFrame, "-0" );
					  }else{
						  strcat( blueTxFrame, "-" );
					  }
					  itoa(getDate.Month ,tmp2,10);
					  strcat( blueTxFrame, tmp2 );

					  if(getDate.Date < 10){
						  strcat( blueTxFrame, "-0" );
					  }else{
						  strcat( blueTxFrame, "-" );
					  }
					  itoa(getDate.Date ,tmp2,10);
					  strcat( blueTxFrame, tmp2 );

					  if(getTime.Hours < 10){
						  strcat( blueTxFrame, "T0" );
					  }else{
						  strcat( blueTxFrame, "T" );
					  }
					  itoa(godziny,tmp2,10);
					  strcat( blueTxFrame, tmp2 );

					  if(getTime.Minutes < 10){
						  strcat( blueTxFrame, ":0" );
					  }else{
						  strcat( blueTxFrame, ":" );
					  }
					  itoa(minuty,tmp2,10);
					  strcat( blueTxFrame, tmp2 );

					  if(getTime.Seconds < 10){
						  strcat( blueTxFrame, ":0" );
					  }else{
						  strcat( blueTxFrame, ":" );
					  }
					  itoa(sekundy,tmp2,10);
					  strcat( blueTxFrame, tmp2 );

					  strcat( blueTxFrame, "+01:00\n" );

					  lenth = strlen(blueTxFrame) ;
					  if (lenth >0) {
						  txDone = 1;
						  HAL_UART_Transmit_DMA(&huart1, blueTxFrame , lenth);
					  }
				  }
			  }

			  if( (buffTmp[3]=='t') && (buffTmp[4]== '=')  ){

				  if( (buffTmp[7]==':') && (buffTmp[10]== ':') ){

				  uint8_t tmpBuff[10];

				  tmpBuff[0] = buffTmp[5];
				  tmpBuff[1] = buffTmp[6];
				  setTime.Hours =  atoi(tmpBuff);

				  tmpBuff[0] = buffTmp[8];
				  tmpBuff[1] = buffTmp[9];
				  setTime.Minutes =  atoi(tmpBuff);

				  tmpBuff[0] = buffTmp[11];
				  tmpBuff[1] = buffTmp[12];
				  setTime.Seconds =  atoi(tmpBuff);

				  if (HAL_RTC_SetTime(&hrtc, &setTime, RTC_FORMAT_BIN) != HAL_OK)
				  {
				    Error_Handler();
				  }

				  txDone = 1;
				  char error[] = "OK\n";
				  HAL_UART_Transmit_DMA(&huart1, error , strlen(error) );

				  }else{

					  if(txDone == 0){
						  txDone = 1;
						  char error[] = "Podaj poprawne dane np. \"at+t=22:01:55\"\n";
						  HAL_UART_Transmit_DMA(&huart1, error , strlen(error) );
					  }

				  }
			  }
			  if( (buffTmp[3]=='d') && (buffTmp[4]== '=')  ){

				  if( (buffTmp[9]=='-') && (buffTmp[12]== '-') ){

				  uint8_t tmpBuff[10];

				  tmpBuff[0] = buffTmp[7];
				  tmpBuff[1] = buffTmp[8];
				  setDate.Year =  atoi(tmpBuff);

				  tmpBuff[0] = buffTmp[10];
				  tmpBuff[1] = buffTmp[11];
				  setDate.Month =  atoi(tmpBuff);

				  tmpBuff[0] = buffTmp[13];
				  tmpBuff[1] = buffTmp[14];
				  setDate.Date =  atoi(tmpBuff);

				  if (HAL_RTC_SetDate(&hrtc, &setDate, RTC_FORMAT_BIN) != HAL_OK)
				  {
				    Error_Handler();
				  }

				  txDone = 1;
				  char error[] = "OK\n";
				  HAL_UART_Transmit_DMA(&huart1, error , strlen(error) );

				  }else{

					  if(txDone == 0){
						  txDone = 1;
						  char error[] = "Podaj poprawne dane np. \"at+d=2016-12-31\"\n";
						  HAL_UART_Transmit_DMA(&huart1, error , strlen(error) );
					  }

				  }
			  }

			  if( (buffTmp[3]=='s') && (buffTmp[4]== '=')  ){


				  uint8_t tmpBuff[10];
				  tmpBuff[0] = buffTmp[5];
				  tmpBuff[1] = buffTmp[6];
			   	sciemniacz =  atoi(tmpBuff);

				txDone = 1;
				char error[] = "OK\n";
				HAL_UART_Transmit_DMA(&huart1, error , strlen(error) );
			  }


		  }else{
			  if(txDone == 0){
				  txDone = 1;
				  char error[] = "Podaj poprawne dane np. \"at+t\",\"at+t=22:01:55\", \"at+d=2016-12-30\" \n";
				  HAL_UART_Transmit_DMA(&huart1, error , strlen(error) );
			  }

		  }









		  //osDelay(100);

	  }
}




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if (huart->Instance == USART1) {
		txDone = 0;

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
