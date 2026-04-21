#include <iostream>
#include "ParserTA.hpp" 

using namespace std;

int main() {

    string caminhoArquivo = "Instancias/ta49Osp.psi";
    cout << "teste de leitura da instancia do arquivo: " << caminhoArquivo << endl;

    Instancia instancia = ParserTA::lerArquivo(caminhoArquivo);

    if(instancia.num_trabalhos == 0 || instancia.num_maquinas == 0) {
        cout << "Falha ao ler a instancia do arquivo." << endl;
        return 1; // Retorna um código de erro
    }

    cout << "Numero de trabalhos: " << instancia.num_trabalhos << endl;
    cout << "Numero de maquinas: " << instancia.num_maquinas << endl;

    cout << "Matriz de custos:" << endl;
    for(int i = 0; i < instancia.num_trabalhos; ++i) {
        for(int j = 0; j < instancia.num_maquinas; ++j) {
            cout << instancia.custos[i][j] << "\t";
        }
        cout << endl;
    }

    cout<< "teste concluido com sucesso!" << endl;
    return 0;
}   