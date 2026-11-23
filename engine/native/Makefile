# Núcleo C++ determinístico (agro_core). Sem dependência do Godot.
#   make test                      compila e roda os testes com $(CXX)
#   make vectors                   gera build/<cfg>/detmath_vectors.txt (hash na última linha)
#   make CXX=clang++ OPT=-O0 test  outra configuração
#   make windows                   compila os testes e os vetores para Windows (MinGW-w64)
# As flags de ponto flutuante são parte do contrato de determinismo: sem contração em FMA,
# sem fast-math, double em SSE2 (o código recusa FLT_EVAL_METHOD != 0).

CXX      ?= g++
OPT      ?= -O2
ARCH     ?=
CFG      ?= $(notdir $(firstword $(CXX)))$(OPT)$(subst =,-,$(ARCH))
FPFLAGS  := -ffp-contract=off -fno-fast-math
CXXFLAGS := -std=c++17 $(OPT) $(ARCH) $(FPFLAGS) -Wall -Wextra -Iinclude
LDFLAGS  ?=
EXE      ?=
BUILD    := build/$(CFG)

SRC   := $(sort $(wildcard src/*.cpp))
TESTS := $(sort $(wildcard tests/*.cpp))

.PHONY: test vectors windows clean

$(BUILD)/run_tests$(EXE): $(SRC) $(TESTS) $(wildcard include/agro/*.hpp tests/*.hpp)
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $(SRC) $(TESTS) -o $@ $(LDFLAGS)

$(BUILD)/detmath_vectors$(EXE): src/detmath.cpp tools/detmath_vectors.cpp include/agro/detmath.hpp
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) src/detmath.cpp tools/detmath_vectors.cpp -o $@ $(LDFLAGS)

test: $(BUILD)/run_tests$(EXE)
	./$(BUILD)/run_tests$(EXE)

vectors: $(BUILD)/detmath_vectors$(EXE)
	./$(BUILD)/detmath_vectors$(EXE) > $(BUILD)/detmath_vectors.txt
	@tail -1 $(BUILD)/detmath_vectors.txt

windows:
	$(MAKE) CXX=x86_64-w64-mingw32-g++ EXE=.exe LDFLAGS=-static CFG=mingw$(OPT) \
	  build/mingw$(OPT)/run_tests.exe build/mingw$(OPT)/detmath_vectors.exe

clean:
	rm -rf build
