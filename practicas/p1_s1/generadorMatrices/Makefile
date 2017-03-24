OBJS = main.o matriz.o
EXE = generadorMatrices
GCC = g++
INCLUDES = -I.
CFLAGS = -g
CXXFLAGS =
all: $(OBJS)
	$(GCC) $(OBJS) $(LDFLAGS) $(CFLAGS) $(CXXFLAGS) -o $(EXE)

%.o: %.cpp
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm *.o $(EXE)