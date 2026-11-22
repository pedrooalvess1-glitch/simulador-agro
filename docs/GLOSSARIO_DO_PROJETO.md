# Glossário do projeto

> Termos do **software**. Termos agronômicos (CTC, saturação por bases, Kc, graus-dia, nível de controle) seguem a definição da publicação citada no pacote.
> Todos os termos abaixo são definições de projeto [INFERÊNCIA].

| Termo | Definição | Detalhe em |
|---|---|---|
| **Motor** | Código genérico em Godot (`engine/`). Conhece variáveis de estado, processos e operações; não conhece nenhuma cultura. | `00_INDICE` §4; documento 02 |
| **Pacote** | Conjunto de dados agronômicos com fonte (`packs/<pacote>/`): culturas, solos, clima, pragas, insumos, tabelas de recomendação e cenários. | documentos 04, 14 |
| **Propriedade** | Mapa 3D navegável com área de horta, talhões, galpão e fonte de água. | documento 06 |
| **Canteiro / talhão** | Unidade de manejo com estado próprio de solo, água, cultura e sanidade. Canteiro na horta; talhão na lavoura. | documento 05 |
| **Safra** | Ciclo de uma cultura num canteiro ou talhão, do preparo à colheita. | documento 12 |
| **Protagonista** | Personagem do jogador, controlado em primeira ou terceira pessoa. Opera ferramentas e máquinas. | documento 18 |
| **Tempo simulado** | Relógio do mundo, 24 vezes mais rápido que o real em 1x. O multiplicador 2x, 4x ou 8x muda quantos passos rodam por segundo real. | `BASE_MODELOS` §2.1 |
| **Camada de taxa** | Grupo de processos com o mesmo passo fixo. L0 quadro, L1 tick de mundo, L2 horário, L3 diário, L4 evento. | `BASE_MODELOS` §2.2 |
| **Dormir** | Avanço até a manhã seguinte rodando o mesmo laço de passos sem renderizar. | `BASE_MODELOS` §2.9 |
| **Árbitro de recurso** | Módulo dono de um recurso (luz, água, nitrogênio). Recebe a demanda de cada consumidor, decide quanto entrega e debita o estoque. | `BASE_MODELOS` §2.3 |
| **Ledger** | Registro diário de entradas, saídas e variação de estoque de cada célula, em inteiros. Garante balanço exato de água, nutrientes e carbono. | `BASE_MODELOS` §2.5 |
| ***Double buffer*** | Cada célula lê o estado do dia d e escreve o estado do dia d+1. A ordem das células não muda o resultado. | `BASE_MODELOS` §2.3 |
| **Campanha** | Modo que ensina o básico do cultivo e do manejo, primeiro em área plana e depois em área inclinada. | documento 11 |
| **Modo semiaberto** | Propriedade livre para o jogador, com eventos aleatórios. | documento 20 |
| **Operação** | Ação do jogador (analisar solo, calcar, adubar, plantar, irrigar, capinar, cobrir, monitorar, controlar, colher). Toda operação passa por `act(verbo, alvo, insumo)`. | documento 08 |
| **Laudo** | Resultado de uma medição (análise de solo, foliar, amostragem de pragas). Chega com atraso e com a incerteza da amostragem. | documento 13 |
| **Raio-x** | Visualização do estado real (umidade do perfil, nutrientes, raiz, inóculo), mostrada no relatório de safra e no modo aprendiz. | `00_INDICE` §2, princípio 6 |
| **Relatório de safra** | Tela de fim de safra: produtividade, qualidade, custo, saúde do solo, linha do tempo das operações e a explicação de cada resultado com a fonte. | documento 12 |
| **Fatia vertical** | Primeira entrega jogável: um canteiro, uma cultura, ciclo completo. | `00_INDICE` §6 |
| **Semente (RNG)** | Número que inicializa um gerador pseudoaleatório. Cada subsistema tem o seu, semeado por `hash(semente_mundo, subsistema, dia)`. Com a mesma semente, a mesma série climática, as mesmas ações e as mesmas versões, a safra se repete idêntica. | documento 15 |
| **Registro de fontes** | `docs/referencias/fontes.json`: toda publicação citada, com ID, URL, data de acesso e licença. | `docs/referencias/FONTES.md` |
