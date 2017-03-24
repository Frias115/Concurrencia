OBJS = main.o matriz.o
EXE = multiplicadorMatrices
GCC = g++
INCLUDES = -I.
CXXFLAGS = -g -pg
all: $(OBJS)
	$(GCC) $(OBJS) $(LDFLAGS) $(CXXFLAGS) -o $(EXE)

%.o: %.cpp
	$(CC) -c $< $(CXXFLAGS) -o $@

clean:
	rm *.o $(EXE)