SRCPP := $(wildcard src/*.cpp)
OBJS := $(SRCPP:.cpp=.o)
DEPS := $(SRCPP:.cpp=.d)
CXXFLAGS := -O3 -Wall -Wextra -Wunused-function -std=c++20 -pg


all: main

debug: CXXFLAGS := -g -Wall -Wextra -Wunused-function -std=c++20
debug: main
	
main: $(OBJS)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $< -c -o $@ $(CXXFLAGS) -MMD -MF $*.d

.PHONY: clean mrproper

clean:
	$(RM) $(DEPS)
	$(RM) $(OBJS)

mrproper: clean
	$(RM) main

-include $(DEPS)
