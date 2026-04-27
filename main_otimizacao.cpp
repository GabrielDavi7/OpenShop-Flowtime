#include <iostream>
#include <vector>
#include <chrono>   // Para o cronômetro de precisão
#include <random>   // Para embaralhar as filas (Busca Local)
#include <filesystem>
#include <iomanip>
#include "ParserTA.hpp"
#include "Grafo.hpp"

using namespace std;
namespace fs = std::filesystem;

// =========================================================================
// O JUIZ: Constrói o grafo com a ordem que o Gerador inventou e avalia
// =========================================================================
int avaliarGrafo(const Instancia& inst, const vector<vector<int>>& seq_maquinas, Grafo& grafo_out) {
    int N = inst.num_trabalhos;
    int M = inst.num_maquinas;
    Grafo g(N * M);

    // Injeta os pesos (Tempos das operações)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++){
            int idVertice = (i * M) + j + 1;
            g.setPeso(idVertice, inst.custos[i][j]);
        }
    }

    // 1. Arestas Fixas de Trabalhos (Evita a trapaça do zero absoluto!)
    // Obrigamos os trabalhos a andarem M0 -> M1 -> M2...
    // Isso transforma o problema em um Non-Permutation Flow Shop perfeito.
    for(int i=0; i < N; i++){
        for(int j=0; j < M-1; j++){
            int origem = (i * M) + j + 1;
            int destino = (i * M) + j + 2;
            g.adicionarAresta(origem, destino);
        }
    }

    // 2. Arestas Dinâmicas de Máquinas (Baseadas no DNA embaralhado!)
    // O computador diz a ordem que cada máquina atende os clientes
    for(int j=0; j < M; j++){
        for(int i=0; i < N-1; i++){
            int trabalho_atual = seq_maquinas[j][i];
            int trabalho_prox = seq_maquinas[j][i+1];
            int origem = (trabalho_atual * M) + j + 1;
            int destino = (trabalho_prox * M) + j + 1;
            g.adicionarAresta(origem, destino);
        }
    }

    g.caminhadaTopologica();

    int flowtime = 0;
    for(int i=0; i < N; i++){
        int idUltimaOperacao = (i * M) + M;
        flowtime += g.vertices[idUltimaOperacao].tempo_termino;
    }

    grafo_out = g; // Guarda o grafo processado
    return flowtime;
}

// Estrutura para facilitar o retorno das duas coisas juntas
struct Resultado {
    int flowtime;
    Grafo grafo;
};

// =========================================================================
// O GERADOR: Faz a Busca Local usando SWAP Aleatório por X segundos
// =========================================================================
Resultado otimizarBuscaLocal(const Instancia& inst, double tempo_limite_seg) {
    int N = inst.num_trabalhos;
    int M = inst.num_maquinas;

    // Cria a Solução Inicial (Baseline: 0, 1, 2...)
    vector<vector<int>> melhor_seq(M, vector<int>(N));
    for(int j=0; j<M; j++) {
        for(int i=0; i<N; i++) melhor_seq[j][i] = i;
    }

    Grafo melhor_grafo(N * M);
    int melhor_flowtime = avaliarGrafo(inst, melhor_seq, melhor_grafo);

    // Preparando o motor de aleatoriedade
    random_device rd;
    mt19937 gerador(rd());
    uniform_int_distribution<int> dist_maquina(0, M - 1);
    uniform_int_distribution<int> dist_trabalho(0, N - 1);

    // DISPARA O CRONÔMETRO!
    auto inicio = chrono::high_resolution_clock::now();
    int iteracoes = 0;

    while(true) {
        auto agora = chrono::high_resolution_clock::now();
        chrono::duration<double> tempo_passado = agora - inicio;
        
        // Verifica se o tempo acabou
        if(tempo_passado.count() > tempo_limite_seg) break;

        // Pega a nossa melhor fábrica até agora e faz uma Mutação nela
        vector<vector<int>> nova_seq = melhor_seq;
        
        // MUTAÇÃO AGRESSIVA: 3 trocas seguidas para "pular" o morro
        for(int s = 0; s < 3; s++) {
            int m_rand = dist_maquina(gerador); 
            int p1 = dist_trabalho(gerador);    
            int p2 = dist_trabalho(gerador);    
            swap(nova_seq[m_rand][p1], nova_seq[m_rand][p2]); 
        }

        // Manda pro juiz avaliar a mutação
        Grafo grafo_teste(N * M);
        int flowtime_teste = avaliarGrafo(inst, nova_seq, grafo_teste);

        // Se o tempo diminuiu ou empatou, achamos um caminho melhor!
        if(flowtime_teste <= melhor_flowtime) {
            melhor_flowtime = flowtime_teste;
            melhor_seq = nova_seq;
            melhor_grafo = grafo_teste;
        }
        iteracoes++;
    }
    
    return {melhor_flowtime, melhor_grafo};
}

int main() {
    setlocale(LC_ALL, "C");

    string arquivo_ta01 = "instancias/ta01Osp.psi";
    if(!fs::exists(arquivo_ta01)) {
        cout << "Nao achei a instancia ta01!" << endl;
        return 1;
    }

    Instancia inst = ParserTA::lerArquivo(arquivo_ta01);

    cout << "\n======================================================\n";
    cout << "     COMPARACAO: BASELINE VS BUSCA LOCAL (TA01)       \n";
    cout << "======================================================\n\n";

    // 1. DADOS DO BASELINE (Fila Indiana sem otimização)
    cout << "--- 1. SOLUCAO INICIAL (BASELINE) ---\n";
    Grafo grafo_base(1);
    vector<vector<int>> seq_base(inst.num_maquinas, vector<int>(inst.num_trabalhos));
    for(int j=0; j<inst.num_maquinas; j++) for(int i=0; i<inst.num_trabalhos; i++) seq_base[j][i] = i;
    
    int flowtime_base = avaliarGrafo(inst, seq_base, grafo_base);
    
    cout << "Flowtime Inicial: " << flowtime_base << "\n";
    cout << "Ordem (Top 20): ";
    vector<int> ordem_base = grafo_base.caminhadaTopologica();
    for(size_t i=0; i < min(ordem_base.size(), (size_t)20); i++) cout << ordem_base[i] << " ";
    cout << "...\n";
    
    int ultima_op_base = (inst.num_trabalhos * inst.num_maquinas); // Última do último trabalho
    grafo_base.imprimirCaminhoMaximo(ultima_op_base);
    cout << "\n";


    // 2. DADOS OTIMIZADOS (10 Segundo de Busca)
    cout << "--- 2. SOLUCAO OTIMIZADA (10 SEGUNDO DE BUSCA) ---\n";
    cout << "Embaralhando filas milhares de vezes. Aguarde...\n\n";
    
    // Passa 10.0 segundo de limite
    Resultado res_otimizado = otimizarBuscaLocal(inst, 10.0); 
    
    cout << "Flowtime Otimizado: " << res_otimizado.flowtime << "\n";
    
    // Calcula a porcentagem de melhoria
    double melhoria = 100.0 - ((double)res_otimizado.flowtime / flowtime_base) * 100.0;
    cout << "Melhoria do Sistema: -" << fixed << setprecision(2) << melhoria << "%\n";

    cout << "Nova Ordem Otimizada (Top 20): ";
    vector<int> nova_ordem = res_otimizado.grafo.caminhadaTopologica();
    for(size_t i=0; i < min(nova_ordem.size(), (size_t)20); i++) cout << nova_ordem[i] << " ";
    cout << "...\n";

    res_otimizado.grafo.imprimirCaminhoMaximo(ultima_op_base);
    cout << "\n======================================================\n";

    return 0;
}