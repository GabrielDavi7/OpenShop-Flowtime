#include <iostream>
#include "ParserTA.hpp" 
#include "Grafo.hpp"

using namespace std;

int main() {

    string caminhoArquivo = "Instancias/ta49Osp.psi";

    Instancia instancia = ParserTA::lerArquivo(caminhoArquivo);

    if(instancia.num_trabalhos == 0 || instancia.num_maquinas == 0) {
        cout << "Falha ao ler a instancia do arquivo." << endl;
        return 1; // Retorna um código de erro
    }

    int n = instancia.num_trabalhos;
    int m = instancia.num_maquinas;
    int total_custo = n*m; // Exemplo de cálculo do custo total, pode ser ajustado conforme necessário
    cout << "Inicializando Grafo com " << total_custo << " vertices..." << endl;

    Grafo grafo(total_custo);
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++)
        {
            int idVertice = (i * m) + j + 1; // Calcula o ID do vértice com base no trabalho e máquina  
            
            int temp = instancia.custos[i][j]; // Obtém o custo do trabalho i na máquina j
            grafo.setPeso(idVertice,temp); // Define o peso do vértice com o custo correspondente
        }  
    }

    cout << "Sucesso! " << total_custo << " operacoes cadastradas no Grafo." << endl;

    cout << "O tempo da ultima operacao (Trabalho " << n-1 << ", Maquina " << m-1 << ") eh: " 
         << grafo.getPeso(total_custo) << endl;
    
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

    cout << "Realizando caminhada topologica para calcular o caminho máximo..." << endl;
    grafo.caminhadaTopologica();

    int flowtime = 0;
    for(int trabalho=0; trabalho < n; trabalho++){
        int idUltimaOperacao = (trabalho * m) + m; // ID do vértice da última operação de cada trabalho
        int tempoTermino = grafo.vertices[idUltimaOperacao].tempo_termino;
        cout << "Tempo de término do Trabalho " << trabalho << ": " << tempoTermino << endl;
        flowtime += tempoTermino;
    }

    cout << "\n=========================================" << endl;
    cout << "FLOWTIME TOTAL (Baseline): " << flowtime << endl;
    cout << "=========================================" << endl;

    return 0;
}   