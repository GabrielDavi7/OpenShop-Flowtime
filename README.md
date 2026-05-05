# 🧩 Open Shop Scheduling: Parallel Flowtime Optimizer

Este projeto implementa um motor de otimização para o problema de escalonamento **Open Shop Scheduling Problem (OSSP)**, com foco na minimização do **Flowtime total (∑Cj)**. 

A solução utiliza uma arquitetura híbrida de metaheurísticas com execução paralela para superar as barreiras de estagnação documentadas na literatura clássica.

---

## 🎯 Problema e Complexidade

O Open Shop Scheduling consiste em um conjunto de jobs que devem ser processados em múltiplas máquinas. Cada job executa cada operação exatamente uma vez, sem ordem tecnológica fixa.

### Fundamentação Científica:
- **NP-Completo:** De acordo com **Gonzalez e Sahni (1976)**, o problema de Open Shop não-preemptivo para mais de duas máquinas ($m > 2$) é NP-Completo. Isso justifica a aplicação de metaheurísticas em vez de algoritmos exatos para instâncias de Taillard.
- **Gargalo de Simetria ($n=m$):** Pesquisas de **Bräsel et al. (2008)** demonstram que instâncias "quadradas" (mesmo número de máquinas e jobs) são as mais desafiadoras, pois o conflito de recursos é máximo. O motor deste projeto foi desenhado especificamente para quebrar esses impasses.

---

## 🚀 Tecnologias e Arquitetura

- **Linguagem:** C++17
- **Paralelismo:** `std::thread` (12 threads simultâneas)
- **Otimização:** Busca Tabu Paralela + Variable Neighborhood Search (VNS)
- **Estrutura de dados:** Grafos direcionados acíclicos (DAG)
- **Ordenação:** Algoritmo de Kahn (topological sort)
- **Build System:** Makefile com otimizações `-O3 -pthread`

---

## 🧠 Estratégia de Otimização e Desenvolvimento

O sistema evoluiu através de etapas incrementais para superar o estado da arte:

### 1. Inicialização (Baseline + SPT)
O algoritmo utiliza a heurística **SPT (Shortest Processing Time)** como ponto de partida, conforme recomendado por **Bräsel (2008)** para minimização de Flowtime.

### 2. VNS com Estratégia de "Paciência 2000"
Implementamos um framework **Variable Neighborhood Search** com um gatilho de diversificação agressivo: caso o sistema realize 2.000 iterações sem melhoria no Flowtime, um movimento de *Shaking* é disparado para explorar novas regiões do grafo.

### 3. Busca Tabu e Memória
Integração de uma lista tabu para evitar ciclos viciosos e impedir que o algoritmo retorne a soluções subótimas recentemente exploradas.

### 4. Paralelismo Massivo (Superação de Estagnação)
Enquanto a literatura clássica (**Andresen et al., 2008**) relata que algoritmos sequenciais estagnam em ótimos locais em matrizes densas ($20 \times 20$), este projeto utiliza 12 threads para avaliar milhões de vizinhanças por segundo, permitindo "saltar" para fora dessas zonas de estagnação.

---

## 📊 Resultados e Comportamento Observado

- **Teto de Melhoria:** Em instâncias pequenas ($4 \times 4$ e $5 \times 5$), o algoritmo atinge o seu limite máximo de otimização quase instantaneamente (testes comparativos de 20s, 60s e 6min apresentaram o mesmo resultado, indicando convergência para o ótimo global).
- **Reduções de Alto Nível:** Alcançamos melhorias de até **-26,23%** em instâncias de média ordem e mantivemos reduções consistentes de **-15,54%** em instâncias massivas ($20 \times 20$), onde métodos tradicionais costumam falhar.
- **Trade-off Makespan vs Flowtime:** O motor prioriza a fluidez do sistema. Observou-se que o algoritmo aceita aumentar o *Makespan* estrategicamente para garantir que o tempo médio de entrega dos jobs (*Flowtime*) seja reduzido.

---

## 📚 Referências Bibliográficas

- **GONZALEZ, T.; SAHNI, S.** *Open Shop Scheduling to Minimize Finish Time*. Journal of the ACM, 1976.
- **BRÄSEL, H. et al.** *Heuristic constructive algorithms for open shop scheduling to minimize mean flow time*. European Journal of Operational Research, 2008.
- **ANDRESEN, M. et al.** *Simulated annealing and genetic algorithms for minimizing mean flow time in an open shop*. Mathematical and Computer Modelling, 2008.

---

## 🏗️ Estrutura do Projeto

```bash
OpenShop-Flowtime/
├── include/        # Headers (.hpp)
├── src/            # Implementação principal (grafos, parser)
├── apps/           # Executáveis (otimização, testes)
├── instancias/     # Instâncias Taillard (.psi)
├── bin/            # Binários gerados
└── Makefile        # Sistema de build
