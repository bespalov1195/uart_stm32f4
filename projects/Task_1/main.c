//**************************************************************************************************
// Includes
//**************************************************************************************************

#include "main.h"
#define UREF 3000 //опорное напряжение 3V
#define TWELVE_BIT 4096//максимальное значение для 12bit: 2^12 = 4096

//**************************************************************************************************
// Declarations and definitions
//**************************************************************************************************

void ADC_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void USART6_IRQHandler(void);

void Logging(uint8_t Logging_Num);
void Data_Is_Collect(uint8_t *ReceivedDataPtr_ADC, uint8_t *CollectDataPtr);

void USART2_SendData(uint8_t *pSendData, uint8_t nNumOfDataToSend);
void USART3_SendData(uint8_t *pSendData, uint8_t nNumOfDataToSend);
void USART3_ReceiveData(uint8_t* pReceivedData, uint8_t nNumOfDataToReceive);
void USART6_SendLog(uint8_t *pSendData, uint8_t nNumOfDataToSend);

//**************************************************************************************************
// Global variable
//**************************************************************************************************

uint32_t Data_ADC = 0; //преобразованное значение в mV
uint32_t ReceivedData_ADC; //полученные  преобразованные значение из USART2 --> USART3 в mV

uint8_t *USART2_SendDataPtr; //указатель на буфер, из которого будут переданы данные USART2
uint8_t USART2_NumOfDataToSend; //количество байтов для передачи USART2
uint8_t USART2_NumOfDataSended; //количество переданных байтов USART2

uint8_t *USART3_SendDataPtr; //указатель на буфер, из которого будут переданы данные USART3
uint8_t USART3_NumOfDataToSend; //количество байтов для передачи USART3
uint8_t USART3_NumOfDataSended; //количество переданных байтов USART3

uint8_t *USART3_ReceivedDataPtr; //указатель на буфер, в который будут сохранены полученные данные
uint8_t USART3_NumOfDataToReceive; //количество байтов для приёма
uint8_t USART3_NumOfDataReceived; //количество принятых байтов

uint8_t *USART6_SendDataPtr; //указатель на буфер, из которого будут переданы данные USART6
uint8_t USART6_NumOfDataToSend; //количество байтов для передачи USART6
uint8_t USART6_NumOfDataSended; //количество переданных байтов USART6

//* Определение символьных массивов для логов
uint8_t Log_SendData1[CASE1_NUM] = CASE1;
uint8_t Log_SendData2[CASE2_NUM] = CASE2;
uint8_t Log_SendData3[CASE3_NUM] = CASE3;
uint8_t Log_SendData4[CASE4_NUM] = CASE4;
uint8_t Log_SendData5[CASE5_NUM] = CASE5;
uint8_t Log_SendData6[CASE6_NUM] = CASE6;
uint8_t Log_SendData7[CASE7_NUM] = CASE7;
uint8_t Log_SendData8[CASE8_NUM] = CASE8;
uint8_t Log_SendData9[CASE9_NUM] = CASE9;
uint8_t Log_SendData10[CASE10_NUM] = CASE10;



uint8_t USART_TX_ReadyToExchange = FALSE; //готов к передаче
uint8_t USART_RX_ReadyToExchange = FALSE; //готов к приему

uint8_t USART_TX_ReadyToExchange_FLAG = FALSE; //готов к передаче лог-сообщений

uint8_t SendData[4] ={0x00,0x00,0x00,0x00}; //опледеление массива для отправления данных (4 байта)
uint8_t ReceivedData[4] ={}; //опледеление массива для получения данных (4 байта)
uint8_t NumTX = 4; //количество элементов в массеве SendData[]
uint8_t NumRX = 4; //количество элементов в массеве ReceivedData[]

//**************************************************************************************************
// Function main()     
//**************************************************************************************************

int main(void)
{

	SetSysClock_HSE_84(); //конфигурация тактирования 84MHz

	Configure_USART6(); //конфигурация USART6
	Logging(LOG_IS_SetSysClock_HSE_84); //передаем лог-сообщение
	Logging(LOG_IS_Configure_USART6); //передаем лог-сообщение

	Configure_ADC();//конфигурация ACD
	Logging(LOG_IS_Configure_ADC); //передаем лог-сообщение

	Configure_USART2();//конфигурация USART2
	Logging(LOG_IS_Configure_USART2); //передаем лог-сообщение

	Configure_USART3();//конфигурация USART3
	Logging(LOG_IS_Configure_USART3); //передаем лог-сообщение

	ADC_SoftwareStartConv(ADC1); //включаем преобразование ADC
	Logging(LOG_IS_ADC_SoftwareStartConv_ADC1); //передаем лог-сообщение

	USART2_SendData(SendData, NumTX); //передаем данные из USART2 в USART3 
	while(!USART_TX_ReadyToExchange == TRUE) {}; //ждем, готовности завершения передачи

	Data_Is_Collect(ReceivedData, (uint8_t*)&ReceivedData_ADC); //собираем переданный массив в единый пакет (uint32_t)

	USART3_SendData(ReceivedData, NumRX); //передаем данные из USART3 в PC
	while(!USART_TX_ReadyToExchange == TRUE) {}; //ждем, готовности завершения передачи

	while (1)
	{

	}

	return 0;
}

//**************************************************************************************************
// Procedure ADC_IRQHandler()     
//**************************************************************************************************

//* Обработчик прерывания ADC *//
void ADC_IRQHandler(void)
{
   if (ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET) // прерывание произошло по флагу EOC
   {
      ADC_ClearITPendingBit(ADC1, ADC_IT_EOC); //программно сбросили флаг прерывания

      int i; 

      Data_ADC = (UREF * (uint32_t) ADC_GetConversionValue(ADC1)) / TWELVE_BIT; //получили преобразованное значение по формуле в mV : (Data_ADC = (Uref * DOR)/ 4096)

      //*Разбиваем полученный пакет uint32_t в массив пакетов uint8_t*//
      uint8_t *ptr1_8 = (__uint8_t*)&Data_ADC; 

      for(i = 0; i < 4; i++)
         {
             SendData[i] = *(ptr1_8 + i);
         }

      ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE); //выключили прерывание по флагу EOC

   }
}

//**************************************************************************************************
// Procedure USART2_SendData()    
//**************************************************************************************************

//*Передача данных по USART2*//
void USART2_SendData(uint8_t *pSendData, uint8_t nNumOfDataToSend)
{
	Logging(LOG_IS_USART2_SendData); //передаем лог-сообщение
	USART2_SendDataPtr = pSendData;
	USART2_NumOfDataToSend = nNumOfDataToSend;
	USART2_NumOfDataSended = 0;
	USART_TX_ReadyToExchange = FALSE;

	USART3_ReceiveData(ReceivedData, NumRX); //полученые данные заносим в массив ReceivedData;

	USART_ITConfig(USART2, USART_IT_TC, ENABLE); //включили прерывание по флагу TC (USART2)
}

//**************************************************************************************************
// Procedure USART3_ReceiveData()  
//**************************************************************************************************

//*Прием данных по USART3*//
void USART3_ReceiveData(uint8_t* pReceivedData, uint8_t nNumOfDataToReceive)
{
	Logging(LOG_IS_USART3_ReceiveData); //передаем лог-сообщение
	USART3_ReceivedDataPtr = pReceivedData;
	USART3_NumOfDataToReceive = nNumOfDataToReceive;
	USART3_NumOfDataReceived = 0;
	USART_RX_ReadyToExchange = FALSE;


	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //включили прерывание по флагу RXNE (USART3)
}

//**************************************************************************************************
// Procedure USART3_SendData()
//**************************************************************************************************

//*Передача данных по USART3*//
void USART3_SendData(uint8_t *pSendData, uint8_t nNumOfDataToSend)
{
	Logging(LOG_IS_USART3_SendData); //передаем лог-сообщение
	USART3_SendDataPtr = pSendData;
	USART3_NumOfDataToSend = nNumOfDataToSend;
	USART3_NumOfDataSended = 0;
	USART_TX_ReadyToExchange = FALSE;

	USART_ITConfig(USART3, USART_IT_TC, ENABLE); //включили прерывание по флагу TC (USART3)
}

//**************************************************************************************************
// Procedure USART6_SendLog()
//**************************************************************************************************

//*Передача данных по USART6*//
void USART6_SendLog(uint8_t *pSendData, uint8_t nNumOfDataToSend)
{
	USART6_SendDataPtr = pSendData;
	USART6_NumOfDataToSend = nNumOfDataToSend;
	USART6_NumOfDataSended = 0;
	USART_TX_ReadyToExchange_FLAG = FALSE;


	USART_ITConfig(USART6, USART_IT_TC, ENABLE); //включили прерывание по флагу TC (USART6)
}

//**************************************************************************************************
// Procedure USART2_IRQHandler()
//**************************************************************************************************

//* Обработчик прерывания USART2 *//
void USART2_IRQHandler(void)
{
	//*Если сработал флаг TC, то передаем данные*//
	if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_TC);

		USART_SendData(USART2, (uint8_t)*USART2_SendDataPtr); //передаем данные по USART2

		USART2_SendDataPtr++;
		USART2_NumOfDataSended++;

		if (USART2_NumOfDataSended == USART2_NumOfDataToSend)
		{
			USART_ITConfig(USART2, USART_IT_TC, DISABLE); //выключили прерывание по флагу TC (USART2)
			USART_TX_ReadyToExchange = TRUE; //передача данных завершена
		}
	}
}

//**************************************************************************************************
// Procedure USART3_IRQHandler()
//**************************************************************************************************

//* Обработчик прерывания USART3 *//
void USART3_IRQHandler()
{
	//*Если сработал флаг RXNE, то принимаем данные*//
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);

		*USART3_ReceivedDataPtr = (uint8_t)USART_ReceiveData(USART3); //принимаем данные по USART3

		USART3_ReceivedDataPtr++;
		USART3_NumOfDataReceived++;

		if (USART3_NumOfDataReceived == USART3_NumOfDataToReceive)
		{
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); //выключили прерывание по флагу RXNE (USART3)
			USART_RX_ReadyToExchange = TRUE; //прием данных завершен
		}
	}

	//*Eсли сработал флаг TC, то передаем данные*//
	if (USART_GetITStatus(USART3, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_TC);

		USART_SendData(USART3, (uint8_t)*USART3_SendDataPtr); //передаем данные по USART3

		USART3_SendDataPtr++;
		USART3_NumOfDataSended++;

		if (USART3_NumOfDataSended == USART3_NumOfDataToSend)
		{
			USART_ITConfig(USART3, USART_IT_TC, DISABLE); //выключили прерывание по флагу TC (USART3)
			USART_TX_ReadyToExchange = TRUE; //передача данных завершена
		}
	}

}

//**************************************************************************************************
// Procedure USART6_IRQHandler()
//**************************************************************************************************

//* Обработчик прерывания USART3 *//

void USART6_IRQHandler()
{
	//*Eсли сработал флаг TC, то передаем данные*//
	if (USART_GetITStatus(USART6, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART6, USART_IT_TC);

		USART_SendData(USART6, (uint8_t)*USART6_SendDataPtr); //передаем лог-сообщение по USART6

		USART6_SendDataPtr++;
		USART6_NumOfDataSended++;

		if (USART6_NumOfDataSended == USART6_NumOfDataToSend)
		{
			USART_ITConfig(USART6, USART_IT_TC, DISABLE); //выключили прерывание по флагу TC (USART6)
			USART_TX_ReadyToExchange_FLAG = TRUE; //передача данных завершена
	}	}

}

//**************************************************************************************************
// Procedure Logging()
//**************************************************************************************************

//* Процедура собирает пакет ReceivedData_ADC (uint32_t)  из полученного массива пакетов ReceivedData[] (uint8_t)*//
void Data_Is_Collect(uint8_t *ReceivedDataPtr_ADC, uint8_t *CollectDataPtr)
{
	Logging(LOG_IS_DATA_IS_Collect); //передаем лог-сообщение

	uint8_t i;

	//*Собираем пакет uint32_t из полученного массива пакетов uint8_t*//
	for(i = 0; i < 4; i++)
	{
		*(CollectDataPtr + i) = ReceivedDataPtr_ADC[i];
	}
} 

//**************************************************************************************************
// Procedure Logging()
//**************************************************************************************************

//* Процедура передачи лог-сообщения *//
void Logging(uint8_t Logging_Num)
{

    //*Ищем Logging_Num и передаем лог-сообщение по USART6 *//
    switch(Logging_Num)
    {
        case 1:
        {
			USART6_SendLog(Log_SendData1, CASE1_NUM); //передаем лог-сообщение из USART6 в PC
			while(!USART_TX_ReadyToExchange_FLAG == TRUE) {}; //ждем, готовности завершения передачи
            break;
        }
        case 2:
        {
			USART6_SendLog(Log_SendData2, CASE2_NUM); //передаем лог-сообщение из USART6 в PC
			while(!USART_TX_ReadyToExchange_FLAG == TRUE){}; //ждем, готовности завершения передачи
			break;
        }
        case 3:
        {

            USART6_SendLog(Log_SendData3, CASE3_NUM); //передаем лог-сообщение из USART6 в PC
			while(!USART_TX_ReadyToExchange_FLAG == TRUE){}; //ждем, готовности завершения передачи
            break;
        }
        case 4:
        {

            USART6_SendLog(Log_SendData4, CASE4_NUM); //передаем лог-сообщение из USART6 в PC
			while(!USART_TX_ReadyToExchange_FLAG == TRUE){}; //ждем, готовности завершения передачи
            break;
        }
        case 5:
        {
            USART6_SendLog(Log_SendData5, CASE5_NUM); //передаем лог-сообщение из USART6 в PC
			while(!USART_TX_ReadyToExchange_FLAG == TRUE){}; //ждем, готовности завершения передачи
            break;
        }
        case 6:
        {
            USART6_SendLog(Log_SendData6, CASE6_NUM); //передаем лог-сообщение из USART6 в PC
			while(!USART_TX_ReadyToExchange_FLAG == TRUE){}; //ждем, готовности завершения передачи
            break;
        }
        case 7:
        {
            USART6_SendLog(Log_SendData7, CASE7_NUM); //передаем лог-сообщение из USART6 в PC
			while(!USART_TX_ReadyToExchange_FLAG == TRUE){}; //ждем, готовности завершения передачи
            break;
        }
        case 8:
        {
            USART6_SendLog(Log_SendData8, CASE8_NUM); //передаем лог-сообщение из USART6 в PC
			while(!USART_TX_ReadyToExchange_FLAG == TRUE){}; //ждем, готовности завершения передачи
            break;
        }
        case 9:
        {
            USART6_SendLog(Log_SendData9, CASE9_NUM); //передаем лог-сообщение из USART6 в PC
			while(!USART_TX_ReadyToExchange_FLAG == TRUE){}; //ждем, готовности завершения передачи
			break;
        }
        case 10:
        {
            USART6_SendLog(Log_SendData10, CASE10_NUM); //передаем лог-сообщение из USART6 в PC
			while(!USART_TX_ReadyToExchange_FLAG == TRUE){}; //ждем, готовности завершения передачи
			break;
        }
        default:
            break;
    }


}
