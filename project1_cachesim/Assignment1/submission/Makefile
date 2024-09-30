CC = g++
CFLAGS = -g --std=c++20 -Wall -Iinclude/
SIM_SRC = src/Main.cpp src/GenCache.cpp src/FullAssocCache.cpp src/Simulator.cpp src/Cache.cpp
BUILD_DIR = build
SIM_OBJ = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SIM_SRC))

all: cache_sim

cache_sim: $(SIM_OBJ)
	$(CC) -o $@ $(CFLAGS) $(SIM_OBJ) -lm
	@echo "Cache Simulator build complete."

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) cache_sim
