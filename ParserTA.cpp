#include "ParserTA.hpp" 
#include <fstream>
#include <iostream>
#include <string>    

Instancia ParserTA::lerArquivo(const std::string& caminhoArquivo) {
    Instancia instancia;
    std::ifstream arquivo(caminhoArquivo);
    std::string lixo;

    if(!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << caminhoArquivo << std::endl;
        return instancia; // Retorna uma instância vazia em caso de erro
    }

    arquivo>>lixo>>instancia.num_trabalhos>>instancia.num_maquinas; // Lê o número de trabalhos e máquinas, ignorando a palavra "trabalhos" e "maquinas"
    arquivo>>lixo; // pula a palavra "custos"

    instancia.custos.resize(instancia.num_trabalhos, std::vector<int>(instancia.num_maquinas));

    for(int i = 0; i < instancia.num_trabalhos; ++i) {
        for(int j = 0; j < instancia.num_maquinas; ++j) {
           if(!(arquivo >> instancia.custos[i][j])) {
                break; // Sai do loop se não conseguir ler um custo, o que pode indicar um formato de arquivo incorreto
            }
        }
    }
    arquivo.close();
    return instancia;
}