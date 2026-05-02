# 🧩 Open Shop Scheduling: Flowtime Optimizer

Este projeto consiste em uma ferramenta de **simulação e otimização** para o problema de escalonamento **Open Shop**, com foco na **minimização do Flowtime** (tempo total de permanência dos trabalhos no sistema).

A solução utiliza **Teoria dos Grafos** para representar dependências entre operações e aplica uma **meta-heurística de Busca Local** para encontrar soluções melhores.

---

## 🚀 Tecnologias e Conceitos

- **Linguagem:** C++17
- **Representação:** Grafos Acíclicos Dirigidos (DAG)
- **Algoritmo Base:** Ordenação Topológica (Algoritmo de Kahn)
- **Otimização:** Meta-Heurística Híbrida (VNS - Variable Neighborhood Search + Multi-Start)
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

#### Executar o caminho

Executa o algoritmo que mostra a diferença de caminho entre baseline e VSN.

```bash
make run_caminho
```

#### Executar o específico
Executar teste específico (Instância Individual)

```bash
make run_especifico
# Ou via executável direto: .\bin\especifico.exe instancias/ta31Osp.psi 120 //escolha instancia e tempo
```

#### Limpar arquivos gerados

```bash
make clean
```

---

## 🧠 Algoritmo de Otimização

O sistema evoluiu para uma **Meta-Heurística Híbrida**, combinando **Variable Neighborhood Search (VNS)** com um mecanismo de **Multi-Start**, estruturado da seguinte forma:

1. **Baseline e Memória Global:**
   Geração de uma solução inicial sequencial. Esse primeiro resultado é salvo em uma "Memória Global" blindada para garantir que o recorde absoluto nunca seja perdido.

2. **Perturbação Dinâmica (VNS):**
   Realização de trocas aleatórias (_swaps_) na ordem de execução. A intensidade das trocas (fator `k`) é adaptativa: começa leve para refinamento local e aumenta gradativamente de força para conseguir "pular" para fora de platôs e Ótimos Locais.

3. **Avaliação (O Juiz):**
   - Construção de um grafo de dependências (arestas fixas de operações + arestas dinâmicas das máquinas).
   - Aplicação do Algoritmo de Kahn (Ordenação Topológica).
   - Cálculo do **Flowtime** (objetivo primário) e do **Makespan** (gargalo de fábrica).

4. **Aceitação e Refinamento:**
   Se a nova solução reduz o Flowtime (ou empata, para permitir navegação em platôs), ela substitui a atual e a força do pulo (`k`) volta a ser 1. Se quebrar o recorde absoluto, a Memória Global é atualizada.

5. **Fuga de Estagnação (Multi-Start):**
   Caso o algoritmo atinja um limite de "paciência" (10.000 tentativas sem melhoria), as filas de todas as máquinas sofrem um embaralhamento total (_Shuffle_). Isso reinicia a busca em uma área completamente inexplorada do problema, impedindo que o algoritmo fique travado.

---

## 🔄 Evolução e Impacto da Atualização

Durante as análises de desempenho da primeira versão (baseada em _Hill Climbing_ com _3-Swap_ fixo), observou-se que o algoritmo estagnava rapidamente em instâncias médias (como a `ta05`), caindo em armadilhas matemáticas conhecidas como **Ótimos Locais**.

Para contornar esse problema e exaurir as possibilidades do espaço de busca, o motor de otimização foi reescrito, o que trouxe resultados drásticos e reestruturou a forma como a Inteligência Artificial lida com o problema de roteamento do _Open Shop_:

- **Quebra de Platôs:** Instâncias que antes apresentavam 0.00% de melhoria tiveram o bloqueio quebrado, apresentando reduções sólidas de tempo.
- **Redução Massiva:** Em instâncias como a `ta10` e `ta20`, as melhorias de Flowtime saltaram de valores singelos para reduções robustas entre **15% e 20%**.
- **Reestruturação do Gargalo (Caminho Crítico):** Os logs comprovam que o VNS não apenas ajusta os tempos, mas altera estruturalmente o Caminho Crítico da fábrica. A IA descentraliza o trabalho, mudando completamente qual máquina atua como o gargalo final (Makespan) para otimizar o fluxo geral.

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
