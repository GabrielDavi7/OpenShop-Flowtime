#include <iostream>
#include "ParserTA.hpp" 
#include "Grafo.hpp"

#include <filesystem>
#include <iomanip> // para usar std::setw para formatar a saída

using namespace std;
namespace fs = std::filesystem;


int flowtimeSequencial(const string& caminhoArquivo){

    Instancia instancia = ParserTA::lerArquivo(caminhoArquivo);
    if(instancia.num_trabalhos == 0 || instancia.num_maquinas == 0) {
        cout << "Falha ao ler a instancia do arquivo." << endl;
        return -1; // Retorna um código de erro
    }

    int n = instancia.num_trabalhos;
    int m = instancia.num_maquinas;

    Grafo grafo(n*m); // Criamos um grafo com n*m vértices, onde cada vértice representa uma operação (trabalho, máquina)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++){
            int idVertice = (i * m) + j + 1; // Calcula o ID do vértice com base no trabalho e máquina  
            grafo.setPeso(idVertice,instancia.custos[i][j]); // Define o peso do vértice com o custo correspondente
        }  
    }

    for(int maquina=0; maquina < m; maquina++){
        for(int trabalho=0; trabalho < n-1; trabalho++){
            int origem = (trabalho * m) + maquina + 1; // ID do vértice da operação atual
            int destino = ((trabalho + 1) * m) + maquina + 1; // ID do vértice da próxima operação na mesma máquina
            grafo.adicionarAresta(origem, destino); // Adiciona uma aresta do vértice atual para o próximo vértice na mesma máquina
        }
    }

    for(int trabalho=0; trabalho < n; trabalho++){
        for(int maquina=0; maquina < m-1; maquina++){
            int origem = (trabalho * m) + maquina + 1; // ID do vértice da operação atual
            int destino = (trabalho * m) + maquina + 2; // ID do vértice da próxima operação no mesmo trabalho
            grafo.adicionarAresta(origem, destino); // Adiciona uma aresta do vértice atual para o próximo vértice no mesmo trabalho
        }
    }

    grafo.caminhadaTopologica();

    int flowtime = 0;
    for(int trabalho=0; trabalho < n; trabalho++){
        int idUltimaOperacao = (trabalho * m) + m; // ID do vértice da última operação de cada trabalho
        int tempoTermino = grafo.vertices[idUltimaOperacao].tempo_termino;
        flowtime += tempoTermino;
    }
    return flowtime;
}


int main() {

    setlocale(LC_ALL, "C");

    string caminhoPasta = "instancias/"; // Caminho para a pasta contendo as instâncias
    cout << "\n======================================================\n";
    cout << "      PROCESSAMENTO EM LOTE - OPEN SHOP FLOWTIME      \n";
    cout << "======================================================\n";
    // Cabeçalho da tabela (setw define a largura da coluna)
    cout << left << setw(30) << "Arquivo da Instancia" << "| " << "Flowtime Inicial\n";
    cout << "------------------------------------------------------\n";

    if(!fs::exists(caminhoPasta) || !fs::is_directory(caminhoPasta)) {
        cout << "A pasta '" << caminhoPasta << "' não existe ou não é um diretório." << endl;
        return 1;
    }

    int arquivos_processados = 0;

    for(const auto& entry: fs::directory_iterator(caminhoPasta)) {
        if(entry.is_regular_file()) {
            string caminhoArquivo = entry.path().string();
            int flowtime = flowtimeSequencial(caminhoArquivo);
            if (flowtime != -1) {
                cout << left << setw(30) << entry.path().filename().string() << "| " << flowtime << endl;
                arquivos_processados++;
            } else {
                cout << left << setw(30) << entry.path().filename().string() << "| ERRO DE LEITURA" << endl;
            }
        }
    }
    cout << "======================================================\n";
    cout << "Total de arquivos processados: " << arquivos_processados << endl;
    cout << "======================================================\n";

    return 0;
}   