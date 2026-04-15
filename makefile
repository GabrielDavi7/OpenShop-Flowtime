# Compilador e Flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Arquivos Objeto
OBJ_COMMON = Grafo.o
OBJ_TESTE = main_teste.o
OBJ_SOLVER = main_instancias.o ParserTA.o

# Alvos principais
all: teste solver

teste: $(OBJ_COMMON) $(OBJ_TESTE)
	$(CXX) $(CXXFLAGS) -o teste_grafo $(OBJ_COMMON) $(OBJ_TESTE)

solver: $(OBJ_COMMON) $(OBJ_SOLVER)
	$(CXX) $(CXXFLAGS) -o solver_instancias $(OBJ_COMMON) $(OBJ_SOLVER)

# Regras de compilação
%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main_teste.o: main_teste.cpp
	$(CXX) $(CXXFLAGS) -c main_teste.cpp

main_instancias.o: main_instancias.cpp
	$(CXX) $(CXXFLAGS) -c main_instancias.cpp

clean:
	rm -f *.o teste_grafo solver_instancias