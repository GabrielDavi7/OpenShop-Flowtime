# 🧩 Open Shop Scheduling: Flowtime Optimizer

Este projeto consiste em uma ferramenta de **simulação e otimização** para o problema de escalonamento **Open Shop**, com foco na **minimização do Flowtime** (tempo total de permanência dos trabalhos no sistema).

A solução utiliza **Teoria dos Grafos** para representar dependências entre operações e aplica uma **meta-heurística de Busca Local** para encontrar soluções melhores.

---

## 🚀 Tecnologias e Conceitos

- **Linguagem:** C++17
- **Representação:** Grafos Acíclicos Dirigidos (DAG)
- **Algoritmo Base:** Ordenação Topológica (Algoritmo de Kahn)
- **Otimização:** Hill Climbing Estocástico (Busca Local com 3-Swap)
- **Build System:** Makefile (compatível com Windows e Linux)

---

## 📂 Estrutura do Projeto

```
OpenShop-Flowtime/
├── include/        # Arquivos de cabeçalho (.hpp)
├── src/            # Implementação de grafos e parser (.cpp)
├── apps/           # Programas principais (otimização, testes, baseline)
├── instancias/     # Arquivos de entrada (.psi) - instâncias de Taillard
├── bin/            # Executáveis gerados
└── Makefile        # Script de compilação
```

---

## 🛠️ Como Compilar e Executar

### ✅ Pré-requisitos

- Compilador **g++** com suporte a C++17
- Ferramenta **make**

---

### 🔧 Comandos

#### Compilar o projeto

```bash
make
```

#### Executar Grafo Fixo

Teste Grafo Fixo

```bash
make run_teste
```

#### Executar processamento em lote (Baseline)

Calcula o Flowtime inicial para todas as instâncias.

```bash
make run_instancias
```

#### Executar a otimização (Busca Local)

Executa o algoritmo que tenta melhorar o Flowtime inicial.

```bash
make run_otimizacao
```

#### Limpar arquivos gerados

```bash
make clean
```

---

## 🧠 Algoritmo de Otimização

O sistema utiliza **Hill Climbing Estocástico**, seguindo o fluxo:

1. **Baseline:**
   Geração de uma solução inicial sequencial.

2. **Perturbação:**
   Realização de trocas aleatórias (swaps) na ordem de execução.

3. **Avaliação:**
   - Construção de um grafo de dependências
   - Aplicação do algoritmo de Kahn
   - Cálculo do **Flowtime** e do **Makespan (caminho crítico)**

4. **Aceitação:**
   Se a nova solução melhora o Flowtime, ela substitui a atual.

---

## 📊 Resultados Obtidos

Teste realizado com a instância **ta01Osp.psi** durante **10 segundos**:

| Métrica  | Valor Inicial | Valor Otimizado |
| -------- | ------------- | --------------- |
| Flowtime | 1043          | 958             |
| Makespan | 352           | 267             |

### 📈 Melhorias

- **Redução do Flowtime:** ~8.15%
- **Redução do Makespan:** significativa

---

## 📌 Observações

- O desempenho depende do tempo de execução da busca local (Alguns casos tempo maior não significa melhoria, por exemplo na instancia 01 onde 10 segundos ja achou o resultado "Ótimo Local (ou Local Optimum).")
- Estratégias mais avançadas (Simulated Annealing, Tabu Search) podem melhorar ainda mais os resultados
- O projeto é modular e permite fácil extensão

---

## 👨‍💻 Autor

Desenvolvido como projeto acadêmico utilizando conceitos de:

- Escalonamento de tarefas
- Teoria dos grafos
- Otimização heurística

---
