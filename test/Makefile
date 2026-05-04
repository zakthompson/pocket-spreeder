CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -I../core/include -I../hal/include
CORE_SRC = ../core/src/orp.cpp ../core/src/tokenizer.cpp ../core/src/reader.cpp

test_orp: test_orp.cpp ../core/src/orp.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_tokenizer: test_tokenizer.cpp ../core/src/orp.cpp ../core/src/tokenizer.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test_reader: test_reader.cpp $(CORE_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: test_orp test_tokenizer test_reader
	./test_orp
	./test_tokenizer
	./test_reader

clean:
	rm -f test_orp test_tokenizer test_reader

.PHONY: test clean
