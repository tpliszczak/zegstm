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
#include "adc.h"
#include "wyswietlacz7Seg.h"
#include "eeprom.h"
#include "crc.h"

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */

uint16_t VirtAddVarTab[NB_OF_VAR] = {0x5555, 0x6666, 0x7777};
uint16_t eepromVarDataTab[NB_OF_VAR] = {0, 0, 0};
uint16_t eeIloscOdpalen;

osThreadId blueTaskHandle;
osThreadId co1sekundeTaskHandle;
osSemaphoreId blueBinarySemHandle;

//wyswietlacz

uint32_t crcbuff[1];
extern wyswietlacz7Seg wyswietlaCyfrySet;

volatile uint8_t cyfra_1, cyfra_2, cyfra_3, cyfra_4, cyfra_5, cyfra_6, cyfra_7, cyfra_8, cyfra_9, cyfra_10, cyfra_11, cyfra_12;
volatile uint8_t kropka_1, kropka_2, kropka_3, kropka_4, kropka_5, kropka_6, kropka_7, kropka_8, kropka_9, kropka_10, kropka_11, kropka_12;


uint8_t rtcTimer1Sec, czujniki_cnt;

volatile uint32_t ADCread, ADCpoziom[3]={900,1500,2300};




uint8_t temp1, temp2, dsBuff[9], temperaturaDs[3];
uint16_t DS_tmp;

osThreadId taskLed7SegHandle;


RTC_TimeTypeDef getTime;
RTC_DateTypeDef getDate;

RTC_TimeTypeDef setTime;
RTC_DateTypeDef setDate;


uint8_t blueRxFrame[256], blueTxFrame[256], Rx_data[256], Rx_indx;
volatile uint8_t txDone = 0;
 uint8_t sekundy, minuty, godziny;

uint32_t sciemniacz = 0;  //80noc 50 srednio 0 reszta

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

void taskLed7Seg(void const * argument);
void taskBlue(void const * argument);
void co1SekundeTask(void const * argument);

uint8_t rtcSetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef* time);
uint8_t rtcGetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef* time);

void displayPrzeliczCzas(void);
void przyciskiRtc(void);

void eepromStart(void);
void eepromWrite(uint16_t adr, uint16_t value);
void eepromRead(uint16_t adr, uint16_t* value);

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

	eepromStart();
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

  osThreadDef(Taskblue, taskBlue, osPriorityNormal, 0, 256);
  blueTaskHandle = osThreadCreate(osThread(Taskblue), NULL);

  osThreadDef(TaskCo1Sekunde, co1SekundeTask, osPriorityNormal, 0, 128);
  co1sekundeTaskHandle = osThreadCreate(osThread(TaskCo1Sekunde), NULL);


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
  for(;;)  {
	  przyciskiRtc();
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */
void co1SekundeTask(void const * argument){
	uint32_t PreviousWakeTime = osKernelSysTick();

	for(;;){
		HAL_ADC_Start_IT(&hadc1); //ADC

		kropka_4^=1;

		if(rtcTimer1Sec) --rtcTimer1Sec;






		osDelayUntil(&PreviousWakeTime, 1000);
	}
}

void taskLed7Seg(void const * argument){
	uint32_t PreviousWakeTime = osKernelSysTick();

	uint32_t  konewrt=0, DSerror=0;
	uint8_t  temperaturaLast=0, licznikErr=0, odczyt = 0;
	uint16_t tempSrednia = 0, tempsredniaTab[5] = {0,0,0,0,0}, indeksSredni=0;

	for(;;){

		//**************************************************************************************** CZAS GET
		rtcGetTime(&hrtc, & getTime);
		if (HAL_RTC_GetDate(&hrtc, & getDate, RTC_FORMAT_BIN) != HAL_OK)
		  {
		    Error_Handler();
		  }
		godziny = getTime.Hours;
		minuty = getTime.Minutes;
		sekundy = getTime.Seconds;
		//**************************************************************************************** END


		if( 0 == (sekundy%3) ) {
			czujniki_cnt = search_sensors();
		}
		if( 1 == (sekundy%3) ){
			DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );
		}
		if( 2 == (sekundy%3) ) {
			if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &temperaturaDs[0], &temperaturaDs[1], &temperaturaDs[2] )){
				odczyt = 1;
			}
		}




//		temperaturaDs[0] = 1;
//		temperaturaDs[1] = 52;
//		temperaturaDs[2] = 1;


		cyfra_5 = sekundy/10;
		cyfra_6 = sekundy%10;
		kropka_6 = 1;

		cyfra_7 = getDate.Year / 10;
		cyfra_8 = getDate.Year % 10;

		cyfra_7 = (DSerror%100) /10;
		cyfra_8 = (DSerror%100) % 10;

		cyfra_7 = (eeIloscOdpalen % 100) / 10;
		cyfra_8 = (eeIloscOdpalen % 100) % 10;


//		if(eeIloscOdpalen > 10){
//			eeIloscOdpalen = 0;
//			eepromWrite(VirtAddVarTab[0], eeIloscOdpalen);
//		}


		if (  (( temperaturaDs[1] > temperaturaLast && (temperaturaDs[1] - temperaturaLast) > 1) || ( temperaturaDs[1] > temperaturaLast && (temperaturaLast - temperaturaDs[1]) > 1) ) && (licznikErr < 5) && odczyt == 1 && konewrt ) {
			odczyt = 0;
			++licznikErr;
			kropka_12 = 1;
			++DSerror;

		} else if (odczyt == 1) {
			konewrt = 1;
			odczyt = 0;
			licznikErr = 0;

			kropka_9 = 0;
			kropka_10 = 0;
			kropka_11 = 0;
			kropka_12 = 0;
			temperaturaLast = temperaturaDs[1];

			if(indeksSredni > 4)indeksSredni = 0;
			tempsredniaTab[indeksSredni++] = temperaturaDs[1] * 10 + temperaturaDs[2];

			tempSrednia = 0;
			uint8_t ii;
			for(ii = 0; ii < 5; ++ii){
				tempSrednia += tempsredniaTab[ii];
			}
			tempSrednia/=5;

			if ((temperaturaDs[0]) && (tempSrednia  != 0)) { //minus
				cyfra_9 = MINUS;
				HAL_GPIO_WritePin(LED_MINUS_GPIO_Port, LED_MINUS_Pin,
						GPIO_PIN_SET);

				//bez znaku stopni
				if (tempSrednia > 99) {
					cyfra_10 = tempSrednia / 100;
					cyfra_11 = (tempSrednia / 10) % 10;
					kropka_11 = 1;
					cyfra_12 = tempSrednia % 10;
				} else {
					cyfra_10 = tempSrednia / 10;
					kropka_10 = 1;
					cyfra_11 = tempSrednia % 10;
					cyfra_12 = 12;
				}
				//PLUS
			} else {
				HAL_GPIO_WritePin(LED_MINUS_GPIO_Port, LED_MINUS_Pin,
						GPIO_PIN_RESET);
				cyfra_9 = tempSrednia / 100;
				if (cyfra_9 == 0) {
					cyfra_9 = 10;
					cyfra_10 = tempSrednia / 10;
				} else {
					cyfra_10 = (tempSrednia / 10) % 10;
				}
				kropka_10 = 1;
				cyfra_11 = tempSrednia % 10;
				cyfra_12 = 12;
			}
		}


		displayPrzeliczCzas();
		osDelayUntil(&PreviousWakeTime, 1000);
	}
}


void taskBlue(void const * argument){

	uint8_t buffTmp[256], lenth=0;
	uint8_t tmp2[10];
	uint8_t tmpBuff[10];
	  for(;;) {

		  osSemaphoreWait(blueBinarySemHandle, osWaitForever);

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

				  if(txDone == 0){
					  strcpy( (char*) blueTxFrame, "Czas w zegarze ISO 8601: " );

					  if(getDate.Year < 10){
						  strcat((char*) blueTxFrame, "200" );
					  }else{
						  strcat( (char*) blueTxFrame, "20" );
					  }
					  itoa(getDate.Year , (char*) tmp2,10);
					  strcat( (char*) blueTxFrame, (char*) tmp2 );

					  if(getDate.Month < 10){
						  strcat( (char*) blueTxFrame, "-0" );
					  }else{
						  strcat((char*) blueTxFrame, "-" );
					  }
					  itoa(getDate.Month , (char*) tmp2,10);
					  strcat( (char*) blueTxFrame, (char*) tmp2 );

					  if(getDate.Date < 10){
						  strcat( (char*) blueTxFrame, "-0" );
					  }else{
						  strcat( (char*) blueTxFrame, "-" );
					  }
					  itoa(getDate.Date , (char*)tmp2,10);
					  strcat( (char*) blueTxFrame, (char*) tmp2 );

					  if(getTime.Hours < 10){
						  strcat( (char*) blueTxFrame, "T0" );
					  }else{
						  strcat( (char*) blueTxFrame, "T" );
					  }
					  itoa(godziny, (char*) tmp2,10);
					  strcat((char*)  blueTxFrame, (char*) tmp2 );

					  if(getTime.Minutes < 10){
						  strcat( (char*) blueTxFrame, ":0" );
					  }else{
						  strcat( (char*) blueTxFrame, ":" );
					  }
					  itoa(minuty, (char*) tmp2,10);
					  strcat( (char*) blueTxFrame, (char*) tmp2 );

					  if(getTime.Seconds < 10){
						  strcat( (char*) blueTxFrame, ":0" );
					  }else{
						  strcat( (char*) blueTxFrame, ":" );
					  }
					  itoa(sekundy, (char*) tmp2,10);
					  strcat( (char*) blueTxFrame, (char*) tmp2 );

					  strcat( (char*)blueTxFrame, "+01:00\n" );

					  lenth = strlen( (char*) blueTxFrame) ;
					  if (lenth >0) {
						  txDone = 1;
						  HAL_UART_Transmit_DMA(&huart1, blueTxFrame , lenth);
					  }
				  }
			  }

			  if( (buffTmp[3]=='t') && (buffTmp[4]== '=')  ){

				  if( (buffTmp[7]==':') && (buffTmp[10]== ':') ){

				  tmpBuff[0] = buffTmp[5];
				  tmpBuff[1] = buffTmp[6];
				  setTime.Hours =  atoi( (char*) tmpBuff);

				  tmpBuff[0] = buffTmp[8];
				  tmpBuff[1] = buffTmp[9];
				  setTime.Minutes =  atoi( (char*) tmpBuff);

				  tmpBuff[0] = buffTmp[11];
				  tmpBuff[1] = buffTmp[12];
				  setTime.Seconds =  atoi( (char*) tmpBuff);

				  if (HAL_RTC_SetTime(&hrtc, &setTime, RTC_FORMAT_BIN) != HAL_OK)
				  {
				    Error_Handler();
				  }

				  txDone = 1;
				  uint8_t error[] = "OK\n";
				  HAL_UART_Transmit_DMA(&huart1, error , strlen((char*) error) );

				  }else{
					  if(txDone == 0){
						  txDone = 1;
						  uint8_t error[] = "Podaj poprawne dane np. \"at+t=22:01:55\"\n";
						  HAL_UART_Transmit_DMA(&huart1, error , strlen((char*) error) );
					  }
				  }
			  }
			  if( (buffTmp[3]=='d') && (buffTmp[4]== '=')  ){

				  if( (buffTmp[9]=='-') && (buffTmp[12]== '-') ){

				  tmpBuff[0] = buffTmp[7];
				  tmpBuff[1] = buffTmp[8];
				  setDate.Year =  atoi( (char*) tmpBuff);

				  tmpBuff[0] = buffTmp[10];
				  tmpBuff[1] = buffTmp[11];
				  setDate.Month =  atoi( (char*) tmpBuff);

				  tmpBuff[0] = buffTmp[13];
				  tmpBuff[1] = buffTmp[14];
				  setDate.Date =  atoi( (char*) tmpBuff);

				  if (HAL_RTC_SetDate(&hrtc, &setDate, RTC_FORMAT_BIN) != HAL_OK)
				  {
				    Error_Handler();
				  }

				  txDone = 1;
				  uint8_t error[] = "OK\n";
				  HAL_UART_Transmit_DMA(&huart1, error , strlen((char*) error) );

				  }else{

					  if(txDone == 0){
						  txDone = 1;
						  uint8_t error[] = "Podaj poprawne dane np. \"at+d=2016-12-31\"\n";
						  HAL_UART_Transmit_DMA(&huart1, error , strlen((char*) error) );
					  }

				  }
			  }

			  if( (buffTmp[3]=='s') && (buffTmp[4]== '=')  ){
				tmpBuff[0] = buffTmp[5];
				tmpBuff[1] = buffTmp[6];
				sciemniacz = atoi((char*) tmpBuff);

				txDone = 1;
				uint8_t error[] = "OK\n";
				HAL_UART_Transmit_DMA(&huart1, error , strlen((char*) error) );
			  }

			  if( (buffTmp[3]=='a') && (buffTmp[4]!= '=')  ){
				if (txDone == 0) {
					strcpy( (char*) blueTxFrame, "Odczyt ADC = ");
					itoa(ADCread, (char*) tmp2, 10);
					strcat( (char*) blueTxFrame, (char*) tmp2);
					strcat((char*) blueTxFrame, "\n");
					lenth = strlen((char*) blueTxFrame);
					if (lenth > 0) {
						txDone = 1;
						HAL_UART_Transmit_DMA(&huart1, blueTxFrame, lenth);
					}
				}
			  }

		  }else{
			  if(txDone == 0){
				  txDone = 1;
				  uint8_t error[] = "Podaj poprawne dane np. \"at+t\",\"at+t=22:01:55\", \"at+d=2016-12-30\" \n";
				  HAL_UART_Transmit_DMA(&huart1, error , strlen((char*) error) );
			  }
		  }
	  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

	static uint32_t ADCsrednieTab[5], ADCsrednie;
	static uint8_t i, stan;
	uint8_t tmp2[10], lenth;

	if (hadc->Instance == ADC1) {


		crcWynik = 0;
		crcbuff[0] = 1;
		crcbuff[0] = __REV(  crcbuff[0]);
		uint32_t lenB = 1;
		crcWynik = HAL_CRC_Calculate(&hcrc, (uint32_t *) crcbuff, lenB);
		crcWynik =( __REV( crcWynik) )^ 0xFFFFFFFF;



		ADCread = HAL_ADC_GetValue(hadc);
		HAL_ADC_Stop_IT(hadc);

		if(i>4)i=0;
		ADCsrednieTab[i++] = ADCread;
		uint8_t j;
		ADCsrednie = 0;
		for (j = 0; j < 5; ++j) {
			ADCsrednie+= ADCsrednieTab[j];
		}
		ADCsrednie/=5;

		switch (stan) {
			case 0: //max
				sciemniacz = 0;

				if (ADCsrednie < ADCpoziom[2]) {
					stan = 1;
				}

				break;
			case 1: //srednio
				sciemniacz = 30;

				if (ADCsrednie < ADCpoziom[1]) {
					stan = 2;
				}else if (ADCsrednie > ADCpoziom[2] + 400)
					stan = 0;
				break;
			case 2: //ciemno
				sciemniacz = 50;

				if (ADCsrednie < ADCpoziom[0]) {
					stan = 3;
				}else if (ADCsrednie > ADCpoziom[1] + 400)
					stan = 1;
				break;
			case 3: //nic
				sciemniacz = 80;
				if (ADCsrednie > ADCpoziom[0] + 400)
					stan = 2;
				break;

			default:
				stan = 0;
				break;
		}



		strcpy( (char*) blueTxFrame, "Odczyt ADC = ");
		itoa(  ADCread, (char*) tmp2, 10);
		strcat( (char*) blueTxFrame, (char*) tmp2);
		strcat( (char*) blueTxFrame, " ADC srednie = ");
		itoa(ADCsrednie, (char*) tmp2, 10);
		strcat((char*) blueTxFrame, (char*) tmp2);
		strcat((char*) blueTxFrame, " sciemniacz = ");
		itoa(sciemniacz, (char*) tmp2, 10);
		strcat((char*) blueTxFrame, (char*) tmp2);
		strcat((char*) blueTxFrame, " PowerUp  = ");
		itoa(eeIloscOdpalen, (char*) tmp2, 10);
		strcat((char*) blueTxFrame, (char*) tmp2);
		strcat((char*) blueTxFrame, "\n");


		lenth = strlen((char*) blueTxFrame);
		if (lenth > 0) {
			txDone = 1;
			HAL_UART_Transmit_DMA(&huart1, blueTxFrame, lenth);
		}
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


uint8_t rtcSetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef* time){

	uint8_t out = 0;
	if (HAL_RTC_SetTime(hrtc, time, RTC_FORMAT_BIN) != HAL_OK) {
		out = 1;
		Error_Handler();
	}
	return out;
}

uint8_t rtcGetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef* time){
	uint8_t out = 0;
	if (HAL_RTC_GetTime(hrtc, time, RTC_FORMAT_BIN) != HAL_OK) {
		out = 1;
		Error_Handler();
	}
	return out;
}

void displayPrzeliczCzas(void){

	cyfra_1 = getTime.Hours/10;
	if (cyfra_1 == 0) cyfra_1 = NIC;
	cyfra_2 = getTime.Hours%10;
	cyfra_3 = getTime.Minutes/10;
	cyfra_4 = getTime.Minutes%10;

	//przekazanie do struktury wyswietlacza
	wyswietlaCyfrySet.cyfra_1 = cyfra_1;
	wyswietlaCyfrySet.cyfra_2 = cyfra_2;
	wyswietlaCyfrySet.cyfra_3 = cyfra_3;
	wyswietlaCyfrySet.cyfra_4 = cyfra_4;
	wyswietlaCyfrySet.cyfra_5 = cyfra_5;
	wyswietlaCyfrySet.cyfra_6 = cyfra_6;
	wyswietlaCyfrySet.cyfra_7 = cyfra_7;
	wyswietlaCyfrySet.cyfra_8 = cyfra_8;
	wyswietlaCyfrySet.cyfra_9 = cyfra_9;
	wyswietlaCyfrySet.cyfra_10 = cyfra_10;
	wyswietlaCyfrySet.cyfra_11 = cyfra_11;
	wyswietlaCyfrySet.cyfra_12 = cyfra_12;
	wyswietlaCyfrySet.kropka_1 = kropka_1;
	wyswietlaCyfrySet.kropka_2 = kropka_2;
	wyswietlaCyfrySet.kropka_3 = kropka_3;
	wyswietlaCyfrySet.kropka_4 = kropka_4;
	wyswietlaCyfrySet.kropka_5 = kropka_5;
	wyswietlaCyfrySet.kropka_6 = kropka_6;
	wyswietlaCyfrySet.kropka_7 = kropka_7;
	wyswietlaCyfrySet.kropka_8 = kropka_8;
	wyswietlaCyfrySet.kropka_9 = kropka_9;
	wyswietlaCyfrySet.kropka_10 = kropka_10;
	wyswietlaCyfrySet.kropka_11 = kropka_11;
	wyswietlaCyfrySet.kropka_12 = kropka_12;
}

void przyciskiRtc(void){
	static uint8_t wcisnietyPlusFlaga = 0;
	static uint8_t wcisnietyMinusFlaga = 0;

	//plus
	if (!HAL_GPIO_ReadPin( PPLUS_GPIO_Port, PPLUS_Pin)) {
		osDelay(30);
		if (!HAL_GPIO_ReadPin( PPLUS_GPIO_Port, PPLUS_Pin)) {
			wcisnietyMinusFlaga = 0;

			if(!wcisnietyPlusFlaga) rtcTimer1Sec = 255;

			if (!rtcGetTime(&hrtc, &setTime)) {
				++setTime.Minutes;
				if( rtcTimer1Sec <254 ){
					uint8_t minutyTmp = ( ( setTime.Minutes / 10 ) +1) * 10;
					setTime.Minutes = minutyTmp;
					if (rtcTimer1Sec > 244) osDelay(200);
				}
				if (setTime.Minutes > 59) {
					setTime.Minutes = 0;
					++setTime.Hours;
					if(setTime.Hours >23){
						setTime.Hours = 0;
					}
				}
				setTime.Seconds = 0;
				rtcSetTime(&hrtc, &setTime);
				rtcGetTime(&hrtc, &getTime);
			}
			displayPrzeliczCzas();
			osDelay(90);
			wcisnietyPlusFlaga = 1;
		}
	}
	//minus
	else if (!HAL_GPIO_ReadPin( PMIN_GPIO_Port, PMIN_Pin)) {
		osDelay(30);
		if (!HAL_GPIO_ReadPin( PMIN_GPIO_Port, PMIN_Pin)) {
			wcisnietyPlusFlaga = 0;

			if (!wcisnietyMinusFlaga)
				rtcTimer1Sec = 255;

			if (!rtcGetTime(&hrtc, &setTime)) {
				if (rtcTimer1Sec < 254) {
					uint8_t minutyTmp = ((setTime.Minutes / 10) - 1) * 10;
					setTime.Minutes = minutyTmp;
					if (rtcTimer1Sec > 244) osDelay(200);
				}else{
					--setTime.Minutes;
				}
				if (setTime.Minutes > 59) {
					setTime.Minutes = 59;
					--setTime.Hours;
					if (setTime.Hours > 23) {
						setTime.Hours = 23;
					}
				}
				setTime.Seconds = 0;
				rtcSetTime(&hrtc, &setTime);
				rtcGetTime(&hrtc, &getTime);
			}
			displayPrzeliczCzas();
			osDelay(90);
			wcisnietyMinusFlaga = 1;
		}
	}else if (rtcTimer1Sec) {
		wcisnietyPlusFlaga = 0;
		wcisnietyMinusFlaga = 0;
		rtcTimer1Sec = 0;
	} else {
		osDelay(20);
	}
}

void eepromStart(void){

	HAL_FLASH_Unlock();
	EE_Init();

	uint8_t i;
	for(i=0;i<NB_OF_VAR; ++i) {
		EE_ReadVariable(VirtAddVarTab[i], &eepromVarDataTab[i]);
	}
	eeIloscOdpalen = eepromVarDataTab[0];

	//licznik powerup
	++eeIloscOdpalen;
	eepromWrite(VirtAddVarTab[0], eeIloscOdpalen);

	HAL_FLASH_Lock();
}
void eepromWrite(uint16_t adr, uint16_t value){
	HAL_FLASH_Unlock();
	EE_WriteVariable(adr, value);
	HAL_FLASH_Lock();
}
void eepromRead(uint16_t adr, uint16_t* value){
	HAL_FLASH_Unlock();
	EE_ReadVariable(adr, value);
	HAL_FLASH_Lock();
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
