CC := g++
CFLAGS := -std=c++1y -Wall -g

LIB := -L./
INC := -I./

LDLIBS := -lgsl -lcblas

SRCS := $(wildcard BlockSim/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

all: strat selfish

strat: StratSim/main.cpp $(OBJS)
	$(CC) $(INC) $(LDLIBS) -o $@ $^

selfish: SelfishSim/main.cpp $(OBJS)
	$(CC) $(INC) $(LDLIBS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<

clean:
	rm -rf BlockSim/*.o *.o strat selfish

.PHONY: all clean

