CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -I../core/include -I../hal/include
CORE_SRC = ../core/src/orp.cpp ../core/src/tokenizer.cpp

test_orp: test_orp.cpp ../core/src/orp.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_tokenizer: test_tokenizer.cpp $(CORE_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: test_orp test_tokenizer
	./test_orp
	./test_tokenizer

clean:
	rm -f test_orp test_tokenizer

.PHONY: test clean
