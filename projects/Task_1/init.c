//**************************************************************************************************
// Includes
//**************************************************************************************************
#include "main.h"


//**************************************************************************************************
//Procedure SetSysClock_HSE_84() 
//**************************************************************************************************

//* Конфигурация системы тактирования от внешнего осциллятора (HSE 84MHz) *//
void SetSysClock_HSE_84(void)
{
   //* Сконфигурируем систему тактирование HSI *//
   RCC->CR |= ((uint32_t)RCC_CR_HSION); //включаем HSI и дожидаемся его готовности
   while(!(RCC->CR & RCC_CR_HSIRDY))
   {
   }

   RCC->APB1ENR |= RCC_APB1ENR_PWREN;
   PWR->CR |= PWR_CR_VOS;

   //* Определяем делители шин *//
   RCC->CFGR |= RCC_CFGR_HPRE_DIV1; //system clock not divided
   RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; //AHB clock divided by 1 for APB2
   RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; //AHB clock divided by 2 for APB1

   //* Переключаем на внутренний HSI *//
   RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
   while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_HSI)
   {
   }

   RCC->CR &= ~RCC_CR_PLLON; //выключаем PLL для перенастройки

   RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM; //сбрасываем значение PLL->PLLCFGR

   //* Сконфигурируем систему тактирование HSE *//
   RCC->CR |= ((uint32_t)RCC_CR_HSEON); //включаем HSE и дожидаемся его готовности
   while(!(RCC->CR & RCC_CR_HSERDY))
   {
   }

   RCC->APB1ENR |= RCC_APB1ENR_PWREN;
   PWR->CR |= PWR_CR_VOS;

   //* Определяем делители шин *//
   RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
   RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
   RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

   //* Определяем кф PLL *//
   uint16_t PLL_M = 4;
   uint16_t PLL_N = 84;
   uint16_t PLL_P = 2;
   uint16_t PLL_Q = 4;

   RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) | (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24); //заносим кофигурацию PLL

   //* Включаем PLL и дожидаемся готовности PLL *//
   RCC->CR |= RCC_CR_PLLON;
   while((RCC->CR & RCC_CR_PLLRDY) == 0)
   {
   }

   //* Настраиваем Flash prefetch, instruction cache, data cache and wait state (2WS (3CPU cycles) | 60 < HCLK <= 90 | 2.7V - 3.6V) *//
   FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;

   //* Переключаем системное тактирование на PLL *//
   RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));

   RCC->CFGR |= RCC_CFGR_SW_PLL;
   while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
   {
   }

}

//**************************************************************************************************
// Procedure Configure_ADC()
//**************************************************************************************************

//* Конфигурация ADC1 PA1 Channel:6*//
void Configure_ADC(void)
{
   GPIO_InitTypeDef GPIO_Init_ADC;
   ADC_InitTypeDef ADC_Init_User;

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //включили тактирование порта GPIOA

   //*Настраиваем ножки контроллера*//
   GPIO_Init_ADC.GPIO_Pin = GPIO_Pin_1;
   GPIO_Init_ADC.GPIO_Mode = GPIO_Mode_AN;
   GPIO_Init_ADC.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_Init_ADC.GPIO_OType = GPIO_OType_PP;
   GPIO_Init_ADC.GPIO_PuPd = GPIO_PuPd_NOPULL;

   GPIO_Init(GPIOA, &GPIO_Init_ADC); //инициализировали настроенную структуру

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC,ENABLE); //включили тактирование порта ADC

   ADC_Init_User.ADC_Resolution = ADC_Resolution_12b; //разрешающая способность 12 бит
   ADC_Init_User.ADC_ScanConvMode = DISABLE; //режим сканироване отключен
   ADC_Init_User.ADC_ContinuousConvMode = DISABLE; //режим продолжительности преобразования отключен
   ADC_Init_User.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //режим конвертации от внешних триггеров отключен
   ADC_Init_User.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1 ;
   ADC_Init_User.ADC_DataAlign = ADC_DataAlign_Right; //выравнивание по правому краю
   ADC_Init_User.ADC_NbrOfConversion = 1; //сканируем 1 канал

   ADC_Init(ADC1, &ADC_Init_User); //инициализировали настроенную структуру

   ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_56Cycles); //c 1 канала считываем аналоговый сигнал

   NVIC_EnableIRQ(ADC_IRQn); //разрешили глобальное прерывание ADC

   ADC_ITConfig(ADC1,ADC_IT_EOC, ENABLE); //включили  локальное прерывание по флагу EOC

   ADC_Cmd(ADC1, ENABLE); //запустили ADC

}

//**************************************************************************************************
// Procedure Configure_USART2()
//**************************************************************************************************

//* Конфигурация USART2: USART2_TX - PA2 *//
void Configure_USART2(void)
{
   GPIO_InitTypeDef GPIO_Init_USART2;
   USART_InitTypeDef USART2_Init;
   NVIC_InitTypeDef USART2_NVIC_Init_Structure = { 0x0 };

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //включили тактирование порта GPIOA

   //*Настраиваем ножки контроллера*//
   GPIO_Init_USART2.GPIO_Pin = GPIO_Pin_2;
   GPIO_Init_USART2.GPIO_Mode = GPIO_Mode_AF;
   GPIO_Init_USART2.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init_USART2.GPIO_OType = GPIO_OType_PP;
   GPIO_Init_USART2.GPIO_PuPd = GPIO_PuPd_UP;

   GPIO_Init(GPIOA, &GPIO_Init_USART2); //инициализировали настроенную структуру

   GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //изменили преобразование указанного пина

   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //включили тактирование порта USART2

   USART2_Init.USART_BaudRate = 9600; //скорость передачи данных
   USART2_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //без аппаратного управления контроллером
   USART2_Init.USART_Mode = USART_Mode_Tx ; //настройка на передачу данных
   USART2_Init.USART_Parity = USART_Parity_No; //без чётного бита
   USART2_Init.USART_StopBits = USART_StopBits_1; // 1 стоп-бит
   USART2_Init.USART_WordLength = USART_WordLength_8b; //длина передаваемого слова из 8 бит

   USART_Init(USART2, &USART2_Init); //инициализировали структуру

   //* Конфгурация прерывания USART2 *//
   USART2_NVIC_Init_Structure.NVIC_IRQChannel = USART2_IRQn; //разрешили глобальное прерывание USART2
   USART2_NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 0x00; //приоритет
   USART2_NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 1;
   USART2_NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&USART2_NVIC_Init_Structure); //инициализировали структуру


   USART_Cmd(USART2, ENABLE); //запустили USART2
}

//**************************************************************************************************
// Procedure Configure_USART3()
//**************************************************************************************************

//*Конфигурация USART3: Конфигурация USART3_TX - PB10, USART3_RX - PB11 *//
void Configure_USART3(void)
{
    GPIO_InitTypeDef GPIO_Init_USART3;
    USART_InitTypeDef USART3_Init;
    NVIC_InitTypeDef USART3_NVIC_Init_Structure = { 0x0 };

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //включили тактирование порта GPIOB

    //*Настраиваем ножки контроллера*//
    GPIO_Init_USART3.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init_USART3.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init_USART3.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init_USART3.GPIO_OType = GPIO_OType_PP;
    GPIO_Init_USART3.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(GPIOB, &GPIO_Init_USART3); //инициализировали структуру 

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //изменили преобразование указанного пина
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); //изменили преобразование указанного пина

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //включили тактирование порта USART3

    USART3_Init.USART_BaudRate = 9600; //скорость передачи данных
    USART3_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //без аппаратного управления контроллером
    USART3_Init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //настройка на прием и передачу данных
    USART3_Init.USART_Parity = USART_Parity_No; //без чётного бита
    USART3_Init.USART_StopBits = USART_StopBits_1; // 1 стоп-бит
    USART3_Init.USART_WordLength = USART_WordLength_8b; //длина передаваемого слова из 8 бит

    USART_Init(USART3, &USART3_Init); //инициализировали структуру 

    //* Конфгурация прерывания USART3 *//

    USART3_NVIC_Init_Structure.NVIC_IRQChannel = USART3_IRQn; //разрешили глобальное прерывание USART3
    USART3_NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 0x01; //приоритет
    USART3_NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 0;
    USART3_NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&USART3_NVIC_Init_Structure); //инициализировали структуру

    USART_Cmd(USART3, ENABLE); //запустили USART3
}

//**************************************************************************************************
// Procedure Configure_USART6()
//**************************************************************************************************

//*Конфигурация USART6: Конфигурация USART6_TX - PC6 *//
void Configure_USART6(void)
{
    GPIO_InitTypeDef GPIO_Init_USART6;
    USART_InitTypeDef USART6_Init;
    NVIC_InitTypeDef USART6_NVIC_Init_Structure = { 0x0 };

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //включили тактирование порта GPIOС

    //*Настраиваем ножки контроллера*//
    GPIO_Init_USART6.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init_USART6.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init_USART6.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init_USART6.GPIO_OType = GPIO_OType_PP;
    GPIO_Init_USART6.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(GPIOC, &GPIO_Init_USART6); //инициализировали структуру GPIOC_Init

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); //изменили преобразование указанного пина

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); //включили тактирование порта USART3

    USART6_Init.USART_BaudRate = 9600; //скорость передачи данных
    USART6_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //без аппаратного управления контроллером
    USART6_Init.USART_Mode = USART_Mode_Tx; //настройка на прием и передачу данных
    USART6_Init.USART_Parity = USART_Parity_No; //без чётного бита
    USART6_Init.USART_StopBits = USART_StopBits_1; // 1 стоп-бит
    USART6_Init.USART_WordLength = USART_WordLength_8b; //длина передаваемого слова из 8 бит

    USART_Init(USART6, &USART6_Init); //инициализировали структуру 

    //* Конфгурация прерывания USART6 *//

    USART6_NVIC_Init_Structure.NVIC_IRQChannel = USART6_IRQn; //разрешили глобальное прерывание USART6
    USART6_NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 0x01; //приоритет
    USART6_NVIC_Init_Structure.NVIC_IRQChannelSubPriority = 1;
    USART6_NVIC_Init_Structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&USART6_NVIC_Init_Structure); //инициализировали структуру

    USART_Cmd(USART6, ENABLE); //запустили USART2
}
