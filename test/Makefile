CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -I../core/include -I../hal/include
CORE_SRC = ../core/src/orp.cpp
TEST_SRC = test_orp.cpp

test_orp: $(TEST_SRC) $(CORE_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: test_orp
	./test_orp

clean:
	rm -f test_orp

.PHONY: test clean
