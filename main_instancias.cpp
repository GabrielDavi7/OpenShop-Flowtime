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
    
    return 0;
}   