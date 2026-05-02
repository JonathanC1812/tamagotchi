CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g
TARGET   = tamagotchi
SRCS     = main.cpp pet.cpp events.cpp actions.cpp items.cpp fileio.cpp ui.cpp
OBJS     = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
	@echo "Build complete. Run: ./$(TARGET)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
