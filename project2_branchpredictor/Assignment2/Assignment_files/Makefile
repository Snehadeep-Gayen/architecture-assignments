CC = g++
CFLAGS = -Wall -Wextra -Wpedantic

# Use the correct extension of your source file here
SIM_SRC = branchpredictor.cpp main.cpp simulator.cpp
SIM_OBJ = $(SIM_SRC:.cpp=.o)

EXEC = bpsim

all: $(EXEC)

$(EXEC): $(SIM_OBJ)
	$(CC) -o $(EXEC) $(CFLAGS) $(SIM_OBJ)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)
