OBJS = practica2Cuda.o
EXE = multiplicadorMatrices
GCC = nvcc
INCLUDES = -I.
CXXFLAGS = -g
all: $(OBJS)
	$(GCC) $(OBJS) $(LDFLAGS) $(CXXFLAGS) -o $(EXE)

%.o: %.cu
	$(GCC) -c $< $(CXXFLAGS) -o $@

clean:
	rm *.o $(EXE)

////////////////////////////////////////////////////////////////////

objects = main.o particle.o v3.o

all: $(objects)
    nvcc -arch=sm_20 $(objects) -o app

%.o: %.cpp
    nvcc -x cu -arch=sm_20 -I. -dc $< -o $@

clean:
    rm -f *.o app