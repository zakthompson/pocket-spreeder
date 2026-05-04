EMCC = em++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra \
           -I../core/include -I../hal/include

CORE_SRCS = ../core/src/orp.cpp \
            ../core/src/tokenizer.cpp \
            ../core/src/reader.cpp

WEB_SRCS = src/display_web.cpp \
           src/input_web.cpp \
           src/storage_web.cpp \
           src/main_web.cpp

LDFLAGS = -sWASM=1 \
          -sALLOW_MEMORY_GROWTH=1 \
          -sEXPORTED_RUNTIME_METHODS='["UTF8ToString"]' \
          --shell-file shell_template.html

all: build/index.html

build/index.html: $(CORE_SRCS) $(WEB_SRCS) shell_template.html
	mkdir -p build
	$(EMCC) $(CXXFLAGS) $(LDFLAGS) $(CORE_SRCS) $(WEB_SRCS) -o build/index.html
	cp shell.js build/shell.js

serve: build/index.html
	cd build && python3 -m http.server 8080

clean:
	rm -rf build

.PHONY: all serve clean
