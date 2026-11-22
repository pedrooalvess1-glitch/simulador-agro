# Simulador Agro: índice mestre e decisões de infraestrutura

> Documento de **entrada** do projeto. Ele é lido antes de qualquer outro.
> Objetivo: um simulador 3D de microgestão de propriedade rural em que o jogador aplica o conhecimento técnico publicado pela Embrapa (manejo de solo, água, nutrição, fitossanidade, calendário e colheita) sem precisar de uma propriedade real para testar.
> Infraestrutura herdada do repositório `simulador-normas`: Godot fixado, CI, verificador de links, pipeline Blender, convenções e modelo de operação. O conteúdo é novo.
>
> Rótulos: `[FONTE:ID]` · [PRÁTICA] · [INFERÊNCIA] · [MODELO] · [VERIFICAR] · [VERIFICAR NA FONTE] (definições no `CLAUDE.md` §3, regra 7).
>
> **Fase atual:** planejamento. O projeto Godot do simulador só é criado depois da Etapa C do `ROTEIRO`.

---

## 1. Requisitos de base

| # | Requisito | Consequência no projeto |
|---|---|---|
| R1 | **Todas as ferramentas gratuitas** | Godot (MIT), Blender (GPL), Git/GitHub (plano gratuito), gdUnit4 (MIT). |
| R2 | **Simulador de treinamento**, não produto visual polido | Low-poly com cores chapadas. O esforço vai para o modelo agronômico, a interação e o *feedback*. |
| R3 | **Rodar bem em GPU integrada com 2 GB** | Renderizador Compatibility, sem texturas, sem sombras em tempo real, orçamento fixo (§3.3). |
| R4 | **O conhecimento técnico é o centro** | Conhecimento como dado, rastreável até a publicação (§1.4, princípios 1 e 7). |
| R5 | **Microgestão** | A unidade de manejo é o canteiro (horta) ou o talhão (lavoura), com estado próprio de solo, água, cultura e sanidade. O jogador decide cada operação. |
| R6 | **Aparência e mecânicas de *farming simulator*** | Propriedade em 3D navegável, ciclo de dias e estações, ferramentas e máquinas, insumos, colheita e venda (§2.1). |
| R7 | **Extensível a culturas, solos e regiões** | Separação rígida motor × pacote (§4). Cultura nova = dado novo, sem código novo. |
| R8 | **Interferência mínima do usuário** | O agente opera ferramentas, repositório e notebook; o usuário só faz U1–U4 (`CLAUDE.md` §2). |

### 1.1 Modelo de operação
Igual ao do `simulador-normas`: todo o projeto é conduzido no Claude Code, com permissão total, trabalho contínuo e pontos de parada P1–P4 (`ROTEIRO` §1).

### 1.2 Público e uso
O usuário é o primeiro público: um engenheiro com horta própria que quer testar técnicas de manejo antes de aplicá-las e entender o mecanismo de cada recomendação. Um segundo público possível são estudantes de ciências agrárias e pequenos produtores [INFERÊNCIA].

### 1.3 O que o simulador ensina
1. **Por que** cada prática funciona: o jogador vê a causa (pH baixo → menor disponibilidade de P; solo compactado → menor infiltração e raiz rasa) no "raio-x" do relatório de safra.
2. **Quando** fazer: calendário por região, janela de plantio, estádio da cultura, nível de dano de praga.
3. **Quanto** usar: dose de calcário e adubo a partir de análise de solo, lâmina de irrigação a partir da evapotranspiração.
4. **Consequências** de longo prazo: matéria orgânica, erosão, banco de sementes de plantas daninhas, resistência de pragas, rotação e consórcio.

### 1.4 Política de fontes
1. **Fonte primária:** publicações da Embrapa (Sistemas de Produção, Circulares e Comunicados Técnicos, Documentos, livros das unidades, ZARC). Bases de busca: Alice e Infoteca-e [VERIFICAR: endereços, licença de cada publicação e condições de reprodução de tabelas].
2. Todo parâmetro do pacote cita o ID do registro `docs/referencias/fontes.json` e o local (página, tabela, seção). O verificador `tools/conferir_fontes.py` barra no CI citação a ID inexistente e dado sem fonte.
3. Quando duas publicações da Embrapa divergirem, o pacote guarda as duas, com região e ano, e o motor usa a da região do cenário [INFERÊNCIA].
4. **Lacunas:** se a Embrapa não publica um dado necessário ao modelo (ex.: série climática diária), a decisão sobre usar outra fonte é do usuário (pergunta aberta 1, §9).
5. PDFs de terceiros ficam fora do Git (`docs/referencias/pdf/`, ignorada). O registro guarda a referência, a URL e a data de acesso.

---

## 2. Princípios do projeto

1. **Conhecimento é dado, não código.** Nenhum número agronômico dentro de cena ou script do motor.
2. **O mundo é calculado:** solo, água, planta, pragas e doenças evoluem por um modelo diário a partir do clima e das ações do jogador (documento 05).
3. **Um único ponto de mudança do mundo:** `act(verbo, alvo, insumo)`. Interface, testes e replay usam o mesmo caminho.
4. **Camadas:** a de baixo nunca chama a de cima. A comunicação de volta é por eventos (*signals*).
5. **Tudo reproduzível:** uma safra é definida por `(versão do pacote, versão do motor, semente, série climática)`.
6. **Verdade invisível:** o estado real do solo e da planta só aparece por medição. Análise de solo (com prazo de laudo), tensiômetro, amostragem de pragas, análise foliar e observação de sintomas. O raio-x aparece no relatório de safra e no modo aprendiz.
7. **Rastreabilidade:** todo número leva fonte e local. Toda simplificação do simulador leva [MODELO].
8. **Verificável por máquina:** esquema dos dados e citações conferidos no CI desde o primeiro dia.
9. **Leve por padrão:** se um recurso visual não ensina nada, ele não entra.

### 2.1 O que vem do *farming simulator* e o que muda

| Mecânica | No *farming simulator* | Neste simulador |
|---|---|---|
| Propriedade 3D | Navegação livre, máquinas grandes | Navegação livre; escala de horta e pequena propriedade primeiro, lavoura depois |
| Tempo | Dias e estações acelerados | Dia simulado com pausa e avanço rápido; o modelo roda um passo por dia |
| Operações | Arar, plantar, pulverizar, colher | As mesmas e mais: análise de solo, calagem, adubação de plantio e cobertura, irrigação por lâmina, capina, cobertura morta, adubação verde, monitoramento de pragas, tutoramento, desbrota |
| Crescimento | Estádios fixos por tempo | Fenologia calculada (graus-dia ou dias, conforme a fonte), afetada por água, nutriente, temperatura e sanidade |
| Economia | Central no jogo | Custo e receita como métrica de avaliação, não como objetivo único |
| Resultado | Dinheiro | Produtividade, qualidade, custo, saúde do solo e explicação do porquê |

---

## 3. Plataforma

### 3.1 Stack
Herdada e já testada: Godot 4.7.2 (GDScript, Compatibility), Blender 4.5.10 LTS → glTF, gdUnit4 v6.2.3, GitHub Actions. Versões e somas em `tools/ambiente/versoes.env`; comandos no `CLAUDE.md` §5. O mecanismo da escolha do Godot (licença, cenas em texto legíveis pelo agente, Compatibility leve em GPU integrada) foi registrado no `simulador-normas` e vale aqui sem mudança.

### 3.2 GPU integrada
**Mecanismo:** GPU integrada divide a RAM do sistema e tem pouca banda de memória. O que derruba o desempenho é, em ordem, textura grande, sombra em tempo real, pós-processamento e número de *draw calls*. Medição herdada na Radeon R5: ~496 FPS sem sombra e ~75 FPS com sombra numa cena trivial; ~250–280 FPS no `.exe` da cena de fumaça.

Consequência específica deste projeto: **vegetação em massa** é o risco principal. Uma horta tem centenas de plantas e uma lavoura tem milhares. Regras:
1. Plantas repetidas com `MultiMeshInstance3D` (um *draw call* por espécie e estádio).
2. Lavoura acima de um limiar de área vira representação por talhão (faixas de cor e altura), não planta individual [INFERÊNCIA; limiar no documento 03].
3. Nível de detalhe por distância; a microgestão planta a planta só no canteiro selecionado.

### 3.3 Orçamento de desempenho

| Métrica | Limite (perfil Leve) |
|---|---|
| FPS na GPU integrada de referência | ≥ 30 estável (meta 60) |
| Triângulos visíveis | ≤ 150 mil |
| *Draw calls* | ≤ 200 |
| Memória de texturas (só fontes e interface) | ≤ 64 MB |
| Passo diário do modelo para a propriedade inteira | ≤ 50 ms [INFERÊNCIA; avanço rápido de 30 dias em menos de 2 s] |
| Tamanho do instalador | ≤ 200 MB |

---

## 4. Arquitetura em duas partes

As subpastas de `engine/` e `packs/` são criadas com o projeto Godot (Etapa D).

```
engine/                  motor genérico: não conhece nenhuma cultura
  base/     eventos, save, RNG com semente, log
  world/    propriedade, câmera, seleção, máquinas
  model/    passo diário: clima → solo → água → nutrientes → planta → pragas e doenças
  domain/   act(), catálogo, operações, laudos
  game/     safra, desafios, (depois) modo livre, avaliação
  ui/       HUD, caderno de campo, laudos, relatório de safra
packs/
  <pacote>/
    crops/    culturas: fenologia, Kc, demanda de nutrientes, espaçamento, ciclo, janelas
    soils/    perfis de solo: textura, MO, pH, CTC, capacidade de campo, densidade
    climate/  regiões e séries climáticas
    pests/    pragas, doenças e plantas daninhas: condição favorável, dano, nível de controle
    inputs/   insumos: corretivos, fertilizantes, defensivos registrados, sementes
    recs/     tabelas de recomendação (calagem, adubação, irrigação)
    scenarios/ propriedades e desafios
    tests/
```

1. O motor conhece **variáveis de estado, processos e operações**. Valores e tabelas vêm do pacote.
2. Um pacote pode cobrir uma região ou um grupo de culturas. Pacotes se combinam (ex.: `hortalicas` + `solos-nordeste`) [INFERÊNCIA; detalhe no documento 04].

---

## 5. Mapa dos documentos de planejamento

Formato: `_MODELO_DOCUMENTO.md`. Ordem e esforço: `ROTEIRO` §3.

| Nº | Documento | Pergunta que responde |
|---|---|---|
| 01 | Visão e pedagogia | O que é "aprender manejo jogando"; laço de jogo; modo aprendiz |
| 02 | Arquitetura geral | Camadas, contratos, eventos, `act()` |
| 03 | Plataforma e desempenho | Perfis, vegetação em massa, orçamento |
| 04 | Modelo de conhecimento | Esquema dos dados agronômicos e das citações |
| 05 | Modelo solo-planta-atmosfera | Variáveis de estado, processos diários, simplificações [MODELO] |
| 06 | Mundo 3D | Propriedade, terreno, canteiros, kit de peças |
| 07 | Catálogo | Ferramentas, implementos, máquinas, insumos |
| 08 | Interação e operações | Verbos, pré-condições, efeitos |
| 09 | Tempo, clima e calendário | Passo diário, séries climáticas, estações, janelas |
| 10 | Fitossanidade | Pragas, doenças, plantas daninhas, monitoramento, controle |
| 11 | Desafios e campanha | Cenários com objetivo, progressão |
| 12 | Avaliação | Produtividade, qualidade, custo, saúde do solo, explicação |
| 13 | Interface | HUD, caderno de campo, laudos, relatório de safra |
| 14 | Pacote inicial | Culturas, solo e região da fatia vertical |
| 15 | Dados e replay | Save, log, reprodução de safra |
| 16 | Testes e CI | Testes do modelo contra valores das fontes |
| 17 | Pipeline de arte | Plantas por estádio, ferramentas, máquinas |
| 18 | Novo pacote | Como adicionar cultura ou região |
| 19 | Modo livre | Propriedade aberta, sem objetivo fixo |
| 20 | Roadmap | Sequência de entrega |

---

## 6. Fatia vertical (primeira entrega jogável)
Um canteiro, uma cultura, um ciclo completo: análise de solo → calagem e adubação → plantio → irrigação e manejo → monitoramento → colheita → relatório de safra com raio-x. Ela atravessa todas as camadas, do dado citado ao `.exe`, para expor cedo os erros de arquitetura. Cultura e região: pergunta aberta 2 (§9).

---

## 7. Roadmap resumido
1. **Fase 0:** acervo de fontes Embrapa e documentos de planejamento.
2. **Fase 1:** fatia vertical (§6).
3. **Fase 2:** horta completa (várias culturas, rotação, consórcio, adubação verde, pragas).
4. **Fase 3:** lavouras e máquinas (talhões, implementos, plantio direto).
5. **Fase 4:** modo livre e novos pacotes regionais.

---

## 8. Riscos

| Risco | Mecanismo | Mitigação |
|---|---|---|
| Modelo agronômico complexo demais | Cada processo acoplado multiplica parâmetros sem fonte | Começar com poucos processos, cada um com fonte; [MODELO] explícito em toda simplificação |
| Dado sem fonte entra no pacote | Preencher lacuna de memória parece inofensivo | Verificador de fontes no CI (§1.4) |
| Vegetação derruba o FPS | Milhares de nós de planta | MultiMesh, talhão agregado, LOD (§3.2) |
| Licença das publicações | Tabela reproduzida em repositório público | Guardar só valores com citação e URL; PDF fora do Git; conferir licença ([VERIFICAR], §1.4) |

---

## 9. Perguntas abertas

| # | Pergunta | Recomendação |
|---|---|---|
| 1 | Para lacunas que a Embrapa não cobre (ex.: série climática diária), aceitar fontes públicas oficiais (INMET, MAPA/Agrofit para registro de defensivos)? | Sim, só órgãos públicos, com o mesmo registro e rótulo. Mantém a rastreabilidade sem travar o modelo climático |
| 2 | Cultura e região da fatia vertical | Uma hortaliça de ciclo curto e bem documentada (ex.: alface ou tomate cereja) num solo e clima do litoral de Pernambuco. Ciclo curto testa o laço inteiro rápido |
| 3 | Defensivos químicos entram no simulador? | Só como classe de controle com o princípio de manejo integrado, sem marca comercial nem dose de produto, numa primeira fase. Evita reproduzir recomendação de bula fora de contexto |
