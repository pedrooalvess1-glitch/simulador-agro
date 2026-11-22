# Simulador Agro: índice mestre e decisões de infraestrutura

> Documento de **entrada** do projeto. Ele é lido antes de qualquer outro.
> Objetivo: um simulador 3D de propriedade rural em que o jogador vive como protagonista e aplica o conhecimento técnico publicado pela Embrapa no manejo de solo, água, nutrição, fitossanidade, calendário e colheita.
> Infraestrutura herdada do repositório `simulador-normas`: Godot fixado, CI, verificador de links, pipeline Blender, convenções e modelo de operação. O conteúdo é novo.
>
> Rótulos: `[FONTE:ID]` · [PRÁTICA] · [INFERÊNCIA] · [MODELO] · [VERIFICAR] · [VERIFICAR NA FONTE] (definições no `CLAUDE.md` §3, regra 7).
>
> **Fase atual:** Fase 0 (modelos, fontes e núcleo C++). Nada é produzido no Godot nesta fase. Fases, entregáveis e passo a passo em `MAPA_DO_PROJETO.md`. Modelos e dados em `BASE_MODELOS.md`. Arquitetura de simulação em `ARQUITETURA_SIMULACAO.md`.

---

## 1. Requisitos de base

| # | Requisito | Consequência no projeto |
|---|---|---|
| R1 | **Todas as ferramentas gratuitas** | Godot (MIT), Blender (GPL), Git/GitHub (plano gratuito), gdUnit4 (MIT). |
| R2 | **Simulador de treinamento antes de produto visual** | Low-poly com cores chapadas. O esforço vai para o modelo agronômico, a interação e o *feedback*. |
| R3 | **Rodar bem em GPU integrada com 2 GB** | Renderizador Compatibility, sem texturas, sem sombras em tempo real, orçamento fixo (§3.3). |
| R4 | **Conhecimento técnico como centro** | Todo elemento do modelo tem fundamentação em livro, publicação da Embrapa ou manual técnico (§1.4). |
| R5 | **Microgestão** | A unidade de manejo é o canteiro (horta) ou o talhão (lavoura), com estado próprio de solo, água, cultura e sanidade. |
| R6 | **Protagonista jogável** | O jogador anda pela propriedade e opera ferramentas e máquinas com o próprio personagem, em primeira pessoa ou em terceira pessoa. |
| R7 | **Mundo semiaberto** | O produto final é uma propriedade rural navegável. Economia, combustível, NPCs, cidades e animais entram em fases futuras (§7). |
| R8 | **Brasil inteiro** | Modelos parametrizados por região, classe de solo e clima. Nenhum modelo pode supor um único estado. |
| R9 | **Tempo simulado fiel** | Relógio de 24 para 1 com 2x, 4x e 8x, dormir, salvar e carregar, resultado idêntico em qualquer velocidade e no Linux e no Windows (`ARQUITETURA_SIMULACAO`). |
| R10 | **Extensível** | Separação rígida motor × pacote (§4). Cultura nova é dado novo, sem código novo. |
| R11 | **Interferência mínima do usuário** | O agente opera ferramentas, repositório e notebook. O usuário só faz U1–U4 (`CLAUDE.md` §2). |

### 1.1 Modelo de operação
Igual ao do `simulador-normas`. Todo o projeto é conduzido no Claude Code, com permissão total, trabalho contínuo e pontos de parada P1–P4 (`MAPA_DO_PROJETO` §1.1).

### 1.2 Público e uso
O usuário é o primeiro público. Ele é engenheiro, tem horta própria e quer testar técnicas de manejo antes de aplicá-las. Um segundo público possível são estudantes de ciências agrárias e pequenos produtores [INFERÊNCIA].

### 1.3 Modos de jogo
1. **Campanha.** Ensina o básico do cultivo de uma espécie e do manejo, passo a passo. Começa numa área de manejo plana. Depois avança para uma área inclinada, onde escoamento e erosão passam a pesar.
2. **Semiaberto.** A propriedade fica livre para o jogador. Eventos aleatórios (veranico, chuva intensa, surto de praga) acontecem só neste modo.
3. O simulador pode mostrar fenômenos difíceis de observar no campo. Por isso cada resultado vem com a explicação causal no relatório de safra.

### 1.4 Política de fontes
1. Todo elemento do modelo tem fundamentação em livro, publicação da Embrapa ou manual técnico.
2. **Formulação do modelo** (equações e premissas) pode vir de manual técnico internacional, livro ou artigo revisado por pares. Exemplos são o FAO-56 e o manual do AquaCrop.
3. **Parâmetros e dados** vêm de preferência da Embrapa. Na falta, vêm de outro órgão público brasileiro competente (INMET para clima, MAPA/Agrofit para defensivos, institutos estaduais e manuais estaduais de adubação). Parâmetro estrangeiro só entra marcado como provisório e com fonte.
4. Todo parâmetro do pacote cita o ID do registro `docs/referencias/fontes.json` e o local (página, tabela, seção). O `tools/conferir_fontes.py` barra no CI citação a ID inexistente e dado sem fonte.
5. Quando duas publicações divergirem, o pacote guarda as duas com região e ano. O motor usa a da região do cenário [INFERÊNCIA].
6. PDFs de terceiros ficam fora do Git (`docs/referencias/pdf/`). O registro guarda a referência, a URL e a data de acesso.
7. A busca na Embrapa começa pelo BDPA e pela Infoteca-e (`docs/referencias/FONTES.md` §4).

---

## 2. Princípios do projeto

1. **Conhecimento é dado, não código.** Nenhum número agronômico dentro de cena ou script do motor.
2. **O mundo é calculado.** Solo, água, planta, pragas e doenças evoluem por modelos com passo fixo em tempo simulado.
3. **Um único ponto de mudança do mundo.** Toda operação passa por `act(verbo, alvo, insumo)`. Interface, testes e replay usam o mesmo caminho.
4. **Camadas.** A camada de baixo nunca chama a de cima. A comunicação de volta é por eventos (*signals*).
5. **Acoplamento mínimo.** Tudo que muda o mundo passa por uma agenda única de eventos. Módulos não se conhecem e falam apenas com donos de recurso que arbitram oferta e demanda (`ARQUITETURA_SIMULACAO` §3 a §5).
6. **Tudo reproduzível.** Uma safra é definida por `(versão do pacote, versão do motor, semente do mundo, série climática, registro de ações)`. Os modelos de processo são determinísticos. Sorteio só existe em eventos de jogo. O resultado é o mesmo no Linux e no Windows (`ARQUITETURA_SIMULACAO` §7 e §8).
7. **Conservação exata.** Água, nutrientes e carbono são estoques inteiros movidos por transações num ledger. O balanço fecha sem tolerância (`ARQUITETURA_SIMULACAO` §6).
8. **Verdade invisível.** O estado real do solo e da planta só aparece por medição. Análise de solo com prazo de laudo, tensiômetro, amostragem de pragas e observação de sintomas. O raio-x aparece no relatório de safra e no modo aprendiz.
9. **Rastreabilidade.** Todo número leva fonte e local. Toda simplificação do simulador leva [MODELO].
10. **Verificável por máquina.** Esquema dos dados e citações conferidos no CI desde o primeiro dia.
11. **Leve por padrão.** Recurso visual que não ensina nada não entra.

### 2.1 O que vem dos jogos de referência e o que muda

| Mecânica | Medieval Dynasty e Farming Simulator | Neste simulador |
|---|---|---|
| Personagem | Protagonista em primeira pessoa (Medieval Dynasty) ou em máquinas (Farming Simulator) | Protagonista em primeira e terceira pessoa, a pé ou em máquinas pequenas |
| Campo | Grade de quadrados com ordem ritual de operações | Grade de células por canteiro, com a mesma ordem de operações e estado agronômico em cada célula |
| Fertilidade | Fertilizante binário por quadrado ou bônus aditivos fixos de produtividade | Inventário de nutrientes, V% e fatores multiplicativos causais (Ks, nitrogênio, daninha) |
| Tempo | Estações discretas, dia acelerado, dormir para avançar | Graus-dia e clima diário, relógio 24 para 1 com 2x, 4x e 8x, dormir para avançar |
| Análise de solo | Mapa de pH e nitrogênio com alvo de solo temperado (Farming Simulator) | Laudo com atraso e recomendação pelos manuais brasileiros |
| Daninhas | Penalidade fixa por estágio | Competição pela hipérbole de Cousens e capina no momento certo |
| Trabalho | Delegação a trabalhadores (Medieval Dynasty) | Fase futura com NPCs |
| Terreno | Sulcos como troca de material | Deformação da malha do terreno ao arar, passar o microtrator e cavar |

---

## 3. Plataforma

### 3.1 Stack
Herdada e já testada: Godot 4.7.2 (GDScript, Compatibility), Blender 4.5.10 LTS → glTF, gdUnit4 v6.2.3, GitHub Actions. Versões e somas em `tools/ambiente/versoes.env`; comandos no `CLAUDE.md` §5.
Núcleo do mundo calculado em C++17 (`engine/native/`), compilado com MinGW-w64 para Windows, a mesma família de compilador das builds oficiais do Godot [FONTE:GODOT-2026-COMPWIN]. Na fase 1 ele entra no Godot como GDExtension (godot-cpp).

### 3.2 GPU integrada
**Mecanismo.** GPU integrada divide a RAM do sistema e tem pouca banda de memória. O que derruba o desempenho é, em ordem, textura grande, sombra em tempo real, pós-processamento e número de *draw calls*. Medição herdada na Radeon R5: ~496 FPS sem sombra e ~75 FPS com sombra numa cena trivial; ~250–280 FPS no `.exe` da cena de fumaça.

Restrições específicas deste projeto:
1. **Vegetação em massa.** Plantas repetidas com `MultiMeshInstance3D`. Lavoura grande vira representação por talhão. Nível de detalhe por distância.
2. **Sem *compute shaders*.** O renderizador Compatibility não oferece *compute shaders* e não há plano de suporte [VERIFICAR]. Simulação em malha (água no terreno, deformação) roda na CPU em malha grossa ou em GDExtension C++.
3. **Terreno.** Terrain3D 1.1.x para a propriedade e malha própria de 5 cm para cada canteiro (`BASE_MODELOS` §8). A versão 1.1.0 suporta Godot 4.5 a 4.8 e o Compatibility [FONTE:GH-2026-TERRAIN3D110]. A série 2.x não vai suportar o Compatibility [FONTE:TOKISAN-2025-T3DGPU].
4. **Física fora do multiplicador.** Protagonista e máquinas rodam em tempo real. Só a ação quantizada entra no mundo calculado (`ARQUITETURA_SIMULACAO` §3.3).

### 3.3 Orçamento de desempenho

| Métrica | Limite (perfil Leve) |
|---|---|
| FPS na GPU integrada de referência | ≥ 30 estável (meta 60) |
| Triângulos visíveis | ≤ 150 mil |
| *Draw calls* | ≤ 200 |
| Memória de texturas (só fontes e interface) | ≤ 64 MB |
| Simulação agronômica por quadro | dentro de 4 ms de CPU [INFERÊNCIA]; custo por célula a medir no passo D1b (`ARQUITETURA_SIMULACAO` §9) |
| Tamanho do instalador | ≤ 200 MB |

---

## 4. Arquitetura em duas partes

`engine/native/` já existe. As outras subpastas de `engine/` e as de `packs/` são criadas com o projeto Godot (Fase 1).

```
engine/                  motor genérico: não conhece nenhuma cultura
  native/   núcleo C++ agro_core: agenda de eventos, relógio, ledger, árbitros, detmath, RNG de eventos, modelos em ordem causal, act(), save
  bridge/   GDExtension (godot-cpp) com step_to, act, read, save e load
  world/    propriedade, terreno deformável, câmera, protagonista, máquinas (GDScript)
  game/     campanha, modo semiaberto, avaliação (GDScript)
  ui/       HUD, caderno de campo, laudos, relatório de safra (GDScript)
packs/
  <pacote>/
    crops/    culturas: graus-dia, Kc, parâmetros de dossel e de produtividade da água, espaçamento, janelas
    soils/    perfis de solo: textura, MO, pH, CTC, capacidade de campo, densidade
    climate/  estações e séries climáticas
    pests/    pragas, doenças e plantas daninhas
    inputs/   corretivos, fertilizantes, defensivos registrados, sementes
    recs/     tabelas de recomendação (calagem, adubação, irrigação)
    scenarios/ propriedades, campanha e eventos
    tests/
```

1. O motor conhece **variáveis de estado, processos e operações**. Valores e tabelas vêm do pacote.
2. Pacotes se combinam (cultura + região + manual estadual) [INFERÊNCIA; detalhe no documento 04].

---

## 5. Mapa dos documentos de planejamento

Formato: `_MODELO_DOCUMENTO.md`. Ordem, esforço e estado: `MAPA_DO_PROJETO` §3.2.

| Nº | Documento | Pergunta que responde |
|---|---|---|
| 01 | Visão e pedagogia | O que é aprender manejo jogando; campanha e modo semiaberto; modo aprendiz |
| 02 | Arquitetura geral | Camadas, contratos, eventos, `act()` |
| 03 | Plataforma e desempenho | Perfis, vegetação em massa, malha na CPU, orçamento |
| 04 | Modelo de conhecimento | Esquema dos dados agronômicos e das citações |
| 05 | Modelo solo-planta-atmosfera | Variáveis de estado e processos de água, química, biologia e planta |
| 06 | Mundo 3D e terreno | Propriedade, terreno deformável, canteiros, declividade |
| 07 | Catálogo | Ferramentas, instrumentos de campo, máquinas, insumos |
| 08 | Interação e operações | Verbos, pré-condições, efeitos |
| 09 | Relógio e clima | Camadas de taxa, multiplicador, dormir, séries e gerador climático |
| 10 | Fitossanidade | Pragas, doenças, plantas daninhas, defensivos |
| 11 | Campanha | Lições, progressão do plano ao inclinado |
| 12 | Avaliação | Produtividade, qualidade, custo, saúde do solo, explicação |
| 13 | Interface | HUD, caderno de campo, laudos, relatório de safra |
| 14 | Pacote inicial | Alface, solo e estação da fatia vertical |
| 15 | Save, replay e determinismo | Arquivo de save, registro de ações, sementes |
| 16 | Testes e CI | Testes do modelo contra valores das fontes e teste 1x × 8x |
| 17 | Pipeline de arte | Plantas por estádio, ferramentas, máquinas |
| 18 | Protagonista e câmera | Primeira e terceira pessoa, controle, interação com o mundo |
| 19 | Novo pacote | Como adicionar cultura ou região |
| 20 | Modo semiaberto e eventos | Propriedade livre e eventos aleatórios |
| 21 | Roadmap | Sequência de entrega |

---

## 6. Fatia vertical (primeira entrega jogável)
Um canteiro plano de alface, ciclo completo. O protagonista faz análise de solo, calagem e adubação, plantio, irrigação e manejo, monitoramento e colheita. O relatório de safra explica as causas de cada perda. A fatia atravessa todas as camadas, do dado citado ao `.exe`, para expor cedo os erros de arquitetura. Ela só fecha se o balanço de água e de nitrogênio fechar exato e se 1x, 8x, dormir e save/load derem o mesmo hash de estado, no Linux e no Windows. O motivo da alface está no `BASE_MODELOS` §3.

---

## 7. Fases do projeto

Objetivo, entregáveis, critério de saída e passo a passo de cada fase estão no `MAPA_DO_PROJETO` §2 a §8.

## 8. Riscos

| Risco | Mecanismo | Mitigação |
|---|---|---|
| Modelo agronômico complexo demais | Cada processo acoplado multiplica parâmetros sem fonte | Conjunto mínimo por fase (`BASE_MODELOS` §3) |
| Parâmetro provisório vira definitivo | Sem calibração brasileira de alface, defensivos e descompactação, o número fica por inércia | Rótulo [PROVISÓRIO] no pacote e lista de lacunas em `BASE_MODELOS` §10 |
| Dado sem fonte entra no pacote | Preencher lacuna de memória parece inofensivo | Verificador de fontes no CI (§1.4) |
| Divergência entre velocidades do relógio | Passo variável muda o resultado e pode desestabilizar o integrador | Agenda única de eventos com passo fixo e teste 1x = 8x = dormir = save/load no CI |
| Resultado diferente no Windows | FMA e bibliotecas matemáticas mudam os últimos bits e o erro cresce com o tempo | Núcleo C++ com detmath e `-ffp-contract=off`; job Windows no CI compara o hash |
| Simulação em malha pesada | Sem *compute shaders* no Compatibility | Malha grossa na CPU, GDExtension, adiamento para a fase 3 |
| Vegetação derruba o FPS | Milhares de nós de planta | MultiMesh, talhão agregado, LOD (§3.2) |
| Licença das publicações | Tabela reproduzida em repositório público | Guardar só valores com citação e URL; PDF fora do Git |

---

## 9. Perguntas abertas

Nenhuma no momento. O usuário acatou as recomendações das rodadas de pesquisa. As decisões vigentes estão no `DECISOES.md`.
