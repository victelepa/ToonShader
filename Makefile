CXX ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra

SRC = main.cpp
HDR = params.h image.h obj.h raster.h toon.h
BIN = toonobj

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

clean:
	rm -f $(BIN) output.ppm
