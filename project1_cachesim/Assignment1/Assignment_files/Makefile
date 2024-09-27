CC = g++
OPT = -O3
VER = --std=c++20
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(VER) $(WARN) $(INC) $(LIB)

# List all your .cpp files here (source files, excluding header files)
SIM_SRC = Main.cpp GenCache.cpp FullAssocCache.cpp Simulator.cpp Cache.cpp

# List corresponding compiled object files here (.o files)
SIM_OBJ = $(SIM_SRC:.cpp=.o)

#################################

# default rule

cacti: cacti_test
	$(CC) -o cacti_test $(CFLAGS) cacti.cpp Parse.h

all: cache_sim
	@echo "my work is done here..."


# rule for making cache_sim

cache_sim: $(SIM_OBJ)
	$(CC) -o cache_sim $(CFLAGS) $(SIM_OBJ) -lm
	@echo "-----------DONE WITH CACHE_SIM-----------"


# generic rule for converting any .cpp file to any .o file
 
.cpp.o:
	$(CC) $(CFLAGS) -c $<


# type "make clean" to remove all .o files plus the cache_sim binary

clean:
	rm -f *.o cache_sim


# type "make clobber" to remove all .o files (leaves cache_sim binary)

clobber:
	rm -f *.o
