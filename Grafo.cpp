#include "Grafo.hpp"
#include <iostream>
#include <queue> // fila para o algoritmo de ordenação topológica usando algoritmo de Kahn

Grafo::Grafo(int n) { //construtor do grafo, recebe o numero de vertices
    num_vertices = n;

    vertices.resize(n);

    for (int i = 0; i < n; i++) {
        vertices[i].id = i;
    }
}

void Grafo::adicionarAresta(int origem, int destino) {
    vertices[origem].adjacentes.push_back(destino); // vertice de origem aponta para o vertice de destino
    vertices[destino].grau_entrada++; // vertice de destino tem mais uma aresta apontando para ele aumenta o grau de entrada assim um job com 3 setas apontada pra ele tem grau de entrada 0+3
}

void Grafo::caminhadaTopologica(){ //com algoritimo de kahn
    std::queue<int> fila; 

    for(int i=0;  i < num_vertices; i++){
        if(vertices[i].grau_entrada == 0){
            fila.push(i); // se o vertice tem grau de entrada 0, ou seja, não tem dependências
            vertices[i].tempo_termino = vertices[i].peso; // o tempo de termino do vertice é igual ao seu peso, pois ele pode ser processado imediatamente
        }
    }

    while(!fila.empty()){
        int atual = fila.front(); // pega o vertice da frente da fila
        fila.pop(); // remove o vertice da fila

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
}