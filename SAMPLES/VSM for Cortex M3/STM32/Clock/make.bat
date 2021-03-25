set targetcpu=STM32F103R6

set gccpath=d:\.um\c++\labcenter\tools\yagarto\bin

set include_dirs=-I.\core -I.\Inc -I.\Drivers\CMSIS\Device\ST\STM32F1xx\Include -I.\Middlewares\Third_Party\FreeRTOS\Source\include -I.\Middlewares\Third_Party\FreeRTOS\Source\CMSIS_RTOS -I.\Drivers\STM32F1xx_HAL_Driver\Inc -I.\Drivers\STM32F1xx_HAL_Driver\Inc\Legacy -I.\Middlewares\Third_Party\FreeRTOS\Source\portable\GCC\ARM_CM3 -I.\Drivers\CMSIS\Include

set compiler_args=-D__%targetcpu%__ %include_dirs% -mthumb -mcpu=cortex-m3 -Os -fdata-sections -ffunction-sections -mlong-calls -g3 -Wall -c -std=gnu99 -MD -MP -DSTM32F103x6
set linker_args=-Wl,-Map=out.map -Wl,--start-group -lm -Os -Wl,--end-group -Wl,--gc-sections -TSTM32F103X6_FLASH.ld -Wl,--cref -Wl,--entry=Reset_Handler -mthumb -mcpu=cortex-m3
set assembler_args=%include_dirs% -mthumb -g -mcpu=cortex-m3

del *.cof

rem COMPILE

set compiler=arm-none-eabi-gcc
set linker=arm-none-eabi-gcc
set assembler=arm-none-eabi-as
set size=arm-none-eabi-size.exe

%gccpath%\%compiler% ./Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c %compiler_args%
%gccpath%\%compiler% ./Middlewares/Third_Party/FreeRTOS/Source/croutine.c %compiler_args%
%gccpath%\%compiler% ./Middlewares/Third_Party/FreeRTOS/Source/event_groups.c %compiler_args%
%gccpath%\%compiler% ./Middlewares/Third_Party/FreeRTOS/Source/list.c %compiler_args%
%gccpath%\%compiler% ./Middlewares/Third_Party/FreeRTOS/Source/queue.c %compiler_args%
%gccpath%\%compiler% ./Middlewares/Third_Party/FreeRTOS/Source/tasks.c %compiler_args%
%gccpath%\%compiler% ./Middlewares/Third_Party/FreeRTOS/Source/timers.c %compiler_args%
%gccpath%\%compiler% ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c %compiler_args%
%gccpath%\%compiler% ./Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c %compiler_args%
%gccpath%\%compiler% ./Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_crc.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi_ex.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c %compiler_args%
%gccpath%\%compiler% ./Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c %compiler_args%
%gccpath%\%compiler% ./Src/crt0.c %compiler_args%
%gccpath%\%compiler% ./Src/freertos.c %compiler_args%
%gccpath%\%compiler% ./Src/stm32f1xx_it.c %compiler_args%
%gccpath%\%compiler% ./Src/stm32f1xx_hal_msp.c %compiler_args%
%gccpath%\%compiler% ./Src/stm32f1xx_hal_timebase_TIM.c %compiler_args%
%gccpath%\%compiler% ./Src/main.c %compiler_args%
%gccpath%\%compiler% ./Src/gfx.c %compiler_args%
%gccpath%\%compiler% ./Src/tft.c %compiler_args%

%gccpath%\%assembler% ./Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103x6.s %assembler_args% -o startup_stm32f103x6.o
 
%gccpath%\%linker% -o main.elf startup_stm32f103x6.o crt0.o gfx.o tft.o freertos.o stm32f1xx_it.o stm32f1xx_hal_msp.o system_stm32f1xx.o stm32f1xx_hal.o croutine.o event_groups.o list.o queue.o tasks.o timers.o cmsis_os.o heap_4.o port.o stm32f1xx_hal_rcc.o stm32f1xx_hal_rcc_ex.o stm32f1xx_hal_crc.o stm32f1xx_hal_cortex.o stm32f1xx_hal_tim.o stm32f1xx_hal_timebase_TIM.o stm32f1xx_hal_tim_ex.o stm32f1xx_hal_dma.o stm32f1xx_hal_pwr.o stm32f1xx_hal_uart.o stm32f1xx_hal_spi.o stm32f1xx_hal_spi_ex.o stm32f1xx_hal_flash.o stm32f1xx_hal_flash_ex.o stm32f1xx_hal_gpio.o stm32f1xx_hal_gpio_ex.o main.o %linker_args%

%gccpath%\%size% main.elf

del *.map
del *.asm
del *.sdf
del *.pwi
del *.dbk
del *.o
del *.d
