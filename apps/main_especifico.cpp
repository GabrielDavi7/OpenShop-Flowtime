#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <filesystem>
#include <iomanip>
#include <algorithm>
#include <string>
#include "../include/ParserTA.hpp"
#include "../include/Grafo.hpp"

using namespace std;
namespace fs = std::filesystem;

struct Avaliacao {
    int flowtime;
    int makespan;
};

Avaliacao avaliarGrafo(const Instancia& inst, const vector<vector<int>>& seq_maquinas, Grafo& grafo_out) { 
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
    for(int i = 0; i < N; i++){
        int idUltimaOperacao = (i * M) + M;
        int tempo_final = g.vertices[idUltimaOperacao].tempo_termino;
        flowtime += tempo_final;
        if(tempo_final > makespan) {
            makespan = tempo_final;
        }
    }

    grafo_out = g;
    return {flowtime, makespan};
}

struct Resultado {
    int flowtime;
    int makespan;
    Grafo grafo;
};

Resultado otimizarBuscaLocal(const Instancia& inst, double tempo_limite_seg) { 
    int N = inst.num_trabalhos;
    int M = inst.num_maquinas;


    vector<pair<int, int>> custo_jobs(N);
    for(int i = 0; i < N; i++) {
        custo_jobs[i].first = i;
        custo_jobs[i].second = 0;
        for(int j = 0; j < M; j++) custo_jobs[i].second += inst.custos[i][j];
    }
    sort(custo_jobs.begin(), custo_jobs.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second < b.second;
    });

    vector<vector<int>> seq_spt(M, vector<int>(N));
    for(int j = 0; j < M; j++) {
        for(int i = 0; i < N; i++) seq_spt[j][i] = custo_jobs[i].first;
    }


    vector<vector<int>> seq_burra(M, vector<int>(N));
    for(int j = 0; j < M; j++) {
        for(int i = 0; i < N; i++) seq_burra[j][i] = i;
    }

    Grafo grafo_spt(N * M);
    Avaliacao aval_spt = avaliarGrafo(inst, seq_spt, grafo_spt);
    
    Grafo grafo_burro(N * M);
    Avaliacao aval_burra = avaliarGrafo(inst, seq_burra, grafo_burro);

    vector<vector<int>> melhor_seq;
    int melhor_flowtime, melhor_makespan;
    Grafo melhor_grafo(N * M);

    if (aval_burra.flowtime < aval_spt.flowtime) {
        melhor_seq = seq_burra;
        melhor_flowtime = aval_burra.flowtime;
        melhor_makespan = aval_burra.makespan;
        melhor_grafo = grafo_burro;
    } else {
        melhor_seq = seq_spt;
        melhor_flowtime = aval_spt.flowtime;
        melhor_makespan = aval_spt.makespan;
        melhor_grafo = grafo_spt;
    }

    // Variáveis globais para rastrear o melhor absoluto encontrado no tempo
    int global_flowtime = melhor_flowtime;
    int global_makespan = melhor_makespan;
    vector<vector<int>> global_seq = melhor_seq;
    Grafo global_grafo = melhor_grafo;


    random_device rd;
    mt19937 gerador(rd());
    uniform_int_distribution<int> dist_maquina(0, M - 1);
    uniform_int_distribution<int> dist_trabalho(0, N - 1);
    uniform_real_distribution<double> dist_prob(0.0, 1.0);

    int k = 1;
    int k_max = 10;
    int paciencia = 0;
    int max_paciencia = 2000;

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
            if (p1 != p2) {
                // Alternância entre Swap e Inserção
                if (dist_prob(gerador) < 0.5) {
                    swap(nova_seq[m_rand][p1], nova_seq[m_rand][p2]); 
                } else {
                    int job = nova_seq[m_rand][p1];
                    nova_seq[m_rand].erase(nova_seq[m_rand].begin() + p1);
                    nova_seq[m_rand].insert(nova_seq[m_rand].begin() + p2, job);
                }
            }
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
                global_seq = melhor_seq;
                global_grafo = melhor_grafo;
            }
        } 
        else {
            k++; 
            paciencia++;
            if (aval_teste.flowtime == melhor_flowtime) {
                melhor_seq = nova_seq;
                melhor_grafo = grafo_teste;
                melhor_makespan = aval_teste.makespan;
            } 
        }

        if (k > k_max) k = 1;

        if (paciencia > max_paciencia) {
            for(int j = 0; j < M; j++) shuffle(melhor_seq[j].begin(), melhor_seq[j].end(), gerador);
            Avaliacao aval_reset = avaliarGrafo(inst, melhor_seq, melhor_grafo);
            melhor_flowtime = aval_reset.flowtime;
            melhor_makespan = aval_reset.makespan;
            k = 1; 
            paciencia = 0;
        }
    }
    
    return {global_flowtime, global_makespan, global_grafo};
}

int main(int argc, char* argv[]) { 
    setlocale(LC_ALL, "C");

    if (argc < 2) {
        cout << "USO INCORRETO!" << endl;
        cout << "Execute assim: ./seu_programa <caminho_do_arquivo> [tempo_limite_segundos]" << endl;
        cout << "Exemplo: ./main_especifico instancias/ta31Osp.psi 120" << endl;
        return 1;
    }

    string caminhoArquivo = argv[1];
    
    double tempo_limite = 60.0;
    if (argc >= 3) {
        tempo_limite = stod(argv[2]);
    }

    if(!fs::exists(caminhoArquivo) || !fs::is_regular_file(caminhoArquivo)) {
        cout << "ERRO: O arquivo '" << caminhoArquivo << "' nao existe ou e invalido." << endl;
        return 1;
    }

    cout << "\n" << string(70, '=') << "\n";
    cout << "   TESTE INDIVIDUAL - META-HEURISTICA VNS \n";
    cout << string(70, '=') << "\n";
    cout << "Arquivo      : " << caminhoArquivo << "\n";
    cout << "Tempo Limite : " << tempo_limite << " segundos\n";
    cout << string(70, '-') << "\n";

    Instancia inst = ParserTA::lerArquivo(caminhoArquivo);
    if(inst.num_trabalhos == 0 || inst.num_maquinas == 0) {
        cout << "ERRO: Falha ao ler a instancia. Verifique o formato do arquivo.\n";
        return 1;
    }

    vector<vector<int>> seq_base(inst.num_maquinas, vector<int>(inst.num_trabalhos));
    for(int j = 0; j < inst.num_maquinas; j++) {
        for(int i = 0; i < inst.num_trabalhos; i++) seq_base[j][i] = i;
    }
    Grafo grafo_base(1); 
    Avaliacao aval_base = avaliarGrafo(inst, seq_base, grafo_base);

    cout << "[*] Avaliando modelo inicial (Baseline)..." << "\n";
    cout << "    Flowtime: " << aval_base.flowtime << " | Makespan: " << aval_base.makespan << "\n\n";

    cout << "[*] Iniciando motor VNS por " << tempo_limite << " segundos. Aguarde..." << "\n";
    
   
    Resultado res_otimizado = otimizarBuscaLocal(inst, tempo_limite);

    double melhoria_ft = 100.0 - ((double)res_otimizado.flowtime / aval_base.flowtime) * 100.0;
    double melhoria_ms = 100.0 - ((double)res_otimizado.makespan / aval_base.makespan) * 100.0;

    cout << "\n" << string(70, '-') << "\n";
    cout << "   RESULTADOS FINAIS\n";
    cout << string(70, '-') << "\n";
    cout << "Novo Flowtime : " << res_otimizado.flowtime << " (Melhoria de -" << fixed << setprecision(2) << melhoria_ft << "%)\n";
    cout << "Novo Makespan : " << res_otimizado.makespan << " (Melhoria de -" << fixed << setprecision(2) << melhoria_ms << "%)\n";
    cout << string(70, '=') << "\n\n";

    return 0;
}