#ifndef MAIN_H
#define MAIN_H

//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <stm32f4xx.h>

#define FALSE 0
#define TRUE !FALSE

//*Описываем информацию для лог-передачи*//
#define CASE1                     "  <<  1)---THE_PROCEDURE_WAS_USED_: SETSysClock_HSE_84()  >>"
#define CASE1_NUM 60

#define CASE2                     "  <<  2)---THE_PROCEDURE_WAS_USED_: Configure_USART6()  >>"
#define CASE2_NUM 58

#define CASE3                     "  <<  3)---THE_PROCEDURE_WAS_USED_: Configure_ADC()  >>"
#define CASE3_NUM 55

#define CASE4                     "  <<  4)---THE_PROCEDURE_WAS_USED_: Configure_USART2()  >>"
#define CASE4_NUM 58

#define CASE5                     "  <<  5)---THE_PROCEDURE_WAS_USED_: Configure_USART3()  >>"
#define CASE5_NUM 58

#define CASE6                     "  <<  6)---THE_PROCEDURE_WAS_USED_: ADC_SoftwareStartConv(ADC1)  >>"
#define CASE6_NUM 67

#define CASE7                     "  <<  7)---THE_PROCEDURE_WAS_USED_: USART2_SendData(SendData,NumTX)  >>"
#define CASE7_NUM 71

#define CASE8                     "  <<  8)---THE_PROCEDURE_WAS_USED_: USART3_ReceiveData(ReceivedData, NumRX)  >>"
#define CASE8_NUM 79

#define CASE9                     "  <<  9)---THE_PROCEDURE_WAS_USED_: Data_Is_Collect(ReceivedData, (uint8_t*)&ReceivedData_ADC)  >>"
#define CASE9_NUM 98

#define CASE10                     "  <<  10)---THE_PROCEDURE_WAS_USED_: USART3_SendData(ReceivedData,NumTX)  >>"
#define CASE10_NUM 77

//**************************************************************************************************
// Declarations and definitions
//**************************************************************************************************

//* Перечисляем логи *//
typedef enum {
   LOG_IS_SetSysClock_HSE_84 = 1,
   LOG_IS_Configure_USART6,
   LOG_IS_Configure_ADC,
   LOG_IS_Configure_USART2,
   LOG_IS_Configure_USART3,
   LOG_IS_ADC_SoftwareStartConv_ADC1,
   LOG_IS_USART2_SendData,
   LOG_IS_USART3_ReceiveData,
   LOG_IS_DATA_IS_Collect,
   LOG_IS_USART3_SendData} Log;


void SetSysClock_HSE_84(void);
void Configure_ADC(void);
void Configure_USART2(void);
void Configure_USART3(void);
void Configure_USART6(void);

#endif
