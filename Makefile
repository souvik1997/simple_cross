UNAME := $(shell uname)

CXX_Linux = g++
CXX_Darwin = $(shell xcrun -sdk macosx -f clang++)
CXXFLAGS_base = -std=c++2b -Wall -Werror -O2 -fsanitize=undefined -fsanitize=address
CXXFLAGS_Darwin = -isysroot $(shell xcrun -sdk macosx -show-sdk-path)
CXXFLAGS_Linux = -fanalyzer

CXX = $(CXX_$(UNAME))
CXXFLAGS = $(CXXFLAGS_base) $(CXXFLAGS_$(UNAME))

# Default rule
all: simple_cross
.PHONY: test

simple_cross: simple_cross.cpp Price.cpp Order.cpp
	$(CXX) $(CXXFLAGS) -o $@  $^
	
test: simple_cross $(wildcard tests/input_*.txt) $(wildcard tests/output_*.txt)
	for input in $(wildcard tests/input_*.txt); do \
		output=$$(echo $$input | sed -e 's/input/output/g'); \
		./simple_cross $$input | diff - $$output || exit 1; \
	done

clean:
	rm -f simple_cross
