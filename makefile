# Makefile para compilar os arquivos fonte do projeto OpenShop-Flowtime
#   Comandos:
#       "make" - compila todos os executáveis
#       "make run_teste" - executa o programa do grafo fixo
#       "make run_instancias" - executa o programa flowtime de instâncias
#       "make run_otimizacao" - executa o programa de otimização
#       "make run_especifico" - executa o teste individual (exemplo na ta31)
#       "make clean" - remove a pasta bin e os executáveis

CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O3
INC      := -Iinclude

SRC_DIR  := src
APP_DIR  := apps
BIN_DIR  := bin

COMMON_SRCS := $(SRC_DIR)/Grafo.cpp $(SRC_DIR)/ParserTA.cpp

all: $(BIN_DIR)/otimizacao.exe $(BIN_DIR)/teste.exe $(BIN_DIR)/instancias.exe $(BIN_DIR)/caminho.exe $(BIN_DIR)/especifico.exe

$(BIN_DIR)/otimizacao.exe: $(APP_DIR)/main_otimizacao.cpp $(COMMON_SRCS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@

$(BIN_DIR)/teste.exe: $(APP_DIR)/main_teste.cpp $(COMMON_SRCS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@

$(BIN_DIR)/instancias.exe: $(APP_DIR)/main_instancias.cpp $(COMMON_SRCS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@

$(BIN_DIR)/caminho.exe: $(APP_DIR)/main_caminho.cpp $(COMMON_SRCS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@  

$(BIN_DIR)/especifico.exe: $(APP_DIR)/main_especifico.cpp $(COMMON_SRCS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@  

run_otimizacao: $(BIN_DIR)/otimizacao.exe
	@.\$(BIN_DIR)\otimizacao.exe

run_teste: $(BIN_DIR)/teste.exe
	@.\$(BIN_DIR)\teste.exe

run_instancias: $(BIN_DIR)/instancias.exe
	@.\$(BIN_DIR)\instancias.exe

run_caminho: $(BIN_DIR)/caminho.exe
	@.\$(BIN_DIR)\caminho.exe

# Executa o programa com uma instância de exemplo (ta31) e 60 segundos de tempo limite
run_especifico: $(BIN_DIR)/especifico.exe
	@.\$(BIN_DIR)\especifico.exe instancias/ta31Osp.psi 60

clean:
	@if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)
	@echo Limpeza concluida.