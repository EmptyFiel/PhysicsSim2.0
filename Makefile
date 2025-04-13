# Compiler
CXX = g++

# Directories
SRC_DIR = ./src
INCLUDE_DIR = ./include
LIB_DIR = ./lib

# Output
TARGET = main

# Flags
CXXFLAGS = -I$(INCLUDE_DIR)
LDFLAGS = -L$(LIB_DIR) -lm -lglad -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)

# Build target
$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)
