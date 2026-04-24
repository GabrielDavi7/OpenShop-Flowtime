#include "Grafo.hpp"
#include <iostream>
#include <fstream> // para ler o arquivo de arestas
    int main(){
        
        Grafo meu_grafo(16); // 16 para comecar o id em 1 e não em 0 preservando oque foi passado no arquivo do trabalho, ou seja, o vertice 1 tem id 1 e não 0, o vertice 15 tem id 15 e não 14

        for(int i = 1; i <15; i++){
            meu_grafo.vertices[i].peso = 1;
        }
        std::ifstream arquivo_arestas("grafo_fixo.txt"); // Grafo fixo do trabalho, tem 16 vertices e as arestas estão definidas no arquivo grafo_fixo.txt

        if (!arquivo_arestas.is_open()) {
            std::cout << "ERRO: Nao foi possivel abrir o arquivo!" << std::endl;
            return 1; 
        }

        int origem, destino;
        while (arquivo_arestas >> origem >> destino) {
            meu_grafo.adicionarAresta(origem, destino);
        }
        arquivo_arestas.close();


        meu_grafo.caminhadaTopologica();

        int maior_tempo = 0;
        int id_maximal = -1;
        
        for (int i = 1; i < 15; i++) {
            if (meu_grafo.vertices[i].tempo_termino > maior_tempo) {
                maior_tempo = meu_grafo.vertices[i].tempo_termino;
                id_maximal = i;
            }
    }

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
