# Base de modelos do simulador

> Documento-base da Fase 0. Ele guarda a escolha dos modelos, a arquitetura de simulação e os dados já levantados até a Etapa C do `ROTEIRO`. Depois o conteúdo passa para os documentos 04, 05, 09 e 15 e este arquivo sai do repositório.
> Nenhum número daqui está no registro de fontes ainda. Todo parâmetro leva [VERIFICAR NA FONTE] até o passo B1 e entra no pacote só com ID e local.
> Equações vêm de manuais técnicos, livros e artigos. Dados vêm da Embrapa e de órgãos brasileiros. Parâmetro estrangeiro leva o rótulo **provisório**.

## 1. Critérios e matriz de decisão

O usuário fixou sete critérios. Cada modelo candidato é avaliado contra todos.

| # | Critério | Por que importa |
|---|---|---|
| C1 | Baixa complexidade inicial | A fatia vertical precisa sair inteira antes de qualquer refinamento |
| C2 | Escalabilidade | O modelo precisa trocar estado com modelos de fases posteriores sem reescrita |
| C3 | Parâmetros publicados ou mensuráveis pela Embrapa ou por órgão brasileiro | Regra de fonte do projeto (`00_INDICE` §1.4) |
| C4 | Cobertura de vários cultivos | Cultura nova deve ser só dado novo |
| C5 | Estabilidade quando vários modelos interagem | O acoplamento não pode divergir nem depender da velocidade do relógio |
| C6 | Poder explicativo biológico | Um mesmo modelo deve explicar vários fenômenos do cultivo |
| C7 | Documentação aberta | O agente precisa ler as equações para implementar e testar |

O simulador pode revelar fenômenos difíceis de ver no campo. Por isso o modelo precisa ser causal. Bônus fixo de produtividade não explica nada e fica fora.

Nota de 1 a 5 (5 é o melhor). O traço indica critério que não se aplica.

| Domínio e candidato | C1 | C2 | C3 | C4 | C5 | C6 | C7 | Escolha |
|---|---|---|---|---|---|---|---|---|
| Acoplamento por Lie, árbitro de recurso e ledger inteiro | 4 | 5 | – | 5 | 5 | – | 5 | Fase 1 |
| Splitting de Strang | 2 | 3 | – | 4 | 4 | – | 5 | Descartado |
| Waveform relaxation | 1 | 3 | – | 4 | 5 | – | 4 | Descartado |
| Água por *bucket* FAO-56 com Curve Number | 5 | 4 | 4 | 5 | 5 | 2 | 5 | Fase 1 |
| Água em várias camadas (cascata) | 4 | 5 | 4 | 5 | 5 | 3 | 5 | Fase 2 |
| Equação de Richards | 1 | 4 | 3 | 5 | 2 | 4 | 5 | Fase 4 opcional |
| Planta AquaCrop simplificado (água) | 4 | 3 | 3 | 4 | 5 | 3 | 5 | Fase 1 |
| Planta por luz (RUE, tipo LINTUL) | 3 | 4 | 2 | 4 | 4 | 4 | 5 | Descartado |
| Planta híbrida, mínimo entre luz e água | 3 | 5 | 3 | 5 | 4 | 5 | 5 | Fase 2 |
| Daninha logística com Cousens | 5 | 2 | 2 | 2 | 5 | 1 | 4 | Fase 1 |
| Daninha com o mesmo código da planta | 3 | 5 | 2 | 5 | 4 | 5 | 4 | Fase 2 |
| Solo físico com Ds, bioporos e rugosidade | 3 | 5 | 4 | 5 | 4 | 4 | 4 | Fase 3 |
| Compactação completa (Söhne, SoilFlex) | 1 | 3 | 2 | 5 | 3 | 4 | 4 | Descartado |
| Química por inventário, pH–V e cinética do calcário | 4 | 4 | 5 | 5 | 5 | 3 | 5 | Fase 2 |
| Troca de Gapon e salinidade | 3 | 4 | 3 | 5 | 4 | 4 | 5 | Fase 3 |
| Húmus em dois compartimentos | 4 | 4 | 3 | 5 | 5 | 3 | 4 | Fase 3 |
| RothC ou CENTURY | 2 | 4 | 3 | 5 | 4 | 4 | 5 | Fase 4 |
| Especiação com PHREEQC em tempo real | 1 | 2 | 2 | 5 | 1 | 5 | 5 | Descartado |
| Clima por série real do INMET | 5 | 5 | 5 | 5 | 5 | – | 5 | Fase 1 |
| Gerador ClimaBR (Markov de 1ª ordem) | 4 | 5 | 5 | 5 | 5 | – | 4 | Fase 2 |
| Markov com exponencial mista | 3 | 5 | 4 | 5 | 5 | – | 4 | Fase 3 |
| Encosta por CN, MUSLE e D8 em cascata | 4 | 5 | 4 | 5 | 5 | 3 | 5 | Fase 3 |
| Onda cinemática e *virtual pipes* | 2 | 3 | 2 | 5 | 2 | 3 | 4 | Fase 4 só visual |

## 2. Arquitetura de simulação

### 2.1 Relógio
1. A razão base é **24 para 1**. Um dia de jogo dura uma hora real em 1x. O jogador escolhe 1x, 2x, 4x ou 8x. Em 8x o dia dura 7,5 minutos.
2. O multiplicador muda quantos passos de tempo simulado rodam por segundo real. Ele nunca muda o tamanho do passo (acumulador de passo fixo, Fiedler) [MODELO].
3. O mecanismo é a estabilidade. Um integrador explícito só é estável abaixo de um passo máximo ligado à constante de tempo mais rápida do processo. Multiplicar o passo por 8 levaria processos rápidos à divergência e faria o resultado depender da velocidade.
4. A razão 24 para 1 fica mantida. A estimativa de custo da §2.8 mostra que a CPU não limita. O limite é o tempo do jogador, já que um ciclo de alface de 45 dias leva 45 horas reais em 1x e 5,6 horas em 8x.
5. Proposta a validar. Oferecer 16x e 32x e a opção "pular até o próximo evento" na campanha. Em 16x os 45 dias passam em 2,8 horas.

### 2.2 Camadas de taxa

| Camada | Passo (tempo de jogo) | Segue o multiplicador | Processos |
|---|---|---|---|
| L0 quadro | um quadro real | Não | Renderização, interface, câmera, protagonista, máquinas, partículas |
| L1 tick de mundo | 1 a 5 minutos | Sim | Relógio, posição do sol, válvulas de irrigação, início e fim de chuva |
| L2 horário | 1 hora | Sim | Temperatura e umidade horárias, molhamento foliar, decaimento de defensivo, temperatura da composteira |
| L3 diário | 1 dia | Sim | Astronomia, ET0, água, química, planta, daninha, solo físico, conferência de balanço |
| L4 evento | instantâneo | Não se aplica | Operações do jogador e chegada de laudos |

1. A física do protagonista e das máquinas roda em tempo real. O Godot avisa que acelerar a física exige mais passos por segundo. Em 8x seriam 480 por segundo, fora do alcance do notebook. Nenhum resultado agronômico depende da física do Godot.
2. O trabalho físico do jogador custa mais tempo de jogo em velocidades altas. Isso vira pressão de gestão.

### 2.3 Acoplamento mínimo
O acoplamento mínimo vem do desenho das interfaces. O esquema numérico vem depois.

1. **Entre módulos da mesma célula.** Splitting de Lie (Gauss–Seidel) na ordem causal. Quem produz um recurso roda antes de quem consome. O erro local do Lie é (Δt²/2)[A,B], o comutador dos operadores. O erro global é de primeira ordem em Δt. Com Δt de um dia, igual à resolução dos dados do FAO-56 e do INMET, o erro fica na ordem do erro de modelo já aceito por esses manuais.
2. **Por que não Strang.** O Strang exige meio-passo com sentido. Isso vale para EDOs. Não vale para "CN do evento diário" nem para "Ks do dia", que são mapeamentos empíricos.
3. **Entre células.** Jacobi com *double buffer*. Cada célula lê o estado imutável do dia d e escreve no estado do dia d+1. A ordem das células e o número de *threads* deixam de importar. Isso garante o mesmo resultado em 1x e em 8x.
4. **Entre camadas de taxa.** A camada rápida entrega à lenta **integrais** (soma de chuva, soma de ET0, soma de graus-dia) e nunca amostras. Integral de fluxo é massa e conserva por construção.
5. **Módulos não se conhecem.** Falam apenas com módulos de recurso (água, nitrogênio, luz) que arbitram oferta e demanda, como no APSIM. Cada consumidor declara a demanda. O dono do recurso decide quanto entrega. O consumidor cresce com o que recebeu. Só o dono do recurso debita o estoque, então não existe estoque negativo nem dupla contagem, mesmo com várias espécies.
6. **Pulsos ficam dentro de um módulo.** Chuva e irrigação são descontínuos. O Curve Number roda dentro do módulo de água. Mestres de passo fixo Gauss–Jacobi e Gauss–Seidel são inadequados para sinais descontínuos divididos entre módulos [VERIFICAR NA FONTE: Gomes et al. 2018 e Linköping ECP 157, no passo B1].
7. **Realimentações lentas** têm atraso de um dia. A cobertura do dossel de ontem afeta a evaporação de hoje. Esse atraso é estável.
8. Iteração de ponto fixo e *waveform relaxation* ficam para acoplamentos rígidos. Nenhum acoplamento das fases 1 a 3 é rígido na escala diária.

### 2.4 Subsistemas e ordem diária

| # | Subsistema | Camada | Lê | Escreve |
|---|---|---|---|---|
| 0 | Relógio e calendário | L1 | | tick (inteiro de 64 bits em segundos de jogo), dia, dia juliano |
| 1 | Astronomia (FAO-56) | L3 | latitude, dia juliano | declinação, duração do dia, Ra |
| 2 | Clima | L2, agrega para L3 | data, RNG (clima, dia) | T(h), Rs, UR, vento, chuva e integrais diárias |
| 3 | ET0 | L3 | integrais do clima | ET0 |
| 4 | Eventos do jogador | L4 | registro de ações | fluxos pendentes e mudanças de estado físico |
| 5 | Árbitro de luz | L3 | cobertura e altura de cada espécie | fração interceptada por espécie |
| 6 | Água do solo | L3 | chuva, irrigação, ET0, demandas | escoamento, infiltração, drenagem, evaporação, transpiração alocada, Ks |
| 7 | Química do solo | L3 | drenagem, demandas de N | lixiviação, N alocado, pH, V% |
| 8 | Plantas (cultura e daninhas, mesmo código) | L3 | luz, água e N alocados, graus-dia | biomassa, cobertura, raiz, demanda do dia seguinte |
| 9 | Solo físico | L3 lento e L4 | tráfego, preparo, raízes, chuva acumulada | Ds, bioporos, rugosidade, ΔCN |
| 10 | Conferência | fim do L3 | ledger | verificação exata e hash do dia |

**Ordem do dia.** 0, 1, 2, 3, 4 (eventos com tick até o fim do dia, em ordem de tick e sequência), 5, 8a (demandas), 6, 7, 8b (crescimento), 9, 10.

Variáveis trocadas entre subsistemas.

| De para | Variável | Armazenamento | Natureza |
|---|---|---|---|
| Clima para ET0 e plantas | Tmax, Tmin, Rs, UR, u2, P | ponto flutuante de 64 bits | forçante |
| ET0 para água e plantas | ET0 | ponto flutuante | taxa potencial |
| Árbitro de luz para plantas | fração interceptada | ponto flutuante | partição |
| Plantas para água | demanda de transpiração, profundidade de raiz | ponto flutuante | demanda |
| Água para plantas | transpiração alocada | inteiro (µm) | fluxo conservado |
| Água para química | drenagem | inteiro (µm) | fluxo conservado |
| Química para plantas | N alocado | inteiro (mg/m²) | fluxo conservado |
| Solo físico para água e plantas | CN modificado, fator de resistência, fração de bioporos | ponto flutuante | estado |

A irrigação do regador às 7 horas e a chuva às 15 horas entram no módulo de água como fluxos do dia. Na fase 1 a ordem dentro do dia não muda o balanço diário. Se a fase 3 exigir resolução horária (Green–Ampt), só o módulo de água passa a fazer subpassos. A interface não muda.

### 2.5 Conservação exata
1. Estoques conservados ficam em inteiros. Água em micrômetros. Nitrogênio, fósforo, potássio e carbono em mg/m².
2. Todo fluxo é calculado em ponto flutuante e **arredondado uma só vez**. Depois é aplicado como transação (sai da origem, entra no destino) com o mesmo inteiro. Soma de inteiros é associativa. Entradas menos saídas menos variação de estoque dá zero exato, sem depender da ordem de soma nem do número de *threads*.
3. O ledger diário de cada célula guarda entradas (chuva, irrigação, escoamento vindo de fora, adubo), saídas (escoamento, drenagem, evaporação, transpiração, lixiviação, colheita) e variação de estoque. A conferência ao fim do L3 não tem tolerância.
4. Fronteiras do sistema (atmosfera, lençol, fora da propriedade) são contas-sumidouro explícitas. A interface pode responder "para onde foi a minha água".
5. A precisão do caminho da água pode ser de primeira ordem. A conservação é exata. Em jogo, erro de primeira ordem passa despercebido e água que some parece defeito.

### 2.6 Dados e paralelismo
1. Estrutura de arrays. Um `PackedFloat64Array` por variável de estado e um `PackedInt64Array` por estoque conservado. O índice de célula é o mesmo em todos. O estado da simulação fica em 64 bits. Só a renderização usa 32 bits (`Vector3`, `HeightMapShape3D`).
2. `WorkerThreadPool` com tarefas por bloco de células. É determinístico porque cada tarefa lê só o estado do dia d e escreve só nas próprias posições do dia d+1. As reduções globais acontecem depois do `join`, em ordem fixa de índice, ou em inteiros. Soma em ponto flutuante não é associativa.
3. Começar em GDScript. O núcleo de cada célula fica numa função pura `step_cell(i, estado_d, estado_d1)`. Essa forma permite portar para GDExtension em C++ sem mudar a interface.

### 2.7 Determinismo
1. **Nível A, obrigatório na fase 1.** Mesma build e mesma plataforma. Exige RNG por contador com `hash(semente_mundo, subsistema, dia, célula)`, passo fixo, *double buffer*, reduções ordenadas e nenhum uso do tempo do quadro dentro da simulação. O teste de regressão compara o hash diário do estado em 1x e em velocidade máxima.
2. **Nível B, a partir da fase 2 com C++.** Funções transcendentais próprias (`exp`, `log`, `pow`, `sin`, `cos`, `atan2`) para tudo que entra no estado. Compilar com contração de FMA desligada e sem *fast-math*. As operações básicas do IEEE 754 são determinísticas. As transcendentais variam entre bibliotecas matemáticas [VERIFICAR NA FONTE: arXiv 1705.09704 e Catto 2024, no passo B1].
3. Ponto fixo só nos estoques conservados (§2.5). Em toda a simulação ele prejudicaria C1 e C7 sem ganho agronômico.
4. Um save carregado em outra máquina continua válido sem o nível B. O save guarda o **estado** e não só as ações. O nível B só é necessário para replay entre plataformas.

### 2.8 Custo estimado
Estimativa de ordem de grandeza por analogia, a medir no notebook de teste logo na fase 1 com 100, 500 e 1000 células.

| Item | 1x | 8x |
|---|---|---|
| Segundos de jogo por segundo real | 24 | 192 |
| Passos L1 de 5 minutos por segundo real | 0,08 | 0,64 |
| Passos L2 de 1 hora por segundo real | 0,0067 | 0,053 |
| Dias (L3) por hora real | 1 | 8 |

1. A fase 1 gasta de 300 a 1000 operações por célula e dia (Penman–Monteith, *bucket*, CN, planta e química simples). A estimativa no A10-9600P é de 15 a 200 µs por célula em GDScript e de 0,3 a 3 µs em C++. Para 1000 células o lote diário custa de 15 a 200 ms em GDScript e de 0,3 a 3 ms em C++.
2. O único pico é o lote L3. Com *double buffer* ele pode ser fatiado ao longo dos quadros seguintes sem mudar o resultado. A interface mostra o estado do dia d até o dia d+1 ficar completo.
3. Até cerca de 500 células o GDScript basta. Acima disso, ou no modo dormir, o lote vai para C++.
4. Não há motivo para trocar 24 para 1 por 24 para 2.

### 2.9 Semente, passos por quadro, dormir, salvar e carregar
1. Cada subsistema tem o próprio gerador por contador. O valor é função de `hash(semente_mundo, subsistema, dia, célula)` e não há estado de gerador a salvar. O clima do dia 37 não depende de quantos números outro subsistema consumiu. Pular dias não diverge.
2. O número de passos por quadro tem limite, como o `max_physics_steps_per_frame` do Godot. Ao atingir o limite o jogo fica mais lento e o passo não muda. Isso evita a espiral da morte e preserva o determinismo.
3. **Dormir** roda o mesmo laço sem renderizar até a manhã seguinte, com o limite de passos elevado. Dez horas de jogo são cerca de 120 passos L1 e no máximo um L3. Custa menos de 1 segundo em C++ e de 0,2 a 0,5 segundo em GDScript. O dormir ajuda a estabilidade porque elimina o acoplamento com o quadro. Não há física L0 e o acumulador não satura.
4. Toda ação do jogador fica registrada como `(tick, ação, parâmetros)` para replay e regressão.
5. **Salvar** só no limite de um tick L1, com a fila de eventos vazia até aquele tick e o L3 em andamento concluído. O arquivo tem cabeçalho (identificador, versão do esquema, versão do jogo, hash do pacote), semente do mundo, tick, todos os arrays, fila de eventos pendentes ordenada, registro de ações e hash do último dia. O resto do acumulador é tempo real e não entra. Ao carregar ele vale zero.
6. Formato com manifesto JSON e bloco binário para os arrays (`FileAccess`). Sem objetos com *scripts* embutidos.
7. Migração por cadeia de funções `migrar_vN_para_vN+1`, com teste que carrega um save antigo, avança 30 dias e compara o hash esperado.

## 3. Modelos por fase

### 3.1 Fase 1 (canteiro plano de alface)

| Módulo | Modelo | Mecanismo | Referência a registrar |
|---|---|---|---|
| Astronomia | Declinação, fotoperíodo e radiação extraterrestre do FAO-56 | Expressões fechadas para qualquer latitude do Brasil | FAO-56 cap. 3 |
| Clima | Série real de estação automática do INMET e curva diurna de temperatura | Auditável contra a fonte e sem calibração estatística | INMET; Parton e Logan |
| ET0 | Penman–Monteith com Hargreaves–Samani de reserva | Balanço de energia mais demanda aerodinâmica | FAO-56 |
| Água | *Bucket* de uma camada, Kc simples, Ks e escoamento por CN | Tanque com ladrão que conserva massa a cada passo | FAO-56 cap. 8; NRCS |
| Solo físico | Capacidade de campo, ponto de murcha e densidade por classe e textura | Tabela com fonte e curva de retenção (§4.6) | Embrapa; SiBCS |
| Química | Laudo com V% e CTC, calagem por saturação por bases, NPK por formulação, nitrogênio mineral com lixiviação | Inventário e um compartimento dinâmico de nitrato | Embrapa Hortaliças; manuais estaduais |
| Planta | AquaCrop simplificado com graus-dia, cobertura, transpiração, produtividade da água e índice de colheita | Biomassa proporcional à transpiração normalizada pela ET0 | FAO AquaCrop |
| Daninhas | Coorte com cobertura logística e perda de Cousens | Competição com dano marginal decrescente | Cousens 1985 |
| Irrigação | Regador, gotejo com eficiência e fração de área molhada, tensiômetro virtual | Lâmina pela ETc e manejo pela tensão da água | Embrapa Hortaliças |
| Arquitetura | Ledger inteiro, árbitro de recurso, *double buffer*, nível A de determinismo | §2 | APSIM; FAO |
| Relatório | Atribuição da perda aos fatores Ks, nitrogênio, daninha e V% | Cada fator multiplicativo é registrado por dia | [INFERÊNCIA] |

A interface de planta já nasce preparada para o árbitro de luz da fase 2. A alface é a cultura da fatia porque o índice de colheita da alface é alto e quase constante. Isso dispensa florescimento e partição para frutos.

### 3.2 Fase 2 (horta completa e campanha em área plana)
1. Água em várias camadas por cascata.
2. Planta híbrida com biomassa igual ao mínimo entre a limitada por luz e a limitada por água (§7), com árbitro de luz.
3. Daninhas com o mesmo código da planta e parâmetros próprios.
4. Calcário com cinética por classe granulométrica, relação pH–V e NPK por formulação (§4.1 e §6).
5. Mineralização simples de nitrogênio.
6. Gerador climático ClimaBR.
7. Densidade do solo simples por preparo e tráfego, malha de canteiro de 5 cm e deformação do terreno (§8).
8. Núcleo do L3 em C++ com funções matemáticas próprias (nível B).
9. Defensivos com dose e carência do Agrofit e meia-vida provisória (§4.2).
10. Compostagem, palhada com Kc dual, aspersão com molhamento foliar, microtrator e tomate cereja.

### 3.3 Fase 3 (encosta, solo vivo e modo semiaberto)
1. Roteamento D8 em cascata, MUSLE e práticas conservacionistas (§8).
2. Solo físico com resistência à penetração, IHO e bioporos, e plantas de cobertura (§5).
3. Troca de Gapon e salinidade, fósforo em dois compartimentos e húmus em dois compartimentos (§6).
4. Consórcio por partição de luz, banco de sementes de daninhas, tempo hidrotérmico.
5. Markov com exponencial mista para eventos aleatórios e semiárido.
6. ZARC na escolha da época de plantio.

### 3.4 Fases 4 e 5
Fase 4 reúne economia, combustível, NPCs, cidades, gases e deriva, micronutrientes por tabelas de especiação geradas offline, RothC, epidemiologia espacial e água visual por *virtual pipes*. Fase 5 reúne os animais, que reaproveitam o ledger de N e C, o árbitro de recurso (pastejo como consumidor) e a compactação por pisoteio.

## 4. Dados resolvidos nesta rodada

Todos os valores abaixo vêm da segunda pesquisa e ainda não estão no registro de fontes. Fonte e local entram no passo B1.

### 4.1 Calcário
1. **Reatividade legal (MAPA).** RE é o percentual que reage em 3 meses. RE = 0,2·F(10–20) + 0,6·F(20–50) + 1,0·F(<50), com partículas retidas na peneira 10 valendo zero. PRNT = PN·RE/100 (Boletim Técnico 6; RBCS 39, 2015).
2. **Cinética observada (Embrapa).** Depois de 6 meses a reação é quase completa e depois de 3 meses mais de 60% do calcário fino já reagiu (Fageria, Embrapa Arroz e Feijão, Infoteca-e). A reação é mais rápida com o solo úmido.
3. **Formulação adotada.** Cada classe granulométrica j segue dM_j/dt = −k_j·f(θ)·M_j. A dissolução do CaCO₃ exige H⁺ em solução. Por isso f(θ) vale zero abaixo do ponto de murcha e cresce linearmente até 1 na capacidade de campo. Para a classe fina, 1 − e^(−90k) ≈ 0,62 dá k ≈ 0,0107 por dia e 87% em 180 dias. Essa calibração é provisória.
4. **pH e V%.** pH em água = 4,50 + 0,025·V (Raij, Cantarella e Zullo 1979, *Bragantia* 38, acesso aberto). Vale para solos de São Paulo com CTC moderada. O próprio artigo diz que a necessidade de calagem por essa relação sai baixa. O pacote guarda o par (a, b) por classe de solo.
5. Não há equação publicada para pH em CaCl₂. A relação 4,1 + 0,025·V é derivação própria e provisória.

### 4.2 Meia-vida de defensivos
1. A Embrapa tem banco parcial (Circular Técnica 48, Produção Integrada de Maçãs). O IBAMA publica perfis ambientais com DT50, mas vários valores estão tarjados por proteção de dados. O Agrofit traz registro, doses e carência e não traz DT50 de forma sistemática.
2. Não há base brasileira aberta e completa. O banco PPDB (Universidade de Hertfordshire) entra como **provisório**, com campo de origem, para ser trocado por valores do IBAMA ou de estudos brasileiros.
3. Decaimento M(t) = M0·exp(−ln2·t/DT50), com DT50 corrigida por Q10 e por umidade. Solo tropical quente degrada mais rápido que a medida de laboratório a 20 °C.

### 4.3 Alface

| Parâmetro | Valor | Fonte e situação |
|---|---|---|
| Kc inicial, médio, final | 0,70, 1,05 e 0,95 | Marouelli et al., Embrapa, cap. 5 de *Irrigação e fertirrigação em fruteiras e hortaliças* |
| Fração de área molhada | 0,95 | mesmo capítulo |
| Fração de depleção p | 0,30, ajustada por ETc | FAO-56 tabela 22 |
| Profundidade de raiz | 0,25 m em canteiro | apostila UFAC e Embrapa; proposta do projeto |
| Temperatura-base | 6 °C da semeadura ao transplante e 10 °C depois | Brunini et al. 1976, *Bragantia* 35, IAC |
| Soma térmica | 898 °C·dia em 45 dias e 1050 °C·dia em 52 dias | Fernandes et al. 2019, *Rev. Bras. Meteorologia* 34, Piauí |
| Resposta à temperatura | desenvolvimento máximo perto de 22 °C e menor acima | Brunini et al. 1976. Exige função com teto |
| Espaçamento | 0,25 × 0,25 m | Embrapa Hortaliças, livro de cultivares |
| Ciclo | 35 dias no calor e 45 dias nos demais (BRS Mediterrânea) | Embrapa Hortaliças |
| Ky | 0,95 do repolho | **provisório**. A alface não consta no FAO-33 nem no FAO-56 |
| Produtividade da água WP* | 15 g/m² | **provisório**. Valor padrão do AquaCrop |
| Cobertura máxima CCx | cerca de 0,9 adulto em 0,25 × 0,25 m | **provisório** |
| Índice de colheita | 0,8 a 0,9 da parte aérea | **provisório** |

Não há calibração brasileira do AquaCrop para alface. A calibração mais próxima é do Camboja (*Water* 10:666, 2018). Um valor de 410 °C·dia atribuído a Brunini em relatório português não foi verificado e não deve ser usado.

### 4.4 Curve Number
Sartori, Lombardi Neto e Genovez (2005, *RBRH*) classificam os solos brasileiros do SiBCS nos grupos hidrológicos A a D. A ANA adota a classificação (Nota Técnica 46/2018). Não há CN brasileiro para canteiro. Usar "culturas em linha" do TR-55 ajustado pelo grupo de Sartori, como **provisório**. O preparo reduz o CN e a chuva acumulada desfaz a redução, como no módulo SoilWat do APSIM.

### 4.5 Eficiência de irrigação
Padrão do pacote, a conferir na Circular Técnica 136 da Embrapa e nos demais capítulos.

| Método | Faixa publicada (Embrapa) | Padrão do pacote |
|---|---|---|
| Gotejamento | 75 a 90% | 0,90 |
| Microaspersão | 70 a 85% | 0,80 |
| Aspersão convencional | 60 a 80% conforme o tipo | 0,75 |
| Sulco | 30 a 70% | 0,60 |
| Regador | sem fonte Embrapa específica | 0,85 provisório |

A perda (1 menos a eficiência) é particionada em deriva e evaporação, percolação e escoamento. Assim ela fecha o ledger e alimenta a drenagem e a lixiviação de nitrogênio.

### 4.6 Capacidade de campo
A literatura brasileira de IHO usa a capacidade de campo a −10 kPa. Solos arenosos usam −6 kPa. O valor de −33 kPa é convenção norte-americana e subestima a capacidade em Latossolos. O pacote guarda os pontos da curva de retenção (−6, −10, −33, −100 e −1500 kPa) ou os parâmetros de van Genuchten, mais um campo `fc_potential_kPa` por classe (padrão −10, e −6 se a areia passar de 70%). O motor calcula θCC pela curva e nunca lê número solto.

### 4.7 Clima
O ClimaBR (Zanetti, Oliveira e Pruski; validado em 12 localidades de todas as regiões pluviométricas) usa Markov de primeira ordem. Para o semiárido a recomendação é Markov de primeira ordem com quantidades em exponencial mista (Wilks 1998), que captura melhor a cauda de eventos grandes que gera escoamento e erosão. O teste de aceitação é o comprimento dos veranicos. Ordem maior só entra se esse teste falhar. Parâmetros por estação do INMET e por mês.

## 5. Solo físico e descompactação biológica

Estados por camada. Densidade Ds, fração de bioporos contínuos fbio e rugosidade RR. Todos estão no `PackedFloat64Array`.

1. **Resistência à penetração.** RP = a·θ^b·Ds^c (forma de Busscher, ajustada por Tormena et al. 1998 em Latossolo). RP sobe quando o solo seca e quando compacta. Por isso um veranico endurece o solo compactado.
2. **IHO.** Limite superior igual ao menor entre θCC e o θ com 10% de ar. Limite inferior igual ao maior entre θPMP e o θ em que RP chega ao valor crítico. O padrão é 2,0 MPa (Embrapa Trigo e RBCS). Em plantio direto com bioporos citam-se até 3,5 MPa.
3. **Alongamento de raiz.** f_RP vale 1 até cerca de metade do valor crítico, cai linearmente até zero no crítico e a eficiência é f = fbio + (1 − fbio)·f_RP. Raízes da cultura seguinte passam pelos bioporos. Isso explica sem regra especial por que o plantio direto tolera RP maior.
4. **Estresse hídrico (fase 3).** O Ks usa o IHO no lugar de (CC − PMP). O mesmo solo com o mesmo θ pode estressar a planta se estiver compactado.
5. **Compactação por tráfego.** ΔDs = (Ds_max − Ds)·[1 − exp(−σ/σ_c(θ))]·α. A tensão σ é carga sobre área de contato. A tensão crítica σ_c cai com a umidade, então solo úmido compacta mais. É redução de baixa complexidade de Söhne e SoilFlex **proposta pelo projeto** [INFERÊNCIA].
6. **Descompactação.**
   1. Preparo leva Ds ao valor solto, eleva RR e reduz o CN. Depois a reconsolidação é exponencial com a chuva acumulada.
   2. Raízes formam bioporos segundo dfbio/dt = β·(incremento diário de raiz pivotante na camada)·(1 − fbio/fbio_max) − fbio/τ. A densidade cai segundo dDs/dt = −γ·(dfbio/dt)·Ds. O parâmetro β é da espécie (nabo forrageiro e crotalária altos) e τ é a persistência do canal.
   3. Ciclos de umedecimento e secagem entram como termo pequeno proporcional à argila.
7. **Base empírica.** A Embrapa descreve nabo forrageiro, milheto e crotalária como formadores de bioporos. Estudo de densidade ao redor da raiz de nabo indica compactação local durante o engrossamento e descompactação no solo maior depois da decomposição. Por isso o efeito entra em fbio principalmente na morte e decomposição da raiz.
8. **Lacuna.** Não há taxa quantitativa brasileira de redução de Ds ou de formação de bioporos por espécie. β e τ são provisórios e serão calibrados na fase 3 com dados de RBCS e PAB (densidade e macroporosidade antes e depois da cobertura). É o maior risco de credibilidade da fase 3.

Mapa entre ação e estado.

| Ação | Malha | Estado agronômico |
|---|---|---|
| Arar ou enxada rotativa | eleva e aplaina, RR sobe | Ds solta, CN cai, mistura resíduos e calcário na camada |
| Encanteirar | sulcos e cristas | armazenamento em depressão sobe, drenagem lateral |
| Cavar | depressão local | remove e realoca massa de solo (ledger de solo) |
| Tráfego | trilha de 1 a 3 cm | Ds sobe, CN sobe |

## 6. Solo químico e formação de compostos

A interface do módulo é constante nas fases. Ele expõe N, P e K alocáveis, pH, fator de toxidez e condutividade elétrica. Cada fase acrescenta compartimentos e reações atrás dessa interface.

1. **Fase 1.** Laudo (SB, CTC, V%, H+Al, Al) e nitrogênio mineral com lixiviação L = D·[NO₃]/θ.
2. **Fase 2.** Dose NC (t/ha) = (V2 − V1)·T/PRNT. Cinética por classe (§4.1). Estequiometria em cmolc. O CaCO₃ dissolvido soma Ca e Mg à SB e reduz H+Al e Al. O Al³⁺ neutralizado precipita como Al(OH)₃, um compartimento inerte. O pH vem de a + b·V. A saturação por Al reduz o alongamento de raiz e soma efeito ao f_RP. Na subsuperfície o gesso dissolve por limite de solubilidade, leva Ca e reduz Al sem mudar o pH. É um fenômeno emergente didático.
3. **Fase 3. Troca e salinidade.** Razão de sódio trocável ESR = k_G·SAR, com SAR = Na/√((Ca+Mg)/2) em mmolc/L. O USDA Handbook 60 dá k_G = 0,01475, valor estrangeiro **provisório**. Balanço de sais no *bucket* e redução de produtividade de Maas e Hoffman, com limiar 1,3 dS/m e 13% por dS/m para alface (FAO, a partir do FAO-29). A fração de lixiviação é LR = CEw/(5·CEe_t − CEw).
4. **Fase 3. Fósforo.** Dois compartimentos, lábil e fixado (Fe e Al), com Langmuir e capacidade parametrizada pelo fósforo remanescente. A transferência lenta de primeira ordem faz a adubação fosfatada em Latossolo argiloso perder disponibilidade de forma realista. Em pH acima de 7 entra um compartimento análogo de Ca-P.
5. **Fase 3. Húmus (Hénin–Dupuis).** Resíduo fresco R decompõe com k_R·f(T)·f(θ). Uma fração h vira húmus H e o resto vira CO₂, que sai pelo ledger de carbono. dH/dt = h·k_R·R − k_H·f(T)·f(θ)·H e a mineralização de N é k_H·H/(C:N). Ácidos fúlvicos, húmicos e humina são **partição diagnóstica** de H, usada só no laudo didático. As constantes entre frações não são bem identificadas e estados mal identificados desestabilizam o acoplamento (C5).
6. **Fase 4.** Disponibilidade de Fe, Mn, Zn e Cu como função de pH e matéria orgânica, com a quelação amortecendo a queda em pH alto.
7. **Especiação.** PHREEQC em tempo real não serve. Exige iteração de Newton com dezenas de espécies por célula, pode não convergir (C5) e depende de constantes de superfície que não existem para os solos-alvo (C3). A alternativa é rodar o PHREEQC **offline** para poucas classes de solo e gerar tabelas de consulta de custo constante.

## 7. Planta e daninhas

A fase 1 usa o AquaCrop simplificado. A interface já aceita o híbrido da fase 2.

1. **Luz (árbitro).** Para as espécies i de uma célula com área foliar LAI_i e coeficiente k_i, a fração total interceptada é F = 1 − exp(−Σ k_i·LAI_i). A parte da espécie i é F·(k_i·LAI_i)/Σ(k_j·LAI_j). Na fase 3 refina-se por estratos de altura.
2. **Biomassa limitada por luz.** B_L,i = RUE_i·PAR·F_i·f(T)·f(N).
3. **Biomassa limitada por água.** B_W,i = WP*_i·Tr_alloc,i/ET0.
4. **Crescimento.** ΔB_i = min(B_L,i, B_W,i). É a lei de Liebig entre dois recursos co-limitantes. Com água farta a luz manda e aparecem sombreamento, densidade e competição por luz. Na seca a água manda e o modelo volta ao comportamento do AquaCrop.
5. **Nitrogênio.** A demanda é a concentração crítica vezes ΔB. A alocação vem do árbitro de N. O fator f(N) = N_alocado/N_demanda limita o RUE no dia seguinte.
6. **Daninhas** usam o mesmo código com parâmetros próprios (temperatura-base, RUE, WP*, k, altura máxima, profundidade de raiz). A hipérbole de Cousens passa a ser **teste de validação**. O simulador deve reproduzir a perda hiperbólica ao variar a densidade de daninhas. Se não reproduzir, os parâmetros estão errados.

## 8. Terreno e encosta

1. **Propriedade.** Terrain3D na série 1.x (GDExtension em C++). Suporta o renderizador Compatibility e edita na CPU. A série 2.x não deve suportar o Compatibility, então a versão fica fixada na 1.x [VERIFICAR na documentação do Terrain3D antes de fixar]. Resolução de cerca de 1 m.
2. **Canteiro.** Malha própria (`ArrayMesh`) em grade de 5 cm. Um canteiro de 1,2 por 10 m tem 4800 vértices, regenerados na CPU em milissegundos apenas no evento de edição e nunca por quadro.
3. **Colisão.** `HeightMapShape3D` por canteiro em 10 cm, atualizado só ao fim da ação.
4. **Grade agronômica** de cerca de 1 m² por canteiro, separada da grade visual. Cada ação escreve a variação de altura na malha e os estados na célula (§5).
5. **Escoamento (fase 1).** CN diário por célula.
6. **Roteamento na encosta (fase 3).** D8 em grade de 1 a 2 m, com as células ordenadas por cota uma vez a cada edição do terreno. Numa passada de cima para baixo o escoamento de cada célula vira entrada da vizinha de jusante, que infiltra até o déficit do próprio *bucket*. É determinístico, conservativo em inteiros e sem restrição de CFL.
7. **Erosão por evento (MUSLE).** Sed = 11,8·(Q·q_p)^0,56·K·C·P·LS (Williams 1975). O fator LS segue Bertoni e Lombardi Neto. Os valores de P para plantio em contorno, cordões de vegetação e terraços vêm do livro e **precisam de conferência**.
8. **Terraços e curvas de nível** são geometria na malha e aparecem no D8 como interrupções do comprimento de rampa. O fator P só se aplica a práticas que não existem na malha.
9. Onda cinemática e *virtual pipes* exigem subpassos de CFL e ficam para a água visual da fase 4, sem efeito no ledger.

## 9. Jogos de referência

Qualidade das fontes baixa. Vêm de guias e fóruns de comunidade.

1. **Medieval Dynasty.** Não há fertilidade de longo prazo e a rotação não tem efeito agronômico. O adubo prepara cada parcela antes do plantio. Dez unidades de podridão ou duas de esterco viram um adubo. Rega, daninhas e porcentagem de fertilidade **não foram confirmados**.
2. **Farming Simulator 25.** Bônus aditivos fixos. Calcário soma 15% a cada cerca de três colheitas, campo sem daninhas soma 20%, duas adubações somam 22,5% cada, aração soma 15% e rolagem soma 2,5%. Rotação detalhada, pH e N, P e K aparecem em *mods*. Escala de tempo, sono e duração do dia **não foram verificados**.
3. **Lição de projeto.** Os dois jogos usam bônus sem estado. O simulador-agro faz esses efeitos nascerem de estados (pH por V%, daninha por competição, adubo por compartimentos). Os *mods* do Farming Simulator mostram demanda por isso.

## 10. Lacunas abertas e riscos

1. Ky da alface. Não existe no FAO-33. O valor 0,95 do repolho é provisório.
2. Calibração do AquaCrop para alface no Brasil. Não foi encontrada. WP*, CGC e CCx são provisórios.
3. A relação pH–V vale para solos de São Paulo. Em outras mineralogias pode falhar, então os parâmetros são por classe.
4. Meia-vida de defensivos sem base brasileira aberta e completa.
5. Taxas de descompactação biológica por espécie sem fonte quantitativa brasileira. Maior risco da fase 3.
6. Valores de livro não verificados. LS e P de Bertoni e Lombardi Neto, constantes de Busscher e a forma da equação de tração da norma ASABE D497.
7. Estimativas de custo da §2.8 são de ordem de grandeza e exigem benchmark.
8. Determinismo entre plataformas só vale com o nível B.
9. Duração do ciclo em tempo real (45 horas em 1x). Testar 16x, 32x e "pular até o evento" com jogadores.
10. Fontes sobre os jogos de referência são de comunidade.
