OBJS = main.o matriz.o
EXE = multiplicadorMatrices
GCC = g++
INCLUDES = -I.
CXXFLAGS = -g
#linkado
LDFLAGS = -lpthread
all: $(OBJS)
	$(GCC) $(OBJS) $(LDFLAGS) $(CXXFLAGS) -o $(EXE)

%.o: %.cpp
	$(CC) -c $< $(CXXFLAGS) -o $@

clean:
	rm *.o $(EXE)
