#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <filesystem>
#include <iomanip>
#include <algorithm>
#include "../include/ParserTA.hpp"
#include "../include/Grafo.hpp"

#include <thread>
#include <mutex>

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
    vector<vector<int>> seq_burra(M, vector<int>(N));
    for(int j = 0; j < M; j++) {
        for(int i = 0; i < N; i++) {
            seq_spt[j][i] = custo_jobs[i].first;
            seq_burra[j][i] = i;
        }
    }

    Grafo grafo_spt(N * M);
    Avaliacao aval_spt = avaliarGrafo(inst, seq_spt, grafo_spt);
    Grafo grafo_burro(N * M);
    Avaliacao aval_burra = avaliarGrafo(inst, seq_burra, grafo_burro);

    vector<vector<int>> global_seq;
    int global_flowtime, global_makespan;
    Grafo global_grafo(N * M);

    if (aval_burra.flowtime < aval_spt.flowtime) {
        global_seq = seq_burra;
        global_flowtime = aval_burra.flowtime;
        global_makespan = aval_burra.makespan;
        global_grafo = grafo_burro;
    } else {
        global_seq = seq_spt;
        global_flowtime = aval_spt.flowtime;
        global_makespan = aval_spt.makespan;
        global_grafo = grafo_spt;
    }


    random_device rd;
    mt19937 gerador(rd());
    uniform_int_distribution<int> dist_maquina(0, M - 1);
    uniform_int_distribution<int> dist_trabalho(0, N - 1);
    uniform_real_distribution<double> dist_prob(0.0, 1.0);

    vector<vector<int>> matriz_tabu(N, vector<int>(N, 0));
    int iteracao_atual = 0;
    int tabu_tenure = max(5, N / 2);

    int num_threads = thread::hardware_concurrency(); 
    if (num_threads == 0) num_threads = 12; 
    int num_vizinhos = 100; 

    struct Vizinho {
        vector<vector<int>> seq;
        int job1, job2;
        int flowtime;
        int makespan;
        Grafo grafo = Grafo(0);
    };

    vector<vector<int>> seq_atual = global_seq; // Inicia a busca a partir da melhor inicial
    int paciencia = 0;
    int max_paciencia = 500; 

    auto inicio = chrono::high_resolution_clock::now();

    while(true) {
        auto agora = chrono::high_resolution_clock::now();
        chrono::duration<double> tempo_passado = agora - inicio;
        if(tempo_passado.count() > tempo_limite_seg) break;

        if (paciencia > max_paciencia) {
            for(int j = 0; j < M; j++) shuffle(seq_atual[j].begin(), seq_atual[j].end(), gerador);
            for(int r = 0; r < N; r++) fill(matriz_tabu[r].begin(), matriz_tabu[r].end(), 0);
            paciencia = 0;
        }

        vector<Vizinho> vizinhanca(num_vizinhos);
        for(int i = 0; i < num_vizinhos; i++) {
            vizinhanca[i].seq = seq_atual;
            int m_rand = dist_maquina(gerador); 
            int p1 = dist_trabalho(gerador);    
            int p2 = dist_trabalho(gerador);
            while(p1 == p2) p2 = dist_trabalho(gerador);

            vizinhanca[i].job1 = vizinhanca[i].seq[m_rand][p1];
            vizinhanca[i].job2 = vizinhanca[i].seq[m_rand][p2];

            if (dist_prob(gerador) < 0.5) {
                swap(vizinhanca[i].seq[m_rand][p1], vizinhanca[i].seq[m_rand][p2]); 
            } else {
                int job = vizinhanca[i].seq[m_rand][p1];
                vizinhanca[i].seq[m_rand].erase(vizinhanca[i].seq[m_rand].begin() + p1);
                vizinhanca[i].seq[m_rand].insert(vizinhanca[i].seq[m_rand].begin() + p2, job);
            }
        }

        vector<thread> threads;
        int chunk = num_vizinhos / num_threads;

        for(int t = 0; t < num_threads; t++) {
            threads.emplace_back([&, t]() {
                int inicio_chunk = t * chunk;
                int fim_chunk = (t == num_threads - 1) ? num_vizinhos : (t + 1) * chunk;
                
                for(int i = inicio_chunk; i < fim_chunk; i++) {
                    vizinhanca[i].grafo = Grafo(N * M);
                    Avaliacao aval = avaliarGrafo(inst, vizinhanca[i].seq, vizinhanca[i].grafo);
                    vizinhanca[i].flowtime = aval.flowtime;
                    vizinhanca[i].makespan = aval.makespan;
                }
            });
        }
        
        for(auto& th : threads) th.join();

        int melhor_idx = -1;
        int melhor_ft_vizinhanca = 2147483647; 

        for(int i = 0; i < num_vizinhos; i++) {
            int j1 = vizinhanca[i].job1;
            int j2 = vizinhanca[i].job2;
            
            bool is_tabu = (matriz_tabu[j1][j2] > iteracao_atual || matriz_tabu[j2][j1] > iteracao_atual);

            if (is_tabu && vizinhanca[i].flowtime < global_flowtime) {
                is_tabu = false; 
            }

            if (!is_tabu && vizinhanca[i].flowtime < melhor_ft_vizinhanca) {
                melhor_ft_vizinhanca = vizinhanca[i].flowtime;
                melhor_idx = i;
            }
        }

        if (melhor_idx != -1) {
            seq_atual = vizinhanca[melhor_idx].seq;
            int j1 = vizinhanca[melhor_idx].job1;
            int j2 = vizinhanca[melhor_idx].job2;
            
            matriz_tabu[j1][j2] = iteracao_atual + tabu_tenure;
            matriz_tabu[j2][j1] = iteracao_atual + tabu_tenure;

            if (melhor_ft_vizinhanca < global_flowtime) {
                global_flowtime = vizinhanca[melhor_idx].flowtime;
                global_makespan = vizinhanca[melhor_idx].makespan;
                global_seq = vizinhanca[melhor_idx].seq;
                global_grafo = vizinhanca[melhor_idx].grafo;
                paciencia = 0;
            } else {
                paciencia++;
            }
        } else {
            paciencia++;
        }

        iteracao_atual++;
    }
    
    return {global_flowtime, global_makespan, global_grafo};
}

int main() { 
    setlocale(LC_ALL, "C");

    string caminhoPasta = "instancias/"; 
    
    if(!fs::exists(caminhoPasta) || !fs::is_directory(caminhoPasta)) {
        cout << "A pasta '" << caminhoPasta << "' nao existe ou nao e um diretorio." << endl;
        return 1;
    }

    double tempo_por_instancia = 60.0; 

    cout << "\n" << string(95, '=') << "\n";
    cout << "      PROCESSAMENTO EM LOTE - BUSCA TABU PARALELA (" << tempo_por_instancia << "s/arq)      \n";
    cout << string(95, '=') << "\n";
    cout << left << setw(18) << "Instancia" 
         << "| " << setw(11) << "FT Base" 
         << "| " << setw(11) << "FT Otim" 
         << "| " << setw(14) << "Melhoria FT"
         << "| " << setw(11) << "MS Base"
         << "| " << "MS Otim\n";
    cout << string(95, '-') << "\n";

    int arquivos_processados = 0;

    for(const auto& entry: fs::directory_iterator(caminhoPasta)) {
        if(entry.is_regular_file()) {
            string caminhoArquivo = entry.path().string();
            string nomeArquivo = entry.path().filename().string();
            
            Instancia inst = ParserTA::lerArquivo(caminhoArquivo);
            if(inst.num_trabalhos == 0 || inst.num_maquinas == 0) {
                cout << left << setw(18) << nomeArquivo << "| ERRO DE LEITURA\n";
                continue;
            }

            vector<vector<int>> seq_base(inst.num_maquinas, vector<int>(inst.num_trabalhos));
            for(int j = 0; j < inst.num_maquinas; j++) {
                for(int i = 0; i < inst.num_trabalhos; i++) seq_base[j][i] = i;
            }
            Grafo grafo_base(1); 
            Avaliacao aval_base = avaliarGrafo(inst, seq_base, grafo_base);

            Resultado res_otimizado = otimizarBuscaLocal(inst, tempo_por_instancia);

            double melhoria = 100.0 - ((double)res_otimizado.flowtime / aval_base.flowtime) * 100.0;

            cout << left << setw(18) << nomeArquivo 
                 << "| " << setw(11) << aval_base.flowtime 
                 << "| " << setw(11) << res_otimizado.flowtime 
                 << "| -" << fixed << setprecision(2) << setw(11) << melhoria << "%" 
                 << "| " << setw(11) << aval_base.makespan
                 << "| " << res_otimizado.makespan << "\n";

            arquivos_processados++;
        }
    }
    
    cout << string(95, '=') << "\n";
    cout << "Total de arquivos processados: " << arquivos_processados << endl;
    cout << string(95, '=') << "\n";

    return 0;
}