# CLAUDE.md — regras permanentes do projeto `simulador-agro`

> Lido no início de toda sessão do Claude Code. Todo o projeto é conduzido no Claude Code; não há outro agente.
> Estado: `docs/STATUS.md`. Fases, entregáveis, passo a passo, esforço e regra de autonomia: `docs/MAPA_DO_PROJETO.md`. Planejamento: `docs/00_INDICE.md`. Arquitetura de simulação: `docs/ARQUITETURA_SIMULACAO.md`. Decisões: `docs/DECISOES.md`.

## 1. O projeto em resumo
1. Simulador 3D de **microgestão de propriedade rural**, com aparência e mecânicas de *farming simulator* e de Medieval Dynasty. O jogador é o protagonista, em primeira ou terceira pessoa. O produto final é um mundo semiaberto.
2. Foco diferente do *farming simulator*: **manejo técnico** de horta e de várias culturas, canteiro a canteiro. O jogador testa técnicas sem ter uma propriedade.
3. **Conhecimento agronômico é dado, não código:** nenhum número agronômico (dose, espaçamento, ciclo, Kc, limiar de praga, faixa de pH) dentro de cena ou script do motor. Tudo vem do pacote de conteúdo.
4. **Fonte rastreável:** todo parâmetro do pacote cita uma publicação registrada em `docs/referencias/fontes.json` (fonte primária: **Embrapa**), com página ou seção. Política de fontes complementares: `00_INDICE` §1.4.
5. **Mundo calculado:** o estado de solo, água, planta, pragas e doenças evolui por modelos com passo fixo em tempo simulado, numa agenda única de eventos, com relógio 24 para 1 e multiplicador 2x/4x/8x (`docs/ARQUITETURA_SIMULACAO.md`; modelos em `docs/BASE_MODELOS.md`). Nada é pintado à mão. Aleatoriedade só em eventos de jogo.
6. Motor **Godot 4.7.2** (renderizador Compatibility) mais o núcleo C++ `agro_core` em `engine/native/`, que guarda e muda todo o estado do mundo calculado; conteúdo em `packs/<pacote>/`. Fase atual: **Fase 0** (modelos, fontes e núcleo; nada no Godot).
7. Estilo visual: low-poly, **cores chapadas, sem texturas, sem sombras em tempo real**.
8. Alvo de desempenho: GPU integrada (Radeon R5 via ANGLE/D3D11), 7,5 GB de RAM, pouco disco livre.
9. **Base física primeiro:** clima e solo não conhecem nenhuma cultura e são validados contra dados observados sem cultura (Etapa V). Cultura nova entra só como pacote, pelo fluxo de ingestão (`docs/19_INGESTAO_DE_CULTURA.md`). A campanha forma o jogador; o modo semiaberto é o laboratório de experimentos.

## 2. Quem faz o quê e onde
- **O usuário faz só quatro coisas:** U1 manda fotos de objetos (ferramenta, implemento, planta, praga) e aprova a prévia do modelo 3D; U2 usa o simulador e conta o que estranhou; U3 concede acesso (notebook ligado com o app Claude Desktop aberto quando avisado; aprova os pedidos de acesso); U4 responde perguntas essenciais e valida o conteúdo técnico.
- **O agente faz todo o resto**, com permissão total, e pergunta só o essencial (decisão que só o usuário pode tomar, ou irreversível). A um "continue", segue o `MAPA_DO_PROJETO` sem pedir aprovação a cada passo, só nos passos do esforço escolhido para a sessão (Opus 5.5; `médio`, `alto` ou `extra`, `MAPA_DO_PROJETO` §1.2), e para só nos pontos P1–P4 (`MAPA_DO_PROJETO` §1.1).

| Ambiente | Para quê |
|---|---|
| **Sessão na nuvem** (Claude Code no claude.ai, repositório anexado) — principal | Documentos, código, Git e PRs; Godot 4.7.2 para Linux *headless* (importar, testar, exportar o `.exe`); Blender como módulo `bpy`; Python, Node e Playwright. Não depende do notebook. |
| **GitHub Actions** | CI a cada PR; testes e exportação do Godot; Releases. |
| **Notebook do usuário**, a partir da sessão na nuvem | Arquivos pela ponte do app Claude Desktop; tela pelo controle de aplicativos (abrir o `.exe`, ler o painel F3); navegador logado no GitHub para o que a API não permite. |
| **Sessão Local** (Claude Desktop → aba *Code*) | Reserva, se algum passo exigir terminal no Windows. |

## 3. Regras fixas
1. No notebook, só as pastas `C:\projetos\simulador-agro-testes`, `C:\projetos\simulador-agro` e `C:\ferramentas`, e só os aplicativos do projeto (executável de teste, Godot, navegador no GitHub).
2. Todo trabalho em *branch* + PR; *merge* (squash) **só com CI verde**. Nunca `push --force` na `main` nem reescrita de histórico.
3. **Planejamento antes do código do jogo:** as ferramentas já estão fixadas e testadas (herdadas do `simulador-normas`); nada de criar o projeto Godot do simulador antes de concluída a Etapa C do `MAPA_DO_PROJETO`, salvo pedido do usuário. O núcleo C++ (`engine/native/`) não depende do Godot e pode avançar antes (passos B4 a B6).
4. Repositório **público**: nunca comitar tokens, `.env`, credenciais, fotos do usuário (`art/fotos/` fica fora do Git), dados pessoais ou PDFs de terceiros (`docs/referencias/pdf/` fica fora do Git; o registro guarda só a referência e a URL).
5. Nunca escreva um parâmetro agronômico de memória. Todo número do pacote leva `fonte` (ID do registro) e `local` (página, tabela ou seção). Rode `tools/conferir_fontes.py` depois de mexer em `docs/`, `packs/` ou no registro.
6. **Inferência é provisória:** toda simplificação, interpolação ou ligação entre dado e mecânica escrita pelo agente é inferência até o usuário validar. Marque com o rótulo certo e deixe "a validar".
7. Rótulos: **`[FONTE:ID]`** (dado de publicação registrada), **[PRÁTICA]** (só cenário, nunca cobrado), **[INFERÊNCIA]** (decisão de projeto), **[MODELO]** (simplificação matemática do simulador sobre um dado de fonte), **[VERIFICAR]** (fato externo a confirmar), **[VERIFICAR NA FONTE]** (número ainda não conferido na publicação), **[PROVISÓRIO]** (parâmetro estrangeiro ou sem calibração brasileira, sempre com fonte e a ser trocado).
8. Os `.md` descrevem **só o estado atual** e os próximos passos, com o mínimo de texto: cada assunto num só lugar, sem tabela de revisões nem comparação com o passado. O histórico fica no Git.
9. Limpe *builds* e arquivos temporários; no notebook, apague os executáveis de teste que saírem de uso.
10. Limites da sessão na nuvem: sem API GraphQL do GitHub, sem mudar configurações do repositório e sem apagar *branches*. PR pela API REST (`gh api repos/pedrooalvess1-glitch/simulador-agro/pulls --input pr.json`); o resto pelo navegador, já logado no GitHub. Os nomes dos *jobs* do CI serão *checks* obrigatórios da regra da `main`: renomear um *job* exige atualizar a regra no mesmo PR.
11. **Determinismo:** estado do mundo só no `agro_core`, compilado com `-ffp-contract=off` e sem `-ffast-math`; funções elementares só de `agro::detmath`; estoques conservados só por `Ledger::transfer`; nenhum estado calculado em GDScript nem dependente do tempo do quadro; sorteio só em eventos de jogo (`agro::rng`). Mudou o núcleo, rode `tools/testar_nucleo.sh`; mudou o detmath de propósito, atualize `engine/native/golden/` no mesmo PR e explique por quê.

## 4. Estrutura
```
docs/             00_INDICE (entrada), MAPA_DO_PROJETO, STATUS, ARQUITETURA_SIMULACAO, BASE_MODELOS, DECISOES, CONVENCOES, GLOSSARIO_DO_PROJETO, _MODELO_DOCUMENTO, NN_*.md (planejamento), interface/ (wireframes)
docs/referencias/ fontes.json (registro de fontes) · FONTES.md (como registrar) · pdf/ (local, ignorada)
art/              blender/ (scripts .py) · glb/ · previews/ · fotos/ (local, ignorada)
engine/           motor genérico (não conhece nenhuma cultura) · native/ núcleo C++ agro_core (detmath, agenda, relógio, ledger, alocação, RNG de eventos, testes, golden/)
packs/            pacotes de conteúdo agronômico (culturas, solos, clima, pragas, insumos)
tools/            conferir_fontes.py (+ teste) · checar_links.py (+ teste) · testar_nucleo.sh · medir_erro_detmath.py · ambiente/ (versões fixadas, instaladores, cena de fumaça)
.github/          CI · modelos de issue (bug, conteudo-tecnico, desempenho) e de PR
```

## 5. Comandos
```bash
# Registro de fontes e citações (também no CI)
python3 tools/conferir_fontes.py          # testes do próprio verificador: tools/test_conferir_fontes.py
# Links internos entre os .md (também no CI)
python3 tools/checar_links.py             # testes: tools/test_checar_links.py
# Núcleo C++ (também no CI, Linux e Windows): testes, hash dos vetores do detmath e erro em ulp (pip install mpmath)
tools/testar_nucleo.sh                    # --rapido = só g++ -O2; detalhes em engine/native/Makefile
# Ferramentas na nuvem (versões e somas em tools/ambiente/versoes.env; Godot também no CI)
GODOT=$(tools/ambiente/instalar_godot.sh | tail -1)            # + --templates windows,linux para exportar
"$GODOT" --headless --path <projeto> --export-release "Windows Desktop" build/app.exe
BPY=$(tools/ambiente/instalar_bpy.sh | tail -1)                # Blender 4.5.10 LTS como módulo (Python 3.11)
"$BPY" art/blender/<objeto>.py                                  # teste do pipeline: tools/ambiente/teste_blender.py <saida.glb> <saida.png>
tools/ambiente/instalar_gdunit4.sh <projeto>                    # addon gdUnit4 v6.2.3 em addons/gdUnit4
"$GODOT" --headless --path <projeto> --import                  # antes do primeiro teste ou exportação
GODOT_BIN="$GODOT" <projeto>/addons/gdUnit4/runtest.sh --headless --ignoreHeadlessMode -a res://tests   # sai com 100 se um teste falhar
# Captura de cena sem GPU (renderização de software, Mesa llvmpipe)
LIBGL_ALWAYS_SOFTWARE=1 xvfb-run -a "$GODOT" --path <projeto> --rendering-driver opengl3 -- <argumentos da cena>
```
Versões fixadas: **Godot 4.7.2** (`4.7.2.stable.official.ed1daf0bf`), **Blender/bpy 4.5.10 LTS**, **gdUnit4 v6.2.3**. O gdUnit4 recusa o modo *headless* sem `--ignoreHeadlessMode`; testes que dependem de `InputEvent` não funcionam *headless*. A prévia do Blender no Workbench usa `material.diffuse_color`; o glTF usa a *Base Color* do Principled BSDF: o script define as duas.

## 6. Pipeline foto → modelo 3D
1. Foto chega na conversa (guardada em `art/fotos/`, fora do Git).
2. Script `art/blender/<objeto>.py`: low-poly, cores da paleta (hex sRGB convertido para linear, como em `tools/ambiente/teste_blender.py`), sem textura, escala real em metros, origem no chão (o eixo Z do Blender vira o Y do Godot).
3. Exporta `art/glb/<objeto>.glb`; renderiza `art/previews/<objeto>.png` (fundo neutro, 3/4 + frente).
4. Mostra a prévia ao usuário (U1) e só então integra ao catálogo.
5. Plantas têm um modelo por estádio fenológico (ex.: muda, vegetativo, floração, frutificação, ponto de colheita), trocado pelo motor conforme o estado calculado.

## 7. Laço de trabalho
ler `STATUS` e `MAPA_DO_PROJETO` → pegar o próximo passo do esforço da sessão → *branch* → implementar → testar → PR → CI verde → *merge* → atualizar `STATUS`, `MAPA_DO_PROJETO` e `DECISOES` → próximo passo. Cada Release é testada no notebook pelo agente. Ao fim da sessão, avisar o usuário em poucas linhas.

## 8. Como falar com o usuário
Português do Brasil; termos técnicos em inglês quando forem do jargão. Respostas numeradas, ponto a ponto, explicando o mecanismo de cada decisão ("use X porque é melhor" não serve). Passos bem separados, sem escrita compacta. Ele conhece LLMs; não explique IA. Ele chama o assistente de "Claudinho".
