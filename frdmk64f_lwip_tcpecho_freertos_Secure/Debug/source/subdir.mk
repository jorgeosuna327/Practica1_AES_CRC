################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/aes.c \
../source/aes_crc_processing.c \
../source/lwip_tcpecho_freertos.c \
../source/semihost_hardfault.c 

OBJS += \
./source/aes.o \
./source/aes_crc_processing.o \
./source/lwip_tcpecho_freertos.o \
./source/semihost_hardfault.o 

C_DEPS += \
./source/aes.d \
./source/aes_crc_processing.d \
./source/lwip_tcpecho_freertos.d \
./source/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DUSE_RTOS=1 -DPRINTF_ADVANCED_ENABLE=1 -DFRDM_K64F -DFREEDOM -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DSERIAL_PORT_TYPE_UART=1 -DSDK_OS_FREE_RTOS -DMCUXPRESSO_SDK -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\board" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\source" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\mdio" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\phy" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\lwip\contrib\apps\tcpecho" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\lwip\port" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\lwip\src" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\lwip\src\include" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\drivers" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\utilities" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\device" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\component\uart" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\component\serial_manager" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\component\lists" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\CMSIS" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\freertos\freertos_kernel\include" -I"C:\Users\jorge\Documents\MCUXpressoIDE_11.4.0_6237\workspace\frdmk64f_lwip_tcpecho_freertos_Secure\freertos\freertos_kernel\portable\GCC\ARM_CM4F" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


