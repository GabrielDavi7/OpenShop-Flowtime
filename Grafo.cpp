#include Grafo.hpp
#include <iostream>

Grafo::Grafo(int n) { //construtor do grafo, recebe o numero de vertices
    num_vertices = n;

    vertices.resize(n);

    for (int i = 0; i < n; i++) {
        vertices[i].id = i;
    }
}

void adicionarAresta(int origem, int destino) {
    vertices[origem].adjacentes.push_back(destino); // vertice de origem aponta para o vertice de destino
    vertices[destino].grau_entrada++; // vertice de destino tem mais uma aresta apontando para ele aumenta o grau de entrada assim um job com 3 setas apontada pra ele tem grau de entrada 0+3
}
