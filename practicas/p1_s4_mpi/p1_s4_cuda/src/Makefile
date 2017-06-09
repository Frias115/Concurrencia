OBJS = main.o
EXE = main
GCC = nvcc   
INCLUDES = -I/usr/lib/openmpi/include -L/usr/lib/openmpi/lib -lcudart 
CFLAGS = -g 
CXXFLAGS =
LDFLAGS = -lmpi

all: $(OBJS)
	$(GCC) $(OBJS) $(LDFLAGS) $(CFLAGS) $(CXXFLAGS) -o $(EXE)
	
main.o: main.cu
	$(GCC) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -c main.cu -o main.o
	
clean:
	rm *.o $(EXE)



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