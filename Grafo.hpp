#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <vector>

struct Vertice{
    int id; 
    int peso; 
    int grau_entrada; // Numero de aretas que apontam para o vertice
    int temp_termino;   //tempo acumulado para o termino do vertice
    int predecessor_maximo;  // id do vertice predecessor que tem o maior tempo acumulado para o termino
    std::vector<int> adjacentes; // uma lista que mostra para quem o vertice aponta

    Vertice(){
        id = 0;
        peso = 0;
        grau_entrada = 0;
        temp_termino = 0;
        predecessor_maximo = -1; // -1 indica que não tem predecessor
    }
};

#endif