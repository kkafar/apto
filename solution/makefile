.TARGET: all

SRC_PATH = src/main/cpp
BIN_PATH = build/bin
OBJ_PATH = build/obj

CXX = g++
CFLAGS = --std=c++14 -Wall
LFLAGS = --std=c++14 -Wall

SOURCES = $(wildcard $(SRC_PATH)/*.cpp)
OBJECTS = $(addprefix $(OBJ_PATH)/,$(patsubst %.cpp,%.o,$(notdir $(SOURCES))))
BIN_NAME = main

all: build

build: $(OBJECTS)
	$(CXX) -o $(BIN_PATH)/$(BIN_NAME) $(LFLAGS) $^

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(CXX) -o $(OBJ_PATH)/$(notdir $@) -c $^ $(CFLAGS)

run: build
	cat data/base-case.in | ./$(BIN_PATH)/$(BIN_NAME)

clean:
	rm -fv $(OBJ_PATH)/* $(BIN_PATH)/*

clean_obj:
	rm -fv $(OBJ_PATH)/*
