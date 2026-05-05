# 🧩 Open Shop Scheduling: Parallel Flowtime Optimizer

Este projeto implementa um motor de otimização para o problema de escalonamento **Open Shop Scheduling Problem (OSSP)**, com foco na minimização do **Flowtime total (∑Cj)**.

A solução utiliza uma arquitetura híbrida de metaheurísticas com execução paralela para explorar eficientemente o espaço de soluções em instâncias de médio e grande porte.

---

## 🎯 Problema

O Open Shop Scheduling consiste em um conjunto de jobs que devem ser processados em múltiplas máquinas, respeitando a restrição de que cada job executa cada operação exatamente uma vez, sem sobreposição em máquinas ou jobs.

O objetivo deste projeto é minimizar o **tempo total de conclusão (Flowtime)**.

---

## 🚀 Tecnologias e Arquitetura

- **Linguagem:** C++17
- **Paralelismo:** `std::thread` (multi-core CPU)
- **Otimização:** Busca Tabu Paralela + Variable Neighborhood Search (VNS)
- **Estrutura de dados:** Grafos direcionados acíclicos (DAG)
- **Ordenação:** Algoritmo de Kahn (topological sort)
- **Build System:** Makefile com otimizações `-O3 -pthread`

---

## 🧠 Estratégia de Otimização

O sistema é baseado em uma abordagem híbrida de metaheurísticas:

### 1. Inicialização (Baseline + SPT)

O algoritmo inicia a busca a partir de duas soluções:

- Sequência original do problema
- Heurística SPT (Shortest Processing Time)

A melhor solução entre as duas é escolhida como ponto inicial.

---

### 2. Busca Local Paralela

A vizinhança é gerada por:

- swaps entre tarefas
- inserções

A avaliação dos vizinhos é distribuída entre múltiplas threads, explorando o hardware disponível.

---

### 3. Busca Tabu

É utilizada uma estrutura de memória de curto prazo (matriz tabu) para:

- evitar ciclos
- impedir revisitação de soluções recentes

---

### 4. Critério de Aspiração

Movimentos proibidos pela lista tabu podem ser aceitos caso produzam:

- melhoria global do Flowtime

---

### 5. Diversificação (VNS)

Quando ocorre estagnação:

- aplica-se perturbação na solução atual (shuffle)
- a lista tabu é reinicializada
- a busca é reiniciada em nova região do espaço de soluções

---

## 📊 Comportamento Observado

- Em instâncias pequenas e médias (ex: Taillard inicial), o algoritmo converge rapidamente para soluções estáveis.
- Em instâncias maiores, o sistema mantém melhoria incremental ao longo do tempo de execução.
- A paralelização aumenta significativamente a taxa de exploração do espaço de busca.

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
```
