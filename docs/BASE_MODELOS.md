# Base de modelos do simulador

> Documento-base da Fase 0. Ele guarda a escolha dos modelos e a arquitetura de tempo até a Etapa C do `ROTEIRO`, quando o conteúdo passa para os documentos 04, 05, 09 e 15 e este arquivo sai do repositório.
> Os números agronômicos citados aqui ainda não estão no registro de fontes. Por isso todos levam [VERIFICAR NA FONTE] até o passo B1.

## 1. Critérios de escolha dos modelos

O usuário fixou os critérios abaixo. Cada modelo candidato é avaliado contra todos eles.

| # | Critério | Por que importa |
|---|---|---|
| C1 | Baixa complexidade inicial | A fatia vertical precisa sair inteira antes de qualquer refinamento |
| C2 | Escalabilidade | O modelo precisa trocar estado com modelos que entram em fases posteriores sem reescrita |
| C3 | Parâmetros medidos e publicados pela Embrapa ou por órgão brasileiro competente | Regra de fonte do projeto (`00_INDICE` §1.4) |
| C4 | Cobertura de vários cultivos | Cultura nova deve ser só dado novo |
| C5 | Estabilidade numérica ou probabilística quando vários modelos interagem | O acoplamento entre subsistemas não pode divergir nem depender da velocidade do relógio |
| C6 | Poder explicativo biológico | Um mesmo modelo deve explicar vários fenômenos do cultivo |
| C7 | Documentação aberta | O agente precisa ler as equações para implementar e testar |

O simulador pode mostrar fenômenos difíceis de observar no campo. Por isso o modelo precisa ser causal. Um bônus fixo de produtividade não explica nada e fica fora.

## 2. Arquitetura de tempo

### 2.1 Relógio
1. A razão base é **24 para 1**. Um dia de jogo dura uma hora real na velocidade 1x.
2. O jogador escolhe 1x, 2x, 4x ou 8x. Na velocidade 8x um dia de jogo dura 7,5 minutos reais.
3. O multiplicador muda quantos passos de tempo simulado rodam por segundo real. Ele nunca muda o tamanho do passo. Esse é o padrão *fixed timestep* com acumulador [MODELO].
4. O mecanismo da escolha é a estabilidade. Um integrador explícito só é estável abaixo de um passo máximo ligado à constante de tempo mais rápida do processo. Multiplicar o passo por 8 levaria processos rápidos à divergência. Manter o passo fixo faz 1x e 8x produzirem o mesmo estado final na mesma build.
5. O custo computacional cabe no alvo. Uma coluna de solo de um canteiro tem algumas dezenas de variáveis. Na velocidade 8x a camada horária roda 24 passos em 450 segundos reais. Centenas de canteiros em GDScript ficam muito abaixo do orçamento. A razão 24 para 1 fica mantida.

### 2.2 Camadas de taxa

| Camada | Passo (tempo de jogo) | Segue o multiplicador | Processos |
|---|---|---|---|
| L0 quadro | um quadro real | Não | Renderização, interface, câmera, protagonista, máquinas, partículas |
| L1 tick de mundo | 1 a 5 minutos | Sim | Relógio, posição do sol, válvulas de irrigação, início e fim de chuva |
| L2 horário | 1 hora | Sim | Temperatura e umidade do ar horárias, água do canteiro, molhamento foliar, decaimento de defensivo, temperatura da composteira |
| L3 diário | 1 dia | Sim | Clima do dia, ET0, crescimento da cultura e das daninhas, transformações de nitrogênio, reação do calcário |
| L4 evento | instantâneo | Não se aplica | Operações do jogador e chegada de laudos |

1. A física do protagonista e das máquinas roda em tempo real e não acelera. O Godot avisa que acelerar a física exige mais passos de física por segundo. Na velocidade 8x seriam 480 passos por segundo, fora do alcance do notebook de teste. Nenhum resultado agronômico depende da física do Godot.
2. O trabalho físico do jogador custa mais tempo de jogo em velocidades altas. Isso vira pressão de gestão e não é defeito.

### 2.3 Acoplamento mínimo entre subsistemas
1. Cada subsistema roda na sua própria camada de taxa.
2. Um subsistema só lê o estado publicado por outro na fronteira da camada mais lenta. A planta lê a média diária do fator de estresse hídrico calculado nas 24 horas anteriores. Essa técnica se chama *operator splitting* [MODELO].
3. O erro dessa separação é pequeno porque a planta integra devagar comparada à água.
4. A ordem dentro de cada passo é fixa: clima, água, química do solo, planta, daninha, pragas.
5. O estudo detalhado do acoplamento está na segunda pesquisa (passo B0 do `ROTEIRO`).

### 2.4 Determinismo, dormir e salvar
1. Cada subsistema tem seu próprio gerador pseudoaleatório. A semente de cada dia vem de `hash(semente_mundo, subsistema, dia)`. Assim o clima do dia 37 não depende de quantos números outro subsistema consumiu. Também é possível pular dias sem divergência.
2. Toda ação do jogador fica registrada como `(tick, ação, parâmetros)`. Isso permite *replay* e teste de regressão. O teste roda a mesma safra em 1x e em velocidade máxima sem renderização e compara o *hash* do estado.
3. O número de passos por quadro tem limite, como o `max_physics_steps_per_frame` do Godot. Se o limite for atingido o jogo fica mais lento e o passo não muda. Isso evita a "espiral da morte" e preserva o determinismo.
4. O modo **dormir** roda o mesmo laço de passos sem renderizar até a manhã seguinte. Ele usa o mesmo limite de passos por quadro.
5. O jogo salva e carrega. O arquivo guarda o estado completo, o tick atual, as versões do motor e do pacote e a semente do mundo. O registro de ações do jogador vai junto para permitir *replay*.

## 3. Conjunto de modelos da fase 1

| Módulo | Modelo | Mecanismo | Referência a registrar |
|---|---|---|---|
| Astronomia | Equações do FAO-56 para declinação, fotoperíodo e radiação extraterrestre | Expressões fechadas para qualquer latitude do Brasil | FAO-56 cap. 3 |
| Clima | Série real de estação automática do INMET e curva diurna de temperatura | Auditável contra a fonte e sem calibração estatística | INMET; Parton e Logan |
| ET0 | Penman-Monteith FAO-56 com Hargreaves-Samani de reserva | Balanço de energia mais demanda aerodinâmica do ar | FAO-56 |
| Água no solo | *Bucket* de uma camada com Kc simples, fator de estresse Ks e escoamento por Curve Number | Tanque com ladrão que conserva massa a cada passo e por isso é estável | FAO-56 cap. 8; USDA NEH-4 |
| Física do solo | Capacidade de campo, ponto de murcha e densidade por classe de solo e textura | Tabela com fonte, sem função de pedotransferência ainda | Embrapa (FPTs regionais) |
| Química do solo | Laudo com V% e CTC, calagem pela saturação por bases, NPK por formulação, nitrogênio mineral com lixiviação | Aritmética de inventário e um compartimento dinâmico de nitrato | Embrapa Hortaliças; manuais estaduais |
| Planta | "AquaCrop-lite" com graus-dia, cobertura do dossel, transpiração, produtividade da água e índice de colheita | Biomassa proporcional à transpiração normalizada pela ET0 | FAO AquaCrop |
| Daninhas | Coorte com crescimento logístico de cobertura e perda de produção de Cousens | Competição por luz e água com dano marginal decrescente | Cousens 1985 |
| Irrigação | Regador, gotejo com eficiência e fração de área molhada, tensiômetro virtual | Lâmina calculada pela ETc e manejo pela tensão da água no solo | Embrapa Hortaliças (Circular Técnica 69) |
| Relatório | Atribuição da perda de produção aos fatores Ks, nitrogênio, daninha e V% | Cada fator multiplicativo é registrado por dia | [INFERÊNCIA] |

A fatia vertical usa **alface**. O índice de colheita da alface é alto e quase constante. Isso dispensa modelar florescimento e partição para frutos. O tomate cereja exige esses dois processos e entra na fase 2.

## 4. Modelos adiados

| Modelo | Fase | Motivo do adiamento |
|---|---|---|
| Gerador climático WGEN por estação | 2 | A série real já é determinística e auditável na fase 1 |
| Kc dual e palhada | 2 | Só faz diferença quando a cobertura morta existe no jogo |
| Mineralização de nitrogênio, volatilização de amônia, fósforo lábil e fixado, potássio | 2 | Ganho pequeno num ciclo curto de alface |
| Compostagem com C/N, umidade e temperatura | 2 | Depende de resíduos e de um segundo ciclo |
| Defensivos com dose, carência e meia-vida | 2 | Depende dos dados do Agrofit e do registro de fontes |
| Preparo do solo como mudança de estado | 2 | Depende do microtrator e da deformação do terreno |
| Banco de sementes de daninhas | 3 | Escala de tempo de vários ciclos |
| Consórcio por partição de luz | 3 | Reaproveita o código cultura e daninha da fase 1 |
| Matéria orgânica multianual (RothC ou CENTURY) | 3 | Constante de tempo de anos a décadas |
| Compactação por tráfego e descompactação biológica | 3 | Exige raiz em profundidade e várias safras |
| Escoamento em malha sobre o terreno | 3 | Exige passo curto e malha 2D; o renderizador Compatibility não tem *compute shaders* |
| Dispersão de gases e deriva de pulverização | 4 | Exige campo espacial; até lá as perdas entram como balanço de massa |
| Epidemiologia espacial, hidráulica de gotejo, micronutrientes | 4 | Poucos parâmetros brasileiros consolidados |

## 5. Lacunas em pesquisa (passo B0)
1. Cinética da reação do calcário e curva pH × V% por região.
2. Meia-vida de ingredientes ativos com fonte brasileira.
3. Parâmetros do AquaCrop para alface no Brasil.
4. Curve Number para canteiros.
5. Eficiência de irrigação por método com fonte Embrapa.
6. Potencial da capacidade de campo (−10 ou −33 kPa) por região.
7. Gerador climático para o semiárido.
8. Determinismo entre plataformas.
9. Descompactação biológica por raízes.
10. Formação de compostos orgânicos e inorgânicos no solo.
11. Deformação da malha do terreno no Godot com o renderizador Compatibility.
12. Efeito da declividade na água e na erosão para a campanha em área inclinada.
13. Arquitetura multitaxa com acoplamento mínimo.
