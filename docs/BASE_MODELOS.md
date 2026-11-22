# Base de modelos do simulador

> Documento-base da Fase 0. Ele guarda a escolha dos modelos e os dados levantados em três rodadas de pesquisa. A arquitetura de simulação está em `ARQUITETURA_SIMULACAO.md`. Na Etapa C do `MAPA_DO_PROJETO` o conteúdo passa para os documentos 04, 05, 09 e 14 e este arquivo sai do repositório.
> Número com `[FONTE:ID]` está no registro de fontes. Número sem ID vem de publicação que está na fila de verificação do registro (`docs/referencias/fontes.json`). Ele só entra no pacote depois de registrado, com ID e local.
> Equações vêm de manuais técnicos, livros e artigos. Dados vêm da Embrapa e de órgãos brasileiros. Parâmetro estrangeiro leva o rótulo [PROVISÓRIO].

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
| Agenda única de eventos, *splitting* sequencial, árbitro de recurso e ledger inteiro | 4 | 5 | – | 5 | 5 | – | 5 | Fase 1 |
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
| Gerador ClimaBR (Markov de 1ª ordem) | 4 | 5 | 5 | 5 | 5 | – | 4 | Referência de validação |
| Markov com exponencial mista | 3 | 5 | 4 | 5 | 5 | – | 4 | Fase 3 (modo semiaberto) |
| Encosta por CN, MUSLE e D8 em cascata | 4 | 5 | 4 | 5 | 5 | 3 | 5 | Fase 3 |
| Onda cinemática e *virtual pipes* | 2 | 3 | 2 | 5 | 2 | 3 | 4 | Fase 4 só visual |

## 2. Arquitetura de simulação

Agenda de eventos, relógio, ordem causal, árbitros, ledger, determinismo, núcleo C++, save e replay estão em `ARQUITETURA_SIMULACAO.md`.

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
| Daninhas | Coorte com cobertura logística e perda de Cousens, janela de capina pelo período crítico (§4.8) | Competição com dano marginal decrescente | Cousens 1985; [FONTE:SEMINA-2011-GIANCOTTI] |
| Irrigação | Regador, gotejo com eficiência e fração de área molhada, tensiômetro virtual (gatilho de 15 kPa a 10 cm, §4.3) | Lâmina pela ETc e manejo pela tensão da água | Embrapa Hortaliças |
| Arquitetura | Agenda única, ledger inteiro, árbitros, núcleo C++ com detmath | `ARQUITETURA_SIMULACAO` | APSIM; [FONTE:BOX2D-2024-DETERMINISM] |
| Relatório | Atribuição da perda aos fatores Ks, nitrogênio, daninha e V% | Cada fator multiplicativo é registrado por dia | [INFERÊNCIA] |

A interface de planta já nasce preparada para o árbitro de luz da fase 2. A alface é a cultura da fatia porque o índice de colheita da alface é alto e quase constante. Isso dispensa florescimento e partição para frutos.

### 3.2 Fase 2 (horta completa e campanha em área plana)
1. Água em várias camadas por cascata.
2. Planta híbrida com biomassa igual ao mínimo entre a limitada por luz e a limitada por água (§7), com árbitro de luz.
3. Daninhas com o mesmo código da planta e parâmetros próprios.
4. Calcário com cinética por classe granulométrica, relação pH–V e NPK por formulação (§4.1 e §6).
5. Mineralização simples de nitrogênio.
6. Séries do INMET de vários anos e de estações de todas as regiões. A campanha não usa gerador climático, porque a aleatoriedade fica só nos eventos do modo semiaberto (decisão 40).
7. Densidade do solo simples por preparo e tráfego, malha de canteiro de 5 cm, deformação do terreno e Terrain3D 1.1.x na propriedade (§8).
8. Defensivos com dose e carência do Agrofit e meia-vida provisória com correção de temperatura e umidade (§4.2).
9. Compostagem, palhada com Kc dual, aspersão com molhamento foliar (passo horário), microtrator e tomate cereja.

### 3.3 Fase 3 (encosta, solo vivo e modo semiaberto)
1. Roteamento D8 em cascata, MUSLE e práticas conservacionistas (§8).
2. Solo físico com resistência à penetração, IHO e bioporos, e plantas de cobertura (§5).
3. Troca de Gapon e salinidade, fósforo em dois compartimentos e húmus em dois compartimentos (§6).
4. Consórcio por partição de luz, banco de sementes de daninhas, tempo hidrotérmico.
5. Gerador climático do modo semiaberto. Markov de primeira ordem com quantidades em exponencial mista, validado contra o ClimaBR nas mesmas estações.
6. ZARC na escolha da época de plantio.

### 3.4 Fases 4 e 5
Fase 4 reúne economia, combustível, NPCs, cidades, gases e deriva, micronutrientes por tabelas de especiação geradas offline, RothC, epidemiologia espacial e água visual por *virtual pipes*. Fase 5 reúne os animais, que reaproveitam o ledger de N e C, o árbitro de recurso (pastejo como consumidor) e a compactação por pisoteio.

## 4. Dados levantados

Valores das rodadas 2 e 3 de pesquisa. Os que têm `[FONTE:ID]` estão no registro. Os demais vêm de publicações da fila de verificação e entram no pacote só depois de registrados.

### 4.1 Calcário
1. **Reatividade legal (MAPA).** RE é o percentual que reage em 3 meses. RE = 0,2·F(10–20) + 0,6·F(20–50) + 1,0·F(<50), com partículas retidas na peneira 10 valendo zero. PRNT = PN·RE/100 (Boletim Técnico 6; RBCS 39, 2015).
2. **Cinética observada (Embrapa).** Depois de 6 meses a reação é quase completa e depois de 3 meses mais de 60% do calcário fino já reagiu (Fageria, Embrapa Arroz e Feijão, Infoteca-e). A reação é mais rápida com o solo úmido.
3. **Formulação adotada.** Cada classe granulométrica j segue dM_j/dt = −k_j·f(θ)·M_j. A dissolução do CaCO₃ exige H⁺ em solução. Por isso f(θ) vale zero abaixo do ponto de murcha e cresce linearmente até 1 na capacidade de campo. Para a classe fina, 1 − e^(−90k) ≈ 0,62 dá k ≈ 0,0107 por dia e 87% em 180 dias. Essa calibração é provisória.
4. **pH e V%.** pH em água = 4,50 + 0,025·V (Raij, Cantarella e Zullo 1979, *Bragantia* 38, acesso aberto). Vale para solos de São Paulo com CTC moderada. O próprio artigo diz que a necessidade de calagem por essa relação sai baixa. O pacote guarda o par (a, b) por classe de solo.
5. Não há equação publicada para pH em CaCl₂. A relação 4,1 + 0,025·V é derivação própria e provisória.

### 4.2 Meia-vida de defensivos
1. A Embrapa tem banco parcial (Circular Técnica 48, Produção Integrada de Maçãs). O IBAMA publica perfis ambientais com DT50, mas vários valores estão tarjados por proteção de dados. O Agrofit traz registro, doses e carência e não traz DT50 de forma sistemática.
2. Não há base brasileira aberta e completa. O banco PPDB (Universidade de Hertfordshire) entra como **provisório**, com campo de origem, para ser trocado por valores do IBAMA ou de estudos brasileiros.
3. Decaimento M(t) = M0·exp(−ln2·t/DT50), com DT50 corrigida pela temperatura e pela umidade do solo. Solo tropical quente degrada mais rápido que a medida de laboratório a 20 °C.
4. **Correções confirmadas.** Q10 = 2,58, equivalente a energia de ativação de 65,4 kJ/mol, e correção de umidade de Walker f = (θ/θ_ref)^B com B = 0,7 [FONTE:JRC-2021-FOCUSGW]. A origem do Q10 = 2,58 é a EFSA [FONTE:EFSA-2017-PECSOIL].
5. Nenhum DT50 medido no Brasil foi encontrado até a rodada 3.

### 4.3 Alface

| Parâmetro | Valor | Fonte e situação |
|---|---|---|
| Kc inicial, médio, final | 0,70, 1,05 e 0,95 | Marouelli et al., Embrapa, cap. 5 de *Irrigação e fertirrigação em fruteiras e hortaliças* (fila) |
| Fração de área molhada | 0,95 | mesmo capítulo (fila) |
| Fração de depleção p | 0,30, ajustada pela ETc | [FONTE:FAO-1998-FAO56] tabela 22 |
| Profundidade de raiz | 0,25 m em canteiro | apostila UFAC e Embrapa; [INFERÊNCIA] |
| Gatilho de irrigação | 15 kPa no tensiômetro raso (10 cm), faixa de 10 a 20 kPa, 25 kPa no tensiômetro profundo | Marouelli, Embrapa Hortaliças, Circulares Técnicas 57 e 69 (fila) |
| Temperatura-base | 6 °C da semeadura ao transplante e 10 °C depois | Brunini et al. 1976, *Bragantia* 35 (fila) |
| Temperatura superior | 30 °C | calibração AquaCrop do capítulo IntechOpen (fila); [PROVISÓRIO] |
| Soma térmica | 898 °C·dia em 45 dias e 1050 °C·dia em 52 dias | Fernandes et al. 2019, *Rev. Bras. Meteorologia* 34 (fila) |
| Resposta à temperatura | desenvolvimento máximo perto de 22 °C e menor acima | Brunini et al. 1976 (fila). Exige função com teto |
| Espaçamento | 0,25 × 0,25 m | Embrapa Hortaliças, livro de cultivares (fila) |
| Ciclo | 35 dias no calor e 45 dias nos demais (BRS Mediterrânea) | Embrapa Hortaliças (fila) |
| Produtividade da água WP* | 15 g/m² (19 g/m² como limite superior) | [FONTE:MDPI-2018-KET] e dois estudos da fila; [PROVISÓRIO] |
| Cobertura máxima CCx | 81% | capítulo IntechOpen (fila); [PROVISÓRIO] |
| Cobertura inicial CC0 | densidade de plantas vezes área da muda | regra do AquaCrop [MODELO] |
| Crescimento e declínio do dossel CGC, CDC | 14,3 e 8,0% por dia | capítulo IntechOpen (fila); [PROVISÓRIO] |
| Estresse na expansão e nos estômatos | p_exp 0,25 a 0,55, p_sto 0,50, forma 3 | capítulo IntechOpen (fila); [PROVISÓRIO] |
| Índice de colheita de referência HIo | 95% | capítulo IntechOpen (fila); [PROVISÓRIO] |
| Curva crítica de N | %Nc = 4,56·DW^−0,357 (DW em t/ha de massa seca; válida de 0,9 a 3,4 t/ha) | [FONTE:ISHS-2003-TEI]; [PROVISÓRIO] |
| Eficiência de uso da radiação RUE | 1,44 g/MJ de radiação total interceptada, até 40 dias após a emergência | [FONTE:ANNBOT-1996-TEI]; [PROVISÓRIO] |
| Coeficiente de extinção k | 0,60 | [FONTE:ANNBOT-1996-TEI] com pendência (0,60 ou 0,655); [PROVISÓRIO] |
| Ky | não é parâmetro. Faixa de 0,9 a 1,4 vira teste de validação (mediana publicada perto de 1,1) | seis estudos estrangeiros na fila; nenhum brasileiro |

1. Não há calibração brasileira do AquaCrop para alface. A alface não consta do catálogo do AquaCrop [FONTE:MDPI-2018-KET].
2. O KcTr,x de 0,85 do capítulo IntechOpen é menor que o Kc médio de 1,05 de Marouelli. O pacote deriva o KcTr,x do Kcb do FAO-56 e guarda 0,85 só como teste [INFERÊNCIA].
3. Na fase 1 o Ky não entra no cálculo, porque a resposta ao déficit emerge do Ks e do WP*. O teste de validação exige que a inclinação entre perda relativa de produção e déficit relativo de ET fique entre 0,9 e 1,4 [INFERÊNCIA].
4. Um valor de 410 °C·dia atribuído a Brunini em relatório português não foi verificado e não deve ser usado.

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

A base HYBRAS reúne 445 perfis e 1075 amostras de solos brasileiros com parâmetros de van Genuchten e funções de pedotransferência brasileiras [FONTE:VZJ-2018-HYBRAS]. Ela é a fonte padrão da curva de retenção por classe e textura. Trabalho derivado mostra que a sucção da capacidade de campo medida no campo depende da textura e da gênese do solo. O valor fixo de −10 kPa é uma simplificação operacional [MODELO]. O uso de −6 kPa em solo arenoso ainda não tem fonte primária.

### 4.7 Clima
O ClimaBR (Zanetti, Oliveira e Pruski; validado em 12 localidades de todas as regiões pluviométricas) usa Markov de primeira ordem. Para o semiárido a recomendação é Markov de primeira ordem com quantidades em exponencial mista (Wilks 1998), que captura melhor a cauda de eventos grandes que gera escoamento e erosão. O teste de aceitação é o comprimento dos veranicos. Ordem maior só entra se esse teste falhar. Parâmetros por estação do INMET e por mês. O gerador só roda no modo semiaberto (decisão 40).

### 4.8 Plantas daninhas na alface
1. Com 5% de perda tolerada, o período total de prevenção da interferência na alface Solaris foi de 21 dias após o transplante. A convivência no ciclo todo reduziu a produtividade em 25% [FONTE:SEMINA-2011-GIANCOTTI].
2. Espécies dominantes no mesmo trabalho. *Bidens pilosa*, *Amaranthus hybridus*, *Oxalis corniculata*, *Euphorbia heterophylla*, *Commelina benghalensis* e *Lepidium virginicum* [FONTE:SEMINA-2011-GIANCOTTI].
3. Material da Embrapa para ciclo de 40 a 50 dias indica período anterior à interferência de 19 a 21 dias e período total de prevenção de 34 a 35 dias (Ikeda, fila).
4. A janela de capina do jogo vai de 19 a 35 dias após o transplante. O relatório de safra explica a perda quando a capina cai fora dela [INFERÊNCIA].
5. Os parâmetros I e A de Cousens para alface e os parâmetros de crescimento das daninhas não foram encontrados.

### 4.9 Erosão
1. Fator topográfico de Bertoni e Lombardi Neto. LS = 0,00984·L^0,63·S^1,18, com L em metros e S em porcentagem. A forma aparece igual em várias transcrições abertas (fila). O livro *Conservação do Solo* não foi aberto.
2. O fator P vale 1 para preparo morro abaixo. Os demais valores de P e os fatores C e K por classe de solo ainda não foram transcritos.

## 5. Solo físico e descompactação biológica

Estados por camada. Densidade Ds, fração de bioporos contínuos fbio e rugosidade RR. Todos estão no `PackedFloat64Array`.

1. **Resistência à penetração.** RP = a·θ^b·Ds^c (forma de Busscher). RP sobe quando o solo seca e quando compacta. Por isso um veranico endurece o solo compactado. Tormena et al. ajustaram essa forma num Latossolo sob plantio direto e usaram 2,0 MPa como limite [FONTE:RBCS-1998-TORMENA]. Os coeficientes desse ajuste ainda não foram extraídos. Um ajuste verificado em Latossolo Vermelho distrófico sob integração lavoura-pecuária dá RP = 0,0123·θ^−1,2762·Ds^5,6262 na camada de 0 a 0,10 m e RP = 0,0049·θ^−1,8467·Ds^5,6262 na de 0,10 a 0,20 m, com RP em MPa (fila).
2. **IHO.** Limite superior igual ao menor entre θCC e o θ com 10% de ar. Limite inferior igual ao maior entre θPMP e o θ em que RP chega ao valor crítico. O padrão é 2,0 MPa (Embrapa Trigo e RBCS). Em plantio direto com bioporos citam-se até 3,5 MPa.
3. **Alongamento de raiz.** f_RP vale 1 até cerca de metade do valor crítico, cai linearmente até zero no crítico e a eficiência é f = fbio + (1 − fbio)·f_RP. Raízes da cultura seguinte passam pelos bioporos. Isso explica sem regra especial por que o plantio direto tolera RP maior.
4. **Estresse hídrico (fase 3).** O Ks usa o IHO no lugar de (CC − PMP). O mesmo solo com o mesmo θ pode estressar a planta se estiver compactado.
5. **Compactação por tráfego.** ΔDs = (Ds_max − Ds)·[1 − exp(−σ/σ_c(θ))]·α. A tensão σ é carga sobre área de contato. A tensão crítica σ_c cai com a umidade, então solo úmido compacta mais. É redução de baixa complexidade de Söhne e SoilFlex **proposta pelo projeto** [INFERÊNCIA].
6. **Descompactação.**
   1. Preparo leva Ds ao valor solto, eleva RR e reduz o CN. Depois a reconsolidação é exponencial com a chuva acumulada.
   2. Raízes formam bioporos segundo dfbio/dt = β·(incremento diário de raiz pivotante na camada)·(1 − fbio/fbio_max) − fbio/τ. O parâmetro β é da espécie e τ é a persistência do canal. A densidade do solo não muda numa estação (dDs/dt = 0). O efeito da raiz entra pela macroporosidade e pelos bioporos, que reduzem a RP e aumentam a infiltração [MODELO].
   3. Ciclos de umedecimento e secagem entram como termo pequeno proporcional à argila.
7. **Base empírica.** Num Latossolo muito argiloso compactado sob plantio direto, o nabo forrageiro dobrou a macroporosidade média de 0 a 0,20 m, manteve a RP em até 1,6 MPa contra mais de 2,0 MPa na testemunha e aumentou a infiltração em cerca de 46%, cerca de 4 meses depois do manejo do nabo. A densidade do solo não teve diferença significativa. Cada aumento de 0,01 m³/m³ de macroporosidade correspondeu a cerca de 0,1 MPa a menos de RP (R² = 0,89) [FONTE:RBCS-2008-NICOLOSO].
8. **Calibração.** O nabo forrageiro é a única espécie calibrada. Ele usa as três respostas acima como alvo. A crotalária teve resultado contraditório num Argissolo franco-arenoso (fila), então o efeito dela na RP fica fraco e o efeito na condutividade fica ligado. Braquiária, milheto e guandu ficam desligados até haver dado. Raízes de plantas de cobertura crescem normalmente até Ds de 1,75 Mg/m³ e sofrem deformação severa acima de 1,85 Mg/m³ (fila).
9. **Lacuna.** A persistência do efeito depois do manejo (τ) não foi quantificada em nenhuma fonte verificada. É o maior risco de credibilidade da fase 3.

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

1. **Propriedade.** Terrain3D 1.1.x (GDExtension em C++, licença MIT). A versão 1.1.0 suporta Godot 4.5 a 4.8 e o renderizador Compatibility [FONTE:GH-2026-TERRAIN3D110]. A série 1.x edita na CPU e a série 2.x não vai suportar o Compatibility [FONTE:TOKISAN-2025-T3DGPU]. A versão fica fixada na 1.1.x. Resolução de cerca de 1 m.
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
2. **Farming Simulator 25.** O jogo permite de 1 a 28 dias por mês. Com 1 dia por mês, dormir uma noite avança um mês de calendário agronômico [FONTE:WIKIGG-2024-FS25MANUAL]. O simulador-agro não copia isso. O dia agronômico é físico e a aceleração vem só do multiplicador, do dormir e do pular até o evento (decisão 45). Bônus aditivos fixos. Calcário soma 15% a cada cerca de três colheitas, campo sem daninhas soma 20%, duas adubações somam 22,5% cada, aração soma 15% e rolagem soma 2,5%. Rotação detalhada, pH e N, P e K aparecem em *mods*. Escala de tempo, sono e duração do dia **não foram verificados**.
3. **Lição de projeto.** Os dois jogos usam bônus sem estado. O simulador-agro faz esses efeitos nascerem de estados (pH por V%, daninha por competição, adubo por compartimentos). Os *mods* do Farming Simulator mostram demanda por isso.

## 10. Lacunas abertas e riscos

| # | Lacuna | Situação | Próximo passo |
|---|---|---|---|
| 1 | Calibração brasileira do AquaCrop e Ky da alface | Só calibrações estrangeiras [PROVISÓRIO] | Derivar Ky de experimentos brasileiros de lâminas (rodada 4) |
| 2 | Recomendação estadual de N, P e K para alface e exportação de nutrientes | Não verificada | IAC Boletim 100, CQFS RS/SC, 5ª Aproximação MG, IPA (rodada 4) |
| 3 | Temperatura máxima, pendoamento e queima de bordas da alface tropical | Só Tu = 30 °C do AquaCrop [PROVISÓRIO] | Embrapa Hortaliças (rodada 4) |
| 4 | Eficiência do regador e da mangueira | Sem fonte | Rodada 4; senão, uniformidade medida no jogo [INFERÊNCIA] |
| 5 | Relação pH–V em outras mineralogias e pH em CaCl₂ | Só Raij 1979 para São Paulo | Rodada 4 |
| 6 | Cinética do calcário por classe granulométrica | Só RE legal e Fageria | Rodada 4 |
| 7 | DT50 de agrotóxicos medido no Brasil | Correções FOCUS confirmadas; DT50 do PPDB [PROVISÓRIO] | Rodada 4 e termos de uso do PPDB |
| 8 | Descompactação por braquiária, milheto e guandu; persistência τ | Só nabo calibrado | Rodada 4 (Calonego e Rosolem; Silveira Junior 2012) |
| 9 | Coeficientes de Busscher de Tormena 1998 | Citação confirmada; coeficientes não extraídos | Rodada 4 |
| 10 | Fatores P, C e K da EUPS brasileira | Só LS confirmado | Rodada 4 |
| 11 | Cousens I e A para alface e parâmetros das daninhas | Não encontrados | Rodada 4 (*Planta Daninha*) |
| 12 | INMET horário, Parton e Logan, Hargreaves–Samani no Brasil | Não verificados | Rodada 4 |
| 13 | Ano, título ou URL oficial das publicações da fila | 31 itens na fila do registro | Rodada 4 e BDPA/Infoteca-e |
| 14 | Custo real por célula | Estimativa de ordem de grandeza | Benchmark no passo D1b |
| 15 | Fontes sobre jogos de referência | Comunidade, qualidade baixa | Manuais oficiais (rodada 4) |
