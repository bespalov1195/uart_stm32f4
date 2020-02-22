Тестовое задание:
Необходимо написать следующий алгоритм под платформу STM32:

1. С любой доступной ноги контроллера, которая может принять аналоговый сигнал - организовать прием аналогового сигнала;
2. Обработать полученное значение сигнала (оцифровать до int32 в mV);
3. Передать обработанное значение в UART_1
4. Получить значение, отправляемое в UART_1 на UART_2
5. лог производимых операций контроллером (функций) выводить в UART_3.

-------------------------------------------------------

В тестовом задании написал следующий алгоритм под платформу STM32:

1. Организовал прием аналогового сигнала из потенциометра  на ножку PA1.
2. Через ADC1 оцифровал значение сигнала до int32 mV.
3. Передал обработанное значение в USART2 (ножка PA2)
4. Принял значение из USART2 в USART3 (ножка PB11)
Также организовал и организовал передачу данных из USART3_Tx на PC (ножка PB10)
5. Лог производимых операций контроллером (функций) вывел в USART6 (ножка PC6).

В архиве /Test_task/projects/Task_1/ нужно выполнить две команды:
1) make - собрать проект.
2) make-flash - прошить микроконтроллер STM32F407VGT6.

# Build system

Build system

## Prerequisites
* [arm-none-eabi](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads) compiler
* [stlink](https://github.com/texane/stlink) utility

## System reference
* [common_defs.mk](common_defs.mk) -- file with many useful rules. Every makefile that builds something should include it and define `BUILD_ROOT` variable -- path to this file.
* [Makefile](Makefile) -- "main hub" for a project. Here you can add you workshop directories and call `make` on them.
* [common](common) -- directory with various common stuff like startup files and interrupt tables. It's compiled once as a static library `libstmcommon.a` and linked with every binary produced with the build system.
* [stm_spl](stm_spl) -- [Standard Peripherials Library](https://www.st.com/en/embedded-software/stsw-stm32065.html) form ST. It makes development much easier but with an overhead.
* [projects](projects) -- directory with workshops.

## Various notes
* You can put worhskops git repository to [projects](projects) directory.
* It's possible to use helper commands from [common_defs.mk](common_defs.mk) to link your binaries. Example:

``` make
main: main.o
    @echo "Linking ELF"
    $(LINK_ELF)
    @echo "Making image"
    $(MAKE_IMAGE)
```
