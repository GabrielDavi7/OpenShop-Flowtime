#ifndef ParserTA_hpp
#define ParserTA_hpp

#include <string>
#include <vector>

struct Instancia{
    int num_trabalhos;
    int num_maquinas;
    std::vector<std::vector<int>> custos; // Matriz de custos, onde custos[i][j] representa o custo de alocar o trabalho i na máquina j
};

class ParserTA{
    public:
        static Instancia lerArquivo(const std::string& caminhoArquivo);
};

#endif 