OBJS = main.o
EXE = main
GCC = /usr/local/cuda/bin/nvcc   
INCLUDES = -I/usr/lib/openmpi/include -L/usr/lib/openmpi/lib -L/usr/local/cuda/lib64 -lcudart 
CFLAGS = -g -G -D_DEBUG
CXXFLAGS =
LDFLAGS = -lmpi

all: $(OBJS)
	$(GCC) $(OBJS) $(LDFLAGS) $(CFLAGS) $(CXXFLAGS) -o $(EXE)
	
main.o: main.cu
	$(GCC) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -c main.cu -o main.o
	
clean:
	rm *.o $(EXE)