#include "../include/Grafo.hpp"
#include <iostream>
#include <vector>

int main() {
    Grafo meu_grafo(16); // Criamos um grafo com 16 vértices (1 a 15, index 0 não é usado)

    for (int i = 1; i <= 15; i++) {
        meu_grafo.vertices[i].peso = i;
    }

    const std::vector<std::pair<int, int>> arestas_fixas = { // (origem, destino)
        {4, 3}, {4, 1}, {10, 3}, {10, 1}, {7, 10}, {7, 8}, 
        {8, 14}, {8, 2}, {1, 11}, {1, 2}, {3, 5}, {11, 15}, 
        {11, 14}, {14, 13}, {14, 5}, {5, 15}, {2, 12}, 
        {13, 6}, {15, 6}, {15, 9}, {12, 13}, {12, 9}
    };

    for (const auto& aresta : arestas_fixas) {
        meu_grafo.adicionarAresta(aresta.first, aresta.second);
    }

    std::vector<int> ordem_percorrida = meu_grafo.caminhadaTopologica();

    int maior_tempo = 0;
    int id_maximal = -1;
    for (int i = 1; i <= 15; i++) {
        if (meu_grafo.vertices[i].tempo_termino > maior_tempo) {
            maior_tempo = meu_grafo.vertices[i].tempo_termino;
            id_maximal = i;
        }
    }

    std::cout << "Ordem da Caminhada Topologica: ";
    for (int v : ordem_percorrida) std::cout << v << " ";
    std::cout << "\n\n";

    std::cout << "Teste do Grafo fixo: Caminho maximo geral (Minimal ate Maximal)\n";
    if (id_maximal != -1) meu_grafo.imprimirCaminhoMaximo(id_maximal);
    
    std::cout << "\n--------------------------------------------------\n";
    std::cout << "Teste do Grafo fixo: Caminhos para o final de cada linha\n";
    
    std::cout << "Linha Superior - No 6:\n"; meu_grafo.imprimirCaminhoMaximo(6);
    std::cout << "\nLinha do Meio - No 13:\n"; meu_grafo.imprimirCaminhoMaximo(13);
    std::cout << "\nLinha Inferior - No 9:\n"; meu_grafo.imprimirCaminhoMaximo(9);

    return 0;
}