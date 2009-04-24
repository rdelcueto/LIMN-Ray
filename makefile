################################################################################
# LIMN-Ray's MAKEFILE
################################################################################

RM := rm -rf

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
./bin/blas.o \
./bin/lights.o \
./bin/limn-ray.o \
./bin/materials.o \
./bin/parser.o \
./bin/primitives.o \
./bin/rays.o \
./bin/scene.o 

CPP_DEPS += \
./bin/blas.d \
./bin/lights.d \
./bin/limn-ray.d \
./bin/materials.d \
./bin/parser.d \
./bin/primitives.d \
./bin/rays.d \
./bin/scene.d 

# Each subdirectory must supply rules for building sources it contributes
bin/%.o: ./src/%.cpp
	@mkdir -p ./bin ./deps
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -fsingle-precision-constant -Wall -c -fmessage-length=0 `freetype-config --cflags` -fopenmp -mfpmath=sse -msse3 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

# All Target
all: limn-ray

# Tool invocations
limn-ray: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -lgomp -lxerces-c -lpng -lpngwriter -lz -lfreetype -o"limn-ray" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) limn-ray
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
