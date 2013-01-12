#######################
# LIMNRAY v1.1 MAKEFILE
#######################

RM := rm -rf

CPP_FLAGS = -Wall -m64 -mfpmath=sse -Ofast -flto -funroll-loops -march=native -ftree-vectorize -ftree-vectorizer-verbose=9 -fdump-tree-vect-stats -fmessage-length=0 `Magick++-config --cppflags --cxxflags`
#CPP_FLAGS = -Wextra -O0 -g3 -p -pg `Magick++-config --cppflags --cxxflags`

LIBS = -lgomp -lxerces-c `Magick++-config --ldflags --libs`

OBJS = \
./src/blas.o \
./src/lights.o \
./src/limn-ray.o \
./src/materials.o \
./src/parser.o \
./src/primitives.o \
./src/rays.o \
./src/scene.o

all: limn-ray

src/%.o: ./src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" $(CPP_FLAGS)
	@echo 'Finished building: $<'
	@echo ' '

limn-ray: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -o "limn-ray" $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

doc: Doxyfile
	@echo 'Building target: $@'
	@echo 'Invoking: Doxygen'
	doxygen
	
clean:
	-$(RM) $(OBJS) limn-ray Doc
	-rm ./src/*.d ./src/*.vect

.PHONY: all clean
