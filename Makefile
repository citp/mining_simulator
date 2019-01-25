CPP := g++
CPPFLAGS := -std=c++14 -Wall -g

LIB := -L./
INC := -I./

SRCS := $(wildcard BlockSim/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

STRAT_SRCS := $(wildcard StratSim/*.cpp)
STRAT_OBJS := $(patsubst %.cpp,%.o,$(STRAT_SRCS))

all: strat selfish

strat: $(STRAT_SRCS) $(OBJS) 
	$(CPP) $(CPPFLAGS) `gsl-config --cflags` $(INC) -o $@ $^ `gsl-config --libs`

selfish: SelfishSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS) `gsl-config --cflags` $(INC) -o $@ $^ `gsl-config --libs`

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(INC) -o $@ -c $<

clean:
	rm -rf BlockSim/*.o *.o strat selfish

.PHONY: all clean

