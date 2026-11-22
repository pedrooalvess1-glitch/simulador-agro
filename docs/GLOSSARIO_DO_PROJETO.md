# Glossário do projeto

> Termos do **software**. Termos agronômicos (CTC, saturação por bases, Kc, graus-dia, nível de controle) seguem a definição da publicação citada no pacote.
> Todos os termos abaixo são definições de projeto [INFERÊNCIA].

| Termo | Definição | Detalhe em |
|---|---|---|
| **Motor** | Código genérico (`engine/`). O núcleo C++ `agro_core` guarda e muda o estado do mundo calculado; o Godot desenha e recebe a entrada. Conhece variáveis de estado, processos e operações; não conhece nenhuma cultura. | `00_INDICE` §4; `ARQUITETURA_SIMULACAO` §2 |
| **Pacote** | Conjunto de dados agronômicos com fonte (`packs/<pacote>/`): culturas, solos, clima, pragas, insumos, tabelas de recomendação e cenários. | documentos 04, 14 |
| **Propriedade** | Mapa 3D navegável com área de horta, talhões, galpão e fonte de água. | documento 06 |
| **Canteiro / talhão** | Unidade de manejo com estado próprio de solo, água, cultura e sanidade. Canteiro na horta; talhão na lavoura. | documento 05 |
| **Safra** | Ciclo de uma cultura num canteiro ou talhão, do preparo à colheita. | documento 12 |
| **Protagonista** | Personagem do jogador, controlado em primeira ou terceira pessoa. Opera ferramentas e máquinas. | documento 18 |
| **Tempo simulado** | Relógio do mundo, 24 vezes mais rápido que o real em 1x. O multiplicador muda quanto tempo de jogo roda por quadro e nunca o passo dos modelos. | `ARQUITETURA_SIMULACAO` §3.2 |
| **Agenda de eventos** | Fila única de tudo que muda o mundo (passo diário, passo horário, evento de mundo, ação do jogador), ordenada pela chave (tick, fase, seq). | `ARQUITETURA_SIMULACAO` §3.1 |
| **Dormir** | Pulo até a hora de acordar, processando os mesmos eventos sem renderizar. | `ARQUITETURA_SIMULACAO` §3.2 |
| **Estado de leitura** | Estimativa do momento que instrumentos e visual mostram, calculada a partir do estado oficial da manhã e das ações do dia. Não é gravada. | `ARQUITETURA_SIMULACAO` §4.1 |
| **Árbitro de recurso** | Módulo dono de um recurso (luz, água, nitrogênio). Recebe a demanda de cada consumidor, decide quanto entrega pelo método do maior resto e debita o estoque. | `ARQUITETURA_SIMULACAO` §4.3 |
| **Ledger** | Contas inteiras de cada grandeza conservada (água em mL, N, P e K em µg, C em mg, solo em g). Toda mudança é uma transferência com o mesmo inteiro nos dois lados. | `ARQUITETURA_SIMULACAO` §6 |
| **Operador vertical e lateral** | Os processos de uma célula leem só a própria célula (vertical). O escoamento entre células segue um grafo em ordem topológica fixa (lateral, fase 3). | `ARQUITETURA_SIMULACAO` §5 |
| **detmath** | Funções exp, log, pow, sin, cos, tan, atan, atan2 e acos do núcleo, feitas só com operações exatas do IEEE 754 para dar o mesmo resultado no Linux e no Windows. | `ARQUITETURA_SIMULACAO` §7 |
| **Campanha** | Modo que ensina o básico do cultivo e do manejo, primeiro em área plana e depois em área inclinada. | documento 11 |
| **Modo semiaberto** | Propriedade livre para o jogador, com eventos aleatórios. É o único lugar com sorteio. | documento 20 |
| **Operação** | Ação do jogador (analisar solo, calcar, adubar, plantar, irrigar, capinar, cobrir, monitorar, controlar, colher). Toda operação passa por `act(verbo, alvo, insumo)`. | documento 08 |
| **Laudo** | Resultado de uma medição (análise de solo, foliar, amostragem de pragas). Chega com atraso e com a incerteza da amostragem. | documento 13 |
| **Raio-x** | Visualização do estado real (umidade do perfil, nutrientes, raiz, inóculo), mostrada no relatório de safra e no modo aprendiz. | `00_INDICE` §2, princípio 6 |
| **Relatório de safra** | Tela de fim de safra: produtividade, qualidade, custo, saúde do solo, linha do tempo das operações e a explicação de cada resultado com a fonte. | documento 12 |
| **Fatia vertical** | Primeira entrega jogável: um canteiro, uma cultura, ciclo completo. | `00_INDICE` §6 |
| **Semente (RNG)** | Número gravado no mundo que define os eventos sorteados. Cada sorteio é função pura de (semente do mundo, stream, dia, célula, índice). Os modelos de processo não sorteiam. Com a mesma semente, a mesma série climática, as mesmas ações e as mesmas versões, a safra se repete idêntica. | `ARQUITETURA_SIMULACAO` §8 |
| **Registro de fontes** | `docs/referencias/fontes.json`. Toda publicação citada, com ID, URL, data de acesso e licença, e a fila de verificação das publicações ainda incompletas. | `docs/referencias/FONTES.md` |
