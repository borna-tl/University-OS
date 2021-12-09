CC = g++
BUILD_DIR = build
SRC_DIR = src

INCLUDE_DIR = includes
CFLAGS = -std=c++17 -pthread -Wall -Werror -I$(INCLUDE_DIR)

REDUCE_EXECUTABLE_FILE = reduce.out
MAPS_EXECUTABLE_FILE = maps.out
MAIN_EXECUTABLE_FILE = main.out

REDUCE_OBJECTS = \
	$(BUILD_DIR)/reduce.o 

MAPS_OBJECTS = \
	$(BUILD_DIR)/maps.o

MAIN_OBJECTS = \
	$(BUILD_DIR)/main.o

ReduceSensitivityList = \
	$(SRC_DIR)/reduce.cpp \
	$(INCLUDE_DIR)/defines.hpp

MapsensitivityList = \
	$(SRC_DIR)/maps.cpp \
	$(INCLUDE_DIR)/defines.hpp

MainsensitivityList = \
	$(SRC_DIR)/main.cpp \
	$(INCLUDE_DIR)/defines.hpp

all: $(BUILD_DIR) $(REDUCE_EXECUTABLE_FILE) $(MAPS_EXECUTABLE_FILE) $(MAIN_EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/maps.o: $(MapsensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/maps.cpp -o $(BUILD_DIR)/maps.o

$(BUILD_DIR)/reduce.o: $(ReduceSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/reduce.cpp -o $(BUILD_DIR)/reduce.o

$(BUILD_DIR)/main.o: $(MainSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

$(MAPS_EXECUTABLE_FILE): $(MAPS_OBJECTS)
	$(CC) $(CFLAGS) $(MAPS_OBJECTS) -o $(MAPS_EXECUTABLE_FILE)

$(REDUCE_EXECUTABLE_FILE): $(REDUCE_OBJECTS)
	$(CC) $(CFLAGS) $(REDUCE_OBJECTS) -o $(REDUCE_EXECUTABLE_FILE)

$(MAIN_EXECUTABLE_FILE): $(MAIN_OBJECTS)
	$(CC) $(CFLAGS) $(MAIN_OBJECTS) -o $(MAIN_EXECUTABLE_FILE)

.PHONY: clean
clean:
	rm -rf *.out\
	rm -rf $(BUILD_DIR) *.o *.out \
	rmdir -rf systems \
	rmdir -rf pipes/*