################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
./src/blas.cpp \
./src/lights.cpp \
./src/limn-ray.cpp \
./src/materials.cpp \
./src/parser.cpp \
./src/primitives.cpp \
./src/rays.cpp \
./src/scene.cpp 

OBJS += \
./src/blas.o \
./src/lights.o \
./src/limn-ray.o \
./src/materials.o \
./src/parser.o \
./src/primitives.o \
./src/rays.o \
./src/scene.o 

CPP_DEPS += \
./src/blas.d \
./src/lights.d \
./src/limn-ray.d \
./src/materials.d \
./src/parser.d \
./src/primitives.d \
./src/rays.d \
./src/scene.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ./src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -ftree-vectorize -msse2 -ftree-vectorizer-verbose=5 -ffast-math -fsingle-precision-constant -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
