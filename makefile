CXX = g++
CXXFLAGS = -fopenmp -I include

SRCS = src/oort.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = oort

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
