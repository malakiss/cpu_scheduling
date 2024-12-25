CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = lab6
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
all: $(TARGET)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

run:
	./$(TARGET) < 10b-input.txt

.PHONY: all clean run
