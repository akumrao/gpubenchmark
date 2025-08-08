# Compiler and Flags
# make sure to do  export PATH=/usr/local/google/home/aumrao/Android/Sdk/ndk/29.0.13846066/toolchains/llvm/prebuilt/linux-x86_64/bin/:$PATH  or run ./combile.sh
CC = aarch64-linux-android33-clang

CXX = aarch64-linux-android33-clang++

CFLAGS = -Wall -g -Iinc -Icommon -Iapp

CXXFLAGS = -Wall -g -Wno-deprecated -Iinc 


# Directories
SRC_DIR = src
LIB_DIR = ./commonLib
OBJ_DIR = obj
BIN_DIR = ./bin

# Target Executable
TARGET = libs $(BIN_DIR)/gpu_001

# Default Rule (Builds the executable)
all: $(BIN_DIR) $(TARGET)

# Source and Object Files
SRC := $(wildcard $(SRC_DIR)/*.c) $(wildcard app/*.c) $(wildcard common/*.c)
LIB_SRC := $(wildcard $(LIB_DIR)/$(SRC_DIR)/gpu_*.c)

OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
LIB_OBJ := $(patsubst $(LIB_DIR)/$(SRC_DIR)/%.c, $(LIB_DIR)/$(OBJ_DIR)/%.o, $(LIB_SRC))

# Dependency Rule for Object Files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependency Rule for LIB Object Files
$(LIB_DIR)/$(OBJ_DIR)/%.o: $(LIB_DIR)/$(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependency Rule for Executable
$(TARGET): $(OBJ) $(LIB_OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIB_OBJ) -o $@ -Wl,-Bstatic ./libjson.a  -lstdc++ -Wl,-Bdynamic -llog

$(BIN_DIR):
	@mkdir -p $(OBJ_DIR) $(LIB_DIR)/$(OBJ_DIR) $@

# Clean Rule
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR) $(LIB_DIR)/$(OBJ_DIR) libjson.a json.o


json.o: json/json.c
	$(CXX) $(CXXFLAGS) -O -c json/json.c

libjson.a: json.o 
	ar rcs libjson.a json.o

libs: libjson.a