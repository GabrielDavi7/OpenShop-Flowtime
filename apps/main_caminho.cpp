#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <filesystem>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <mutex>
#include "../include/ParserTA.hpp"
#include "../include/Grafo.hpp"

using namespace std;
namespace fs = std::filesystem;

struct Avaliacao {
    int flowtime;
    int makespan;
    int id_no_makespan; 
};

// Avalia o grafo e retorna flowtime, makespan e o nó final do caminho crítico
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

// Estrutura auxiliar para o processamento paralelo da vizinhança
struct Vizinho {
    vector<vector<int>> seq;
    int job1, job2;
    int flowtime;
    int makespan;
    int id_no_makespan;
    Grafo grafo = Grafo(0);
};

Resultado otimizarBuscaLocal(const Instancia& inst, double tempo_limite_seg) { 
    int N = inst.num_trabalhos;
    int M = inst.num_maquinas;

    // Inicialização (SPT vs Sequencial)
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

    Grafo g_spt(N*M), g_burra(N*M);
    Avaliacao aval_spt = avaliarGrafo(inst, seq_spt, g_spt);
    Avaliacao aval_burra = avaliarGrafo(inst, seq_burra, g_burra);

    vector<vector<int>> seq_atual;
    int best_ft;
    if(aval_spt.flowtime < aval_burra.flowtime) {
        seq_atual = seq_spt;
        best_ft = aval_spt.flowtime;
    } else {
        seq_atual = seq_burra;
        best_ft = aval_burra.flowtime;
    }

    // Variáveis Globais de Retorno
    int global_flowtime = best_ft;
    int global_makespan = (best_ft == aval_spt.flowtime) ? aval_spt.makespan : aval_burra.makespan;
    int global_id_makespan = (best_ft == aval_spt.flowtime) ? aval_spt.id_no_makespan : aval_burra.id_no_makespan;
    Grafo global_grafo = (best_ft == aval_spt.flowtime) ? g_spt : g_burra;

    // CONFIGURAÇÃO TABU E PARALELISMO
    random_device rd;
    mt19937 gerador(rd());
    uniform_int_distribution<int> dist_maquina(0, M - 1);
    uniform_int_distribution<int> dist_trabalho(0, N - 1);
    uniform_real_distribution<double> dist_prob(0.0, 1.0);

    vector<vector<int>> matriz_tabu(N, vector<int>(N, 0));
    int iteracao = 0;
    int tabu_tenure = max(5, N / 2);
    int num_threads = thread::hardware_concurrency();
    if(num_threads == 0) num_threads = 4;
    int num_vizinhos = 100; 

    int paciencia = 0;
    int max_paciencia = 500;

    auto inicio = chrono::high_resolution_clock::now();

    while(true) {
        auto agora = chrono::high_resolution_clock::now();
        if(chrono::duration<double>(agora - inicio).count() > tempo_limite_seg) break;

        // VNS Shake: Diversificação se estagnar
        if (paciencia > max_paciencia) {
            for(int j = 0; j < M; j++) shuffle(seq_atual[j].begin(), seq_atual[j].end(), gerador);
            for(int r = 0; r < N; r++) fill(matriz_tabu[r].begin(), matriz_tabu[r].end(), 0);
            paciencia = 0;
        }

        // Geração de Vizinhança
        vector<Vizinho> vizinhanca(num_vizinhos);
        for(int i = 0; i < num_vizinhos; i++) {
            vizinhanca[i].seq = seq_atual;
            int m = dist_maquina(gerador);
            int p1 = dist_trabalho(gerador);
            int p2 = dist_trabalho(gerador);
            while(p1 == p2) p2 = dist_trabalho(gerador);

            vizinhanca[i].job1 = vizinhanca[i].seq[m][p1];
            vizinhanca[i].job2 = vizinhanca[i].seq[m][p2];

            if(dist_prob(gerador) < 0.5) swap(vizinhanca[i].seq[m][p1], vizinhanca[i].seq[m][p2]);
            else {
                int j1 = vizinhanca[i].seq[m][p1];
                vizinhanca[i].seq[m].erase(vizinhanca[i].seq[m].begin() + p1);
                vizinhanca[i].seq[m].insert(vizinhanca[i].seq[m].begin() + p2, j1);
            }
        }

        // AVALIAÇÃO PARALELA
        vector<thread> worker_threads;
        int chunk = num_vizinhos / num_threads;
        for(int t = 0; t < num_threads; t++) {
            int start = t * chunk;
            int end = (t == num_threads - 1) ? num_vizinhos : (t + 1) * chunk;
            worker_threads.emplace_back([&, start, end]() {
                for(int i = start; i < end; i++) {
                    vizinhanca[i].grafo = Grafo(N * M);
                    Avaliacao a = avaliarGrafo(inst, vizinhanca[i].seq, vizinhanca[i].grafo);
                    vizinhanca[i].flowtime = a.flowtime;
                    vizinhanca[i].makespan = a.makespan;
                    vizinhanca[i].id_no_makespan = a.id_no_makespan;
                }
            });
        }
        for(auto& th : worker_threads) th.join();

        // Escolha do melhor vizinho (Critério Tabu + Aspiração)
        int best_idx = -1;
        int best_ft_viz = 2147483647;

        for(int i = 0; i < num_vizinhos; i++) {
            bool is_tabu = (matriz_tabu[vizinhanca[i].job1][vizinhanca[i].job2] > iteracao);
            
            // Aspiração: Aceita se for melhor que o recorde global
            if(is_tabu && vizinhanca[i].flowtime < global_flowtime) is_tabu = false;

            if(!is_tabu && vizinhanca[i].flowtime < best_ft_viz) {
                best_ft_viz = vizinhanca[i].flowtime;
                best_idx = i;
            }
        }

        if(best_idx != -1) {
            seq_atual = vizinhanca[best_idx].seq;
            matriz_tabu[vizinhanca[best_idx].job1][vizinhanca[best_idx].job2] = iteracao + tabu_tenure;
            matriz_tabu[vizinhanca[best_idx].job2][vizinhanca[best_idx].job1] = iteracao + tabu_tenure;

            if(vizinhanca[best_idx].flowtime < global_flowtime) {
                global_flowtime = vizinhanca[best_idx].flowtime;
                global_makespan = vizinhanca[best_idx].makespan;
                global_id_makespan = vizinhanca[best_idx].id_no_makespan;
                global_grafo = vizinhanca[best_idx].grafo;
                paciencia = 0;
            } else paciencia++;
        } else paciencia++;

        iteracao++;
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

    double tempo_por_instancia = 360; 
    vector<string> instancias_alvo = { // Lista de instâncias específicas para gerar os dados de caminhos críticos, nesse caso estão todas, mas pode ser filtrada para um subconjunto se desejado
        "ta31Osp.psi",
};
/*      "ta01Osp.psi", "ta02Osp.psi", "ta03Osp.psi", "ta04Osp.psi", "ta05Osp.psi",
        "ta06Osp.psi", "ta07Osp.psi", "ta08Osp.psi", "ta09Osp.psi", "ta10Osp.psi",
        "ta11Osp.psi", "ta12Osp.psi", "ta13Osp.psi", "ta14Osp.psi", "ta15Osp.psi",
        "ta16Osp.psi", "ta17Osp.psi", "ta18Osp.psi", "ta19Osp.psi", "ta20Osp.psi",
        "ta21Osp.psi", "ta22Osp.psi", "ta23Osp.psi", "ta24Osp.psi", "ta25Osp.psi",
        "ta26Osp.psi", "ta27Osp.psi", "ta28Osp.psi", "ta29Osp.psi", "ta30Osp.psi",
        "ta31Osp.psi", "ta32Osp.psi", "ta33Osp.psi", "ta34Osp.psi", "ta35Osp.psi",
        "ta36Osp.psi", "ta37Osp.psi", "ta38Osp.psi", "ta39Osp.psi", "ta40Osp.psi",
        "ta41Osp.psi", "ta42Osp.psi", "ta43Osp.psi", "ta44Osp.psi", "ta45Osp.psi",
        "ta46Osp.psi", "ta47Osp.psi", "ta48Osp.psi", "ta49Osp.psi", "ta50Osp.psi",
        "ta51Osp.psi", "ta52Osp.psi", "ta53Osp.psi", "ta54Osp.psi", "ta55Osp.psi",
        "ta56Osp.psi", "ta57Osp.psi", "ta58Osp.psi", "ta59Osp.psi", "ta60Osp.psi",   */
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