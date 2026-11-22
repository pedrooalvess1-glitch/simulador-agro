# Roteiro do projeto

> Passos, ordem de execução, esforço e regra de autonomia. Lido em toda sessão, junto com `CLAUDE.md` e `docs/STATUS.md`.
> 🤖 = passo do agente · 👤 = passo do usuário (U1–U4 do `CLAUDE.md` §2). Modelo: **Opus 5.5**; `médio`, `alto` e `extra` = esforço sugerido (§2).

---

## 1. Regra de autonomia

O usuário não aprova passo a passo. A um "continue", o agente segue os passos pendentes do esforço da sessão (§2), na ordem da §3, um PR por item, até acabar o limite da sessão ou chegar a um **ponto de parada**:

| # | Situação | O que o agente faz |
|---|---|---|
| P1 | Decisão que só o usuário pode tomar: escopo de cultura ou região, política de fontes, gosto didático, prioridade | Pergunta em uma frase, com a opção recomendada, e segue com o que não depende da resposta |
| P2 | Ação que não dá para desfazer: apagar *branch*, Artifact ou arquivo que não seja dele; reescrever histórico; publicar fora do repositório | Prepara tudo, descreve a ação e espera o "sim" |
| P3 | Precisa do usuário fisicamente: foto (U1); notebook ligado, aprovação de acesso ou login no navegador (U3) | Pede só isso e segue com outro item |
| P4 | CI vermelho que não consegue consertar após 3 tentativas, ou dado que não bate com a publicação citada | Para o item, explica a causa e passa ao seguinte |

Fora desses casos, o agente decide, registra a decisão em `docs/DECISOES.md` quando ela for de projeto, e segue.

**Para abrir uma sessão:** anexar o repositório, escolher Opus 5.5 com o esforço do próximo passo (`STATUS` §1) e escrever "Claudinho, continue o simulador-agro."

---

## 2. Esforço

| Nível | Critério |
|---|---|
| `médio` | Trabalho mecânico ou com modelo pronto; o CI pega o erro e refazer é barato |
| `alto` | Desenho com inferência sobre o conteúdo agronômico ou contrato usado por vários módulos |
| `extra` | Fundação: esquema do conhecimento, modelo solo-planta-atmosfera, arquitetura e revisão cruzada |

1. O usuário escolhe o esforço no seletor do app; ele vale para a sessão inteira.
2. Numa sessão de esforço X, o agente executa os passos pendentes de nível X, na ordem da §3.
3. Se um passo de nível X depende de um passo de nível menor pendente, o agente faz o pré-requisito no escopo mínimo.
4. O agente não executa passo de nível maior que o da sessão.

---

## 3. Passos pendentes, na ordem de execução

As etapas A, B e C formam a **Fase 0** do `00_INDICE` §7. As etapas D e E entregam a **Fase 1**. As fases 2 a 5 ganham etapas próprias quando a Fase 1 terminar. A Etapa F (arte) acontece quando chegar uma foto. Nada é produzido no Godot antes do fim da Etapa C.

### Etapa A — repositório
- [x] **A1.** 🤖 Repositório `simulador-agro` criado a partir da infraestrutura do `simulador-normas`.
- [ ] **A2.** 🤖 `médio` Pelo navegador logado: regra da `main` (PR obrigatório, *checks* do CI obrigatórios, histórico linear, sem *push* forçado), apagar *branch* no *merge*, rótulos das issues (`bug`, `conteudo-tecnico`, `desempenho`, `docs`, `ideia`, `arte`).
- [x] **A3.** 👤 Perguntas iniciais respondidas (decisões 7, 10, 14 e 15).

### Etapa B — base de modelos e acervo de fontes
- [x] **B0.1.** 🤖 Primeira pesquisa de modelos: catálogo por domínio, conjunto da fase 1, arquitetura de tempo (`BASE_MODELOS`).
- [ ] **B0.2.** 🤖 `extra` Segunda pesquisa: lacunas do `BASE_MODELOS` §5, avaliação dos candidatos pelos critérios C1–C7 e arquitetura multitaxa com acoplamento mínimo. Atualizar o `BASE_MODELOS`.
- [ ] **B1.** 🤖 `alto` Registrar em `docs/referencias/fontes.json` as publicações que sustentam os modelos da fase 1 e os dados da alface (FAO-56, AquaCrop, INMET, Embrapa Hortaliças, manuais estaduais, SiBCS), com URL e data de acesso. Ampliar `dominios_permitidos` conforme a política do `00_INDICE` §1.4.
- [ ] **B2.** 🤖 `alto` Conferir a licença e as condições de reprodução de cada publicação; registrar no campo `licenca`.
- [ ] **B3.** 🤖 `médio` Levantar as fontes da Fase 2: tomate cereja, compostagem, palhada, defensivos (Agrofit), aspersão.

### Etapa C — documentos de planejamento (um PR por documento)
Para cada documento da `00_INDICE` §5: ler `CLAUDE.md`, a `00_INDICE`, o `BASE_MODELOS`, `DECISOES` e os documentos já escritos; escrever no formato do `_MODELO_DOCUMENTO`; todo número com fonte; rodar os verificadores; PR e *merge* com CI verde. O `BASE_MODELOS` sai do repositório na revisão cruzada, depois de absorvido.

| Ordem | Documento | Esforço | Por que nesta posição |
|---|---|---|---|
| 1 | 01 Visão e pedagogia | `alto` | Define o que é aprender manejo jogando e os dois modos de jogo |
| 2 | 09 Relógio e clima | `extra` | O relógio é a base de todos os modelos |
| 3 | 04 Modelo de conhecimento | `extra` | Formato em que todo o conteúdo é escrito |
| 4 | 05 Modelo solo-planta-atmosfera | `extra` | O que torna o mundo calculado |
| 5 | 15 Save, replay e determinismo | `alto` | O formato de save é caro de mudar depois |
| 6 | 14 Pacote inicial | `alto` | Dados reais da fatia vertical |
| 7 | 10 Fitossanidade | `alto` | Daninhas na fase 1; pragas, doenças e defensivos preparados para a fase 2 |
| 8 | 18 Protagonista e câmera | `alto` | Como o jogador age no mundo |
| 9 | 08 Interação e operações | `alto` | Como o jogador muda o mundo |
| 10 | 07 Catálogo | `alto` | Ferramentas, instrumentos e máquinas |
| 11 | 06 Mundo 3D e terreno | `alto` | Propriedade, deformação do terreno, declividade |
| 12 | 11 Campanha | `alto` | Lições do plano ao inclinado |
| 13 | 12 Avaliação | `alto` | Como o resultado é medido e explicado |
| 14 | 02 Arquitetura geral | `extra` | Consolida os contratos acima |
| 15 | 13 Interface | `médio` | Segue os contratos do 02 |
| 16 | 03 Plataforma e desempenho | `médio` | Vegetação em massa, malha na CPU, orçamento |
| 17 | 17 Pipeline de arte | `médio` | Plantas por estádio e máquinas |
| 18 | 16 Testes e CI | `alto` | Testes do modelo contra as fontes e teste 1x × 8x |
| 19 | 21 Roadmap | `médio` | Sequência de entrega das fases 1 a 5 |
| 20 | 19 Novo pacote | `médio` | Só depende do 04 e do 14 |
| 21 | 20 Modo semiaberto e eventos | `alto` | Fase 3 |
| 22 | Revisão cruzada de `docs/` | `extra` | Contradições, lacunas, dependências circulares; absorve e retira o `BASE_MODELOS` |

### Etapa D — esqueleto Godot e fatia vertical
- [ ] **D1.** 🤖 `médio` Projeto Godot (Compatibility, 4.7.2), gdUnit4, CI de testes e de exportação do `.exe` Windows.
- [ ] **D1a.** 🤖 `extra` Relógio de passo fixo com camadas L1–L3, multiplicador 2x/4x/8x, limite de passos por quadro, dormir e teste 1x × 8x (documento 09).
- [ ] **D2.** 🤖 `médio` Abrir o primeiro `.exe` da Release no notebook e conferir se abre.
- [ ] **D3.** 🤖 `alto` Camada base: eventos, salvar e carregar, RNG por subsistema, registro de ações, log (documento 15).
- [ ] **D4.** 🤖 `extra` Carregador e validador do pacote (documento 04).
- [ ] **D5.** 🤖 `extra` Modelos da fase 1 (`BASE_MODELOS` §3; documento 05), com testes contra valores das fontes.
- [ ] **D6.** 🤖 `médio` Propriedade com um canteiro plano e protagonista em primeira e terceira pessoa (documentos 06 e 18).
- [ ] **D7.** 🤖 `alto` `act()` com as operações da fatia vertical (documentos 02 e 08).
- [ ] **D8.** 🤖 `alto` Safra completa e relatório com raio-x (documentos 11, 12 e 13).
- [ ] **D9.** 🤖 `médio` Jogar no notebook e medir o FPS (painel F3). 👤 U2 jogar e dizer o que ficou confuso.

### Etapa E — ciclo de correção (repetido)
- [ ] **E1.** 👤 U2 conta o problema; 🤖 `médio` o agente busca o log no notebook e abre a issue.
- [ ] **E2.** 🤖 Corrigir em *branch*, PR, CI verde, *merge*, Release numerada. Esforço: o do passo que produziu a parte com defeito.

### Etapa F — fotos → modelos 3D (quando chegar uma foto)
- [ ] **F1.** 👤 U1 manda a foto: objeto inteiro, de frente e de lado, fundo limpo e, se der, uma medida conhecida; sem pessoas ou documentos.
- [ ] **F2.** 🤖 `médio` Script `art/blender/<objeto>.py` (`CLAUDE.md` §6): `.glb` e prévia `.png`.
- [ ] **F3.** 👤 U1 responde "parece / não parece".
- [ ] **F4.** 🤖 `médio` Integrar ao catálogo, dentro do orçamento de polígonos (`00_INDICE` §3.3).
