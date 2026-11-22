# Glossário do projeto

> Termos do **software**. Termos agronômicos (CTC, saturação por bases, Kc, graus-dia, nível de controle) seguem a definição da publicação citada no pacote.
> Todos os termos abaixo são definições de projeto [INFERÊNCIA].

| Termo | Definição | Detalhe em |
|---|---|---|
| **Motor** | Código genérico em Godot (`engine/`). Conhece variáveis de estado, processos e operações; não conhece nenhuma cultura. | `00_INDICE` §4; documento 02 |
| **Pacote** | Conjunto de dados agronômicos com fonte (`packs/<pacote>/`): culturas, solos, clima, pragas, insumos, tabelas de recomendação e cenários. | documentos 04, 14 |
| **Propriedade** | Mapa 3D navegável com área de horta, talhões, galpão e fonte de água. | documento 06 |
| **Canteiro / talhão** | Unidade de manejo com estado próprio de solo, água, cultura e sanidade. Canteiro na horta; talhão na lavoura. | documento 05 |
| **Safra** | Ciclo de uma cultura num canteiro ou talhão, do preparo à colheita. | documento 11 |
| **Passo diário** | Uma execução do modelo para toda a propriedade: clima do dia → solo → água → nutrientes → planta → pragas e doenças. | documento 05 |
| **Operação** | Ação do jogador (analisar solo, calcar, adubar, plantar, irrigar, capinar, cobrir, monitorar, controlar, colher). Toda operação passa por `act(verbo, alvo, insumo)`. | documento 08 |
| **Laudo** | Resultado de uma medição (análise de solo, foliar, amostragem de pragas). Chega com atraso e com a incerteza da amostragem. | documento 13 |
| **Raio-x** | Visualização do estado real (umidade do perfil, nutrientes, raiz, inóculo), mostrada no relatório de safra e no modo aprendiz. | `00_INDICE` §2, princípio 6 |
| **Relatório de safra** | Tela de fim de safra: produtividade, qualidade, custo, saúde do solo, linha do tempo das operações e a explicação de cada resultado com a fonte. | documento 12 |
| **Desafio** | Cenário com objetivo (ex.: recuperar um solo compactado, colher no verão chuvoso). | documento 11 |
| **Modo livre** | Propriedade aberta sem objetivo fixo. | documento 19 |
| **Fatia vertical** | Primeira entrega jogável: um canteiro, uma cultura, ciclo completo. | `00_INDICE` §6 |
| **Semente (RNG)** | Número que inicializa o gerador pseudoaleatório. Com a mesma semente, a mesma série climática e as mesmas versões, a safra se repete idêntica. | documento 15 |
| **Registro de fontes** | `docs/referencias/fontes.json`: toda publicação citada, com ID, URL, data de acesso e licença. | `docs/referencias/FONTES.md` |
