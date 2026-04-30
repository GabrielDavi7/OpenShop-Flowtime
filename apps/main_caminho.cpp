#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <filesystem>
#include <iomanip>
#include <algorithm>
#include "../include/ParserTA.hpp"
#include "../include/Grafo.hpp"

using namespace std;
namespace fs = std::filesystem;

struct Avaliacao {
    int flowtime;
    int makespan;
    int id_no_makespan; 
};

Avaliacao avaliarGrafo(const Instancia& inst, const vector<vector<int>>& seq_maquinas, Grafo& grafo_out) { // Avalia o grafo gerado a partir da sequência de máquinas e retorna o flowtime, makespan e o ID do vértice que define o makespan
    int N = inst.num_trabalhos;
    int M = inst.num_maquinas;
    Grafo g(N * M);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++){
            int idVertice = (i * M) + j + 1;
            g.setPeso(idVertice, inst.custos[i][j]);
        }
    }

    for(int i = 0; i < N; i++){
        for(int j = 0; j < M - 1; j++){
            int origem = (i * M) + j + 1;
            int destino = (i * M) + j + 2;
            g.adicionarAresta(origem, destino);
        }
    }

    for(int j = 0; j < M; j++){
        for(int i = 0; i < N - 1; i++){
            int trabalho_atual = seq_maquinas[j][i];
            int trabalho_prox = seq_maquinas[j][i+1];
            int origem = (trabalho_atual * M) + j + 1;
            int destino = (trabalho_prox * M) + j + 1;
            g.adicionarAresta(origem, destino);
        }
    }

    g.caminhadaTopologica();

    int flowtime = 0;
    int makespan = 0;
    int id_no_makespan = -1;

    for(int i = 0; i < N; i++){
        int idUltimaOperacao = (i * M) + M;
        int tempo_final = g.vertices[idUltimaOperacao].tempo_termino;
        flowtime += tempo_final;
        if(tempo_final > makespan) {
            makespan = tempo_final;
            id_no_makespan = idUltimaOperacao;
        }
    }

    grafo_out = g;
    return {flowtime, makespan, id_no_makespan};
}

struct Resultado {
    int flowtime;
    int makespan;
    int id_no_makespan;
    Grafo grafo;
};

Resultado otimizarBuscaLocal(const Instancia& inst, double tempo_limite_seg) { // Otimiza a sequência de máquinas usando uma busca local do tipo VNS e retorna o melhor flowtime, makespan, ID do vértice do makespan e grafo encontrado
    int N = inst.num_trabalhos;
    int M = inst.num_maquinas;

    vector<vector<int>> melhor_seq(M, vector<int>(N));
    for(int j = 0; j < M; j++) {
        for(int i = 0; i < N; i++) melhor_seq[j][i] = i;
    }

    Grafo melhor_grafo(N * M);
    Avaliacao aval_inicial = avaliarGrafo(inst, melhor_seq, melhor_grafo);
    
    int melhor_flowtime = aval_inicial.flowtime;
    int melhor_makespan = aval_inicial.makespan;

    int global_flowtime = melhor_flowtime;
    int global_makespan = melhor_makespan;
    int global_id_makespan = aval_inicial.id_no_makespan;
    vector<vector<int>> global_seq = melhor_seq;
    Grafo global_grafo = melhor_grafo;

    random_device rd;
    mt19937 gerador(rd());
    uniform_int_distribution<int> dist_maquina(0, M - 1);
    uniform_int_distribution<int> dist_trabalho(0, N - 1);

    int k = 1;
    int k_max = 10;
    int paciencia = 0;
    int max_paciencia = 10000;

    auto inicio = chrono::high_resolution_clock::now();

    while(true) {
        auto agora = chrono::high_resolution_clock::now();
        chrono::duration<double> tempo_passado = agora - inicio;
        
        if(tempo_passado.count() > tempo_limite_seg) break;

        vector<vector<int>> nova_seq = melhor_seq;
        
        for(int s = 0; s < k; s++) {
            int m_rand = dist_maquina(gerador); 
            int p1 = dist_trabalho(gerador);    
            int p2 = dist_trabalho(gerador);    
            swap(nova_seq[m_rand][p1], nova_seq[m_rand][p2]); 
        }

        Grafo grafo_teste(N * M);
        Avaliacao aval_teste = avaliarGrafo(inst, nova_seq, grafo_teste);

        if(aval_teste.flowtime < melhor_flowtime) { 
            melhor_flowtime = aval_teste.flowtime;
            melhor_makespan = aval_teste.makespan;
            melhor_seq = nova_seq;
            melhor_grafo = grafo_teste;
            k = 1; 
            paciencia = 0;

            if (melhor_flowtime < global_flowtime) {
                global_flowtime = melhor_flowtime;
                global_makespan = melhor_makespan;
                global_id_makespan = aval_teste.id_no_makespan;
                global_seq = melhor_seq;
                global_grafo = melhor_grafo;
            }
        } 
        else if (aval_teste.flowtime == melhor_flowtime) {
            melhor_seq = nova_seq;
            melhor_grafo = grafo_teste;
            melhor_makespan = aval_teste.makespan;
            k++;
            paciencia++;
        } 
        else {
            k++;
            paciencia++;
        }

        if (k > k_max) k = 1;

        if (paciencia > max_paciencia) {
            for(int j = 0; j < M; j++) {
                std::shuffle(melhor_seq[j].begin(), melhor_seq[j].end(), gerador);
            }
            Avaliacao aval_reset = avaliarGrafo(inst, melhor_seq, melhor_grafo);
            melhor_flowtime = aval_reset.flowtime;
            melhor_makespan = aval_reset.makespan;
            k = 1;
            paciencia = 0;
        }
    }
    
    return {global_flowtime, global_makespan, global_id_makespan, global_grafo};
}

int main() { // Função principal que coordena o processamento em lote das instâncias para gerar dados de caminhos críticos
    setlocale(LC_ALL, "C");

    string caminhoPasta = "instancias/"; 
    
    if(!fs::exists(caminhoPasta) || !fs::is_directory(caminhoPasta)) {
        cout << "A pasta '" << caminhoPasta << "' nao existe ou nao e um diretorio." << endl;
        return 1;
    }

    double tempo_por_instancia = 20.0; // 20 segundos por arquivo

    vector<string> instancias_alvo = {
        "ta01Osp.psi", "ta02Osp.psi", "ta05Osp.psi", "ta10Osp.psi", "ta20Osp.psi"
    };

    cout << "\n================================================================================\n";
    cout << "      GERADOR DE DADOS PARA SLIDES - CAMINHOS CRITICOS (" << tempo_por_instancia << "s/arq)      \n";
    cout << "================================================================================\n";

    for(const auto& entry: fs::directory_iterator(caminhoPasta)) {
        if(entry.is_regular_file()) {
            string nomeArquivo = entry.path().filename().string();
            
            if (find(instancias_alvo.begin(), instancias_alvo.end(), nomeArquivo) == instancias_alvo.end()) {
                continue; 
            }

            string caminhoArquivo = entry.path().string();
            Instancia inst = ParserTA::lerArquivo(caminhoArquivo);

            vector<vector<int>> seq_base(inst.num_maquinas, vector<int>(inst.num_trabalhos));
            for(int j = 0; j < inst.num_maquinas; j++) {
                for(int i = 0; i < inst.num_trabalhos; i++) seq_base[j][i] = i;
            }
            Grafo grafo_base(1); 
            Avaliacao aval_base = avaliarGrafo(inst, seq_base, grafo_base);

            Resultado res_otimizado = otimizarBuscaLocal(inst, tempo_por_instancia);

            double melhoria_ft = 100.0 - ((double)res_otimizado.flowtime / aval_base.flowtime) * 100.0;
            double melhoria_ms = 100.0 - ((double)res_otimizado.makespan / aval_base.makespan) * 100.0;

            cout << "\n[ INSTANCIA: " << nomeArquivo << " ]\n";
            cout << "--------------------------------------------------------------------------------\n";
            cout << "Flowtime : " << aval_base.flowtime << " -> " << res_otimizado.flowtime 
                 << " (-" << fixed << setprecision(2) << melhoria_ft << "%)\n";
            cout << "Makespan : " << aval_base.makespan << " -> " << res_otimizado.makespan 
                 << " (-" << fixed << setprecision(2) << melhoria_ms << "%)\n\n";
            
            cout << "=> CAMINHO CRITICO ORIGINAL (Baseline):\n";
            grafo_base.imprimirCaminhoMaximo(aval_base.id_no_makespan);
            
            cout << "\n=> CAMINHO CRITICO OTIMIZADO (VNS):\n";
            res_otimizado.grafo.imprimirCaminhoMaximo(res_otimizado.id_no_makespan);
            
            cout << "\n================================================================================\n";
        }
    }
    
    cout << "Geracao de log para relatorio concluida." << endl;
    return 0;
}