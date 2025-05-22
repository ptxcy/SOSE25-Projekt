CC = g++

SDST = core/
LDST = core/lib/
TARGET = build

LINKER = -lGL -lGLEW -lSDL2 -lpthread

DEBUG_SUFFIX = -pg -g -O0 -DDEBUG
RELEASE_SUFFIX = -O3 -fno-gcse
SETTINGS_SUFFIX = -DFRAME_RESOLUTION_X=1600 -DFRAME_RESOLUTION_Y=900 -DFEAT_MULTIPLAYER

SRCS = $(wildcard $(SDST)*.cpp)
OBJS = $(SRCS:$(SDST)%.cpp=$(LDST)%.o)
MAIN = main.cpp

CXXFLAGS =


all: $(TARGET)

debug: CXXFLAGS = $(DEBUG_SUFFIX) $(SETTINGS_SUFFIX)
debug: $(TARGET)

release: CXXFLAGS = $(RELEASE_SUFFIX) $(SETTINGS_SUFFIX)
release: $(TARGET)

$(TARGET): $(OBJS) $(MAIN)
	$(CC) $(MAIN) $(OBJS) -o $@ $(LINKER) $(CXXFLAGS)

$(LDST)%.o: $(SDST)%.cpp $(SDST)%.h
	$(CC) $< -o $@ -c $(CXXFLAGS)
