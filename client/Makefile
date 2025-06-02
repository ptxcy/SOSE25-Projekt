CC = g++

SDST_CORE = core/
SDST_SCRIPT = script/
LDST = lib/
TARGET = build

INCLUDE = -I/usr/include/freetype2 -I/usr/include/libpng16
LINKER = -lGL -lGLEW -lSDL2 -lpthread -lfreetype -lcpr -lcurl -lssl -lcrypto -lz

DEBUG_SUFFIX = -pg -g -O0 -DDEBUG
RELEASE_SUFFIX = -O3 -fno-gcse

SRCS_CORE = $(wildcard $(SDST_CORE)*.cpp)
SRCS_SCRIPT = $(wildcard $(SDST_SCRIPT)*.cpp)
SRCS = $(SRCS_CORE) $(SRCS_SCRIPT)
OBJS = $(SRCS:$(SDST)%.cpp=$(LDST)%.o)
MAIN = main.cpp

CXXFLAGS =


all: $(TARGET)

debug: CXXFLAGS = $(DEBUG_SUFFIX)
debug: $(TARGET)

release: CXXFLAGS = $(RELEASE_SUFFIX)
release: $(TARGET)

$(TARGET): $(OBJS) $(MAIN)
	@mkdir -p $(dir $@)
	$(CC) $(MAIN) $(OBJS) -o $@ $(INCLUDE) $(LINKER) $(CXXFLAGS)

$(LDST)%.o: $(SDST)%.cpp $(SDST)%.h
	@mkdir -p $(dir $@)
	$(CC) $< -o $@ -c $(INCLUDE) $(CXXFLAGS)
