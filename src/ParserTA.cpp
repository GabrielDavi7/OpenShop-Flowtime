#include "../include/ParserTA.hpp"
#include <fstream>
#include <iostream>
#include <string>    

Instancia ParserTA::lerArquivo(const std::string& caminhoArquivo) {
    
    Instancia instancia;
    instancia.num_trabalhos = 0;
    instancia.num_maquinas = 0;

    std::ifstream arquivo(caminhoArquivo);
    std::string palavra;

    if(!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << caminhoArquivo << std::endl;
        return instancia;
    }

    // Fica lendo o arquivo até achar "TotalJobs:"
    while (arquivo >> palavra) {
        if (palavra == "TotalJobs:") {
            arquivo >> instancia.num_trabalhos; // O próximo item é o número
            break; 
        }
    }

    // Continua lendo até achar "TotalMachines:"
    while (arquivo >> palavra) {
        if (palavra == "TotalMachines:") {
            arquivo >> instancia.num_maquinas; // O próximo item é o número
            break; 
        }
    }

    // Continua lendo até achar "Costs:"
    while (arquivo >> palavra) {
        if (palavra == "Costs:") {
            break; 
        }
    }

    // Le a matriz apenas se os números de trabalhos e máquinas forem válidos
    if (instancia.num_trabalhos > 0 && instancia.num_maquinas > 0) {
        instancia.custos.resize(instancia.num_trabalhos, std::vector<int>(instancia.num_maquinas));

        for(int i = 0; i < instancia.num_trabalhos; ++i) {
            for(int j = 0; j < instancia.num_maquinas; ++j) {
               if(!(arquivo >> instancia.custos[i][j])) {
                   break;
                }
            }
        }
    }

    arquivo.close();
    return instancia;
}