################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Display/SDL_gfx.cpp 

OBJS += \
./Display/SDL_gfx.o 

CPP_DEPS += \
./Display/SDL_gfx.d 


# Each subdirectory must supply rules for building sources it contributes
Display/%.o: ../Display/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-10.0/bin/nvcc -G -g -O0 -gencode arch=compute_30,code=sm_30  -odir "Display" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-10.0/bin/nvcc -G -g -O0 --compile  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


