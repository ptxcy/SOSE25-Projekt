CC = g++

SDST = core/
LDST = core/lib/
TARGET = build

LINKER = -lGL -lGLEW -lSDL2

DEBUG_SUFFIX = -pg -g -O0 -DDEBUG
RELEASE_SUFFIX = -O3 -fno-gcse

SRCS = $(wildcard $(SDST)*.cpp)
OBJS = $(SRCS:$(SDST)%.cpp=$(LDST)%.o)
MAIN = main.cpp

CXXFLAGS =


all: $(TARGET)

debug: CXXFLAGS = $(DEBUG_SUFFIX)
debug: $(TARGET)

release: CXXFLAGS = $(RELEASE_SUFFIX)
release: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(MAIN) $(OBJS) -o $@ $(LINKER) $(CXXFLAGS)

$(LDST)%.o: $(SDST)%.cpp $(SDST)%.h
	$(CC) $< -o $@ -c $(CXXFLAGS)
