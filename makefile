CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

TARGET_TESTE = teste_grafo.exe
TARGET_SOLVER = solver_instancias.exe

OBJ_COMMON = Grafo.o
OBJ_TESTE = main_teste.o
OBJ_SOLVER = main_instancias.o ParserTA.o

all: teste solver

teste: $(OBJ_COMMON) $(OBJ_TESTE)
	$(CXX) $(CXXFLAGS) -o $(TARGET_TESTE) $(OBJ_COMMON) $(OBJ_TESTE)

solver: $(OBJ_COMMON) $(OBJ_SOLVER)
	$(CXX) $(CXXFLAGS) -o $(TARGET_SOLVER) $(OBJ_COMMON) $(OBJ_SOLVER)

run: teste
	.\$(TARGET_TESTE)

run_solver: solver
	.\$(TARGET_SOLVER)

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main_teste.o: main_teste.cpp
	$(CXX) $(CXXFLAGS) -c main_teste.cpp

main_instancias.o: main_instancias.cpp
	$(CXX) $(CXXFLAGS) -c main_instancias.cpp

ParserTA.o: ParserTA.cpp ParserTA.hpp
	$(CXX) $(CXXFLAGS) -c ParserTA.cpp

clean:
	@echo Limpando arquivos...
	del /f /q *.o $(TARGET_TESTE) $(TARGET_SOLVER) 2>nul || rm -f *.o $(TARGET_TESTE) $(TARGET_SOLVER)