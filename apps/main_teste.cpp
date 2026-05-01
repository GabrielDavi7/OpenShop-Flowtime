#include "../include/ParserTA.hpp" 
#include "../include/Grafo.hpp"
#include <iostream>
#include <fstream>
#include <vector>

int main(){
    
    Grafo meu_grafo(16); // 16 para comecar o id em 1 e não em 0


    for(int i = 1; i <= 15; i++){
        meu_grafo.vertices[i].peso = 1; // Definindo o peso de cada vértice como 1
    }

    std::ifstream arquivo_arestas("grafo_fixo.txt"); 

    if (!arquivo_arestas.is_open()) {
        std::cout << "ERRO: Nao foi possivel abrir o arquivo!" << std::endl;
        return 1; 
    }

    int origem, destino;
    while (arquivo_arestas >> origem >> destino) {
        meu_grafo.adicionarAresta(origem, destino);
    }
    arquivo_arestas.close();

    // Agora chamamos apenas UMA vez e já guardamos o resultado:
    std::vector<int> ordem_percorrida = meu_grafo.caminhadaTopologica();

    int maior_tempo = 0;
    int id_maximal = -1;
    
    // CORREÇÃO 3: i <= 15 (para o nó 15 também poder ser o maximal)
    for (int i = 1; i <= 15; i++) {
        if (meu_grafo.vertices[i].tempo_termino > maior_tempo) {
            maior_tempo = meu_grafo.vertices[i].tempo_termino;
            id_maximal = i;
        }
    }

    std::cout << "Ordem da Caminhada Topologica: ";
    for(int v : ordem_percorrida) {
        std::cout << v << " ";
    }
    std::cout << "\n\n";

    std::cout << "Teste do Grafo fixo: Caminho maximo geral (Minimal ate Maximal)" << std::endl;
    if (id_maximal != -1) {
        meu_grafo.imprimirCaminhoMaximo(id_maximal);
    }
    std::cout << "\n--------------------------------------------------\n" << std::endl;

    std::cout << "Teste do Grafo fixo: Caminhos para o final de cada linha" << std::endl;
    
    std::cout << "Linha Superior - No 6:" << std::endl;
    meu_grafo.imprimirCaminhoMaximo(6);
    std::cout << std::endl;

    std::cout << "Linha do Meio - No 13:" << std::endl;
    meu_grafo.imprimirCaminhoMaximo(13);
    std::cout << std::endl;

    std::cout << "Linha Inferior - No 9:" << std::endl;
    meu_grafo.imprimirCaminhoMaximo(9);
    std::cout << std::endl;

    return 0;
}