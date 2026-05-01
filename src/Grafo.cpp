#include "../include/Grafo.hpp"
#include <iostream>
#include <queue> // fila para o algoritmo de ordenação topológica usando algoritmo de Kahn
#include <algorithm> // para usar a função reverse

Grafo::Grafo(int n) { //construtor do grafo, recebe o numero de vertices
    num_vertices = n;

    vertices.resize(n+1); // +1 para ajustar o índice do vetor, pois os IDs começam em 1

    for(int i = 1; i <= n; i++){
        vertices[i].id = i;
    }
}

void Grafo::adicionarAresta(int origem, int destino) {
    vertices[origem].adjacentes.push_back(destino); // vertice de origem aponta para o vertice de destino
    vertices[destino].grau_entrada++; // vertice de destino tem mais uma aresta apontando para ele aumenta o grau de entrada assim um job com 3 setas apontada pra ele tem grau de entrada 0+3
}

std::vector<int> Grafo::caminhadaTopologica(){ //com algoritimo de kahn
    std::queue<int> fila;
    std::vector <int> ordem;

    for(int i=0;  i < num_vertices; i++){
        if(vertices[i].grau_entrada == 0){
            fila.push(i); // se o vertice tem grau de entrada 0, ou seja, não tem dependências
            vertices[i].tempo_termino = vertices[i].peso; // o tempo de termino do vertice é igual ao seu peso, pois ele pode ser processado imediatamente
        }
    }

    while(!fila.empty()){
        int atual = fila.front(); // pega o vertice da frente da fila
        fila.pop(); // remove o vertice da fila
        ordem.push_back(atual); // adiciona o vertice na ordem topologica

        for(int vizinhos : vertices[atual].adjacentes){
            int tempo_passando_aqui = vertices[atual].tempo_termino + vertices[vizinhos].peso;

            if(tempo_passando_aqui > vertices[vizinhos].tempo_termino){ // se o tempo passando por aqui for maior que o tempo de termino atual do vizinho, atualiza o tempo de termino do vizinho
                vertices[vizinhos].tempo_termino = tempo_passando_aqui;
                vertices[vizinhos].predecessor_maximo = atual;
            }
            vertices[vizinhos].grau_entrada--; // diminui o grau de entrada do vizinho, pois uma das dependências foi processada
            
            if(vertices[vizinhos].grau_entrada == 0){ //se não tem mais dependências, ou seja, grau de entrada 0, pode ser processado adicionamos a fila
                fila.push(vizinhos); 
            }
        }
    }
    return ordem;
}

void Grafo::imprimirCaminhoMaximo(int destino) {
    if (destino < 0 || destino > num_vertices) {
        std::cout << "Destino invalido." << std::endl;
        return;
    }

    std::vector<int> caminho; // vetor para armazenar o caminho máximo
    int atual = destino; // ultima maquina do job, ou seja, o destino

    while (atual != -1) { // enquanto não chegar no início do caminho
        caminho.push_back(atual); // adiciona o vertice atual ao caminho
        atual = vertices[atual].predecessor_maximo; // anda pra tras para o predecessor do vertice atual, ou seja, a maquina anterior do job
    }

    std::reverse(caminho.begin(), caminho.end()); // inverte o caminho para mostrar do início ao destino

    std::cout << "Caminho maximo para o destino " << destino << ": ";
    for (int vertice : caminho) {
        std::cout << vertice << " ";
    }
    std::cout << "\nTempo total (contando vertices): " << vertices[destino].tempo_termino << std::endl; // o tempo total é o tempo de termino do destino, que já conta o peso do destino, ou seja, o tempo de processamento da ultima maquina do job
    std::cout << "Tempo total (contando arestas): " << (vertices[destino].tempo_termino - 1) << std::endl; // o tempo total contando arestas é o tempo total contando vertices menos 1, pois o tempo de termino do destino conta o peso do destino, ou seja, o tempo de processamento da ultima maquina do job, e para contar apenas as arestas, devemos subtrair esse peso
}

void Grafo::setPeso(int idVertice, int peso) {
    if (idVertice < 1 || idVertice > num_vertices) {
        std::cout << "ID do vértice invalido." << std::endl;
        return;
    }
    vertices[idVertice].peso = peso;
}

int Grafo::getPeso(int idVertice) {
    if (idVertice < 1 || idVertice > num_vertices) {
        std::cout << "ID do vértice invalido." << std::endl;
        return 0; // Retorna um valor inválido para indicar erro
    }
    return vertices[idVertice].peso;
}