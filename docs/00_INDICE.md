# Simulador Agro: índice mestre e decisões de infraestrutura

> Documento de **entrada** do projeto. Ele é lido antes de qualquer outro.
> Objetivo: um simulador 3D de propriedade rural em que o jogador vive como protagonista e aplica o conhecimento técnico publicado pela Embrapa no manejo de solo, água, nutrição, fitossanidade, calendário e colheita.
> Infraestrutura herdada do repositório `simulador-normas`: Godot fixado, CI, verificador de links, pipeline Blender, convenções e modelo de operação. O conteúdo é novo.
>
> Rótulos: `[FONTE:ID]` · [PRÁTICA] · [INFERÊNCIA] · [MODELO] · [VERIFICAR] · [VERIFICAR NA FONTE] (definições no `CLAUDE.md` §3, regra 7).
>
> **Fase atual:** Fase 0 (pesquisa de modelos e acervo de fontes). Nada é produzido no Godot nesta fase. Escolha de modelos e arquitetura de tempo em `BASE_MODELOS.md`.

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
| R9 | **Tempo simulado fiel** | Relógio de 24 para 1 com 2x, 4x e 8x, dormir, salvar e carregar, resultado idêntico em qualquer velocidade (`BASE_MODELOS` §2). |
| R10 | **Extensível** | Separação rígida motor × pacote (§4). Cultura nova é dado novo, sem código novo. |
| R11 | **Interferência mínima do usuário** | O agente opera ferramentas, repositório e notebook. O usuário só faz U1–U4 (`CLAUDE.md` §2). |

### 1.1 Modelo de operação
Igual ao do `simulador-normas`. Todo o projeto é conduzido no Claude Code, com permissão total, trabalho contínuo e pontos de parada P1–P4 (`ROTEIRO` §1).

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

---

## 2. Princípios do projeto

1. **Conhecimento é dado, não código.** Nenhum número agronômico dentro de cena ou script do motor.
2. **O mundo é calculado.** Solo, água, planta, pragas e doenças evoluem por modelos com passo fixo em tempo simulado.
3. **Um único ponto de mudança do mundo.** Toda operação passa por `act(verbo, alvo, insumo)`. Interface, testes e replay usam o mesmo caminho.
4. **Camadas.** A camada de baixo nunca chama a de cima. A comunicação de volta é por eventos (*signals*).
5. **Acoplamento mínimo no tempo.** Cada subsistema roda na própria taxa e lê o estado dos outros só na fronteira da camada mais lenta (`BASE_MODELOS` §2.3).
6. **Tudo reproduzível.** Uma safra é definida por `(versão do pacote, versão do motor, semente do mundo, série climática, registro de ações)`.
7. **Verdade invisível.** O estado real do solo e da planta só aparece por medição. Análise de solo com prazo de laudo, tensiômetro, amostragem de pragas e observação de sintomas. O raio-x aparece no relatório de safra e no modo aprendiz.
8. **Rastreabilidade.** Todo número leva fonte e local. Toda simplificação do simulador leva [MODELO].
9. **Verificável por máquina.** Esquema dos dados e citações conferidos no CI desde o primeiro dia.
10. **Leve por padrão.** Recurso visual que não ensina nada não entra.

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

### 3.2 GPU integrada
**Mecanismo.** GPU integrada divide a RAM do sistema e tem pouca banda de memória. O que derruba o desempenho é, em ordem, textura grande, sombra em tempo real, pós-processamento e número de *draw calls*. Medição herdada na Radeon R5: ~496 FPS sem sombra e ~75 FPS com sombra numa cena trivial; ~250–280 FPS no `.exe` da cena de fumaça.

Restrições específicas deste projeto:
1. **Vegetação em massa.** Plantas repetidas com `MultiMeshInstance3D`. Lavoura grande vira representação por talhão. Nível de detalhe por distância.
2. **Sem *compute shaders*.** O renderizador Compatibility não oferece *compute shaders* e não há plano de suporte [VERIFICAR]. Simulação em malha (água no terreno, deformação) roda na CPU em malha grossa ou em GDExtension C++.
3. **Física fora do multiplicador.** Protagonista e máquinas rodam em tempo real (`BASE_MODELOS` §2.2).

### 3.3 Orçamento de desempenho

| Métrica | Limite (perfil Leve) |
|---|---|
| FPS na GPU integrada de referência | ≥ 30 estável (meta 60) |
| Triângulos visíveis | ≤ 150 mil |
| *Draw calls* | ≤ 200 |
| Memória de texturas (só fontes e interface) | ≤ 64 MB |
| Passos de simulação agronômica por quadro na velocidade 8x | dentro de 4 ms de CPU [INFERÊNCIA] |
| Tamanho do instalador | ≤ 200 MB |

---

## 4. Arquitetura em duas partes

As subpastas de `engine/` e `packs/` são criadas com o projeto Godot (Fase 1).

```
engine/                  motor genérico: não conhece nenhuma cultura
  base/     relógio de passo fixo, eventos, save, RNG por subsistema, log
  world/    propriedade, terreno deformável, câmera, protagonista, máquinas
  model/    camadas L1–L3: clima → água → química → planta → daninhas → pragas
  domain/   act(), catálogo, operações, laudos
  game/     campanha, modo semiaberto, eventos aleatórios, avaliação
  ui/       HUD, caderno de campo, laudos, relatório de safra
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

Formato: `_MODELO_DOCUMENTO.md`. Ordem e esforço: `ROTEIRO` §3.

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
Um canteiro plano de alface, ciclo completo. O protagonista faz análise de solo, calagem e adubação, plantio, irrigação e manejo, monitoramento e colheita. O relatório de safra explica as causas de cada perda. A fatia atravessa todas as camadas, do dado citado ao `.exe`, para expor cedo os erros de arquitetura. O motivo da alface está no `BASE_MODELOS` §3.

---

## 7. Fases do projeto

| Fase | Entrega | Conteúdo |
|---|---|---|
| 0 | Base de modelos e acervo de fontes | Pesquisa de modelos, arquitetura de tempo, registro de fontes Embrapa, documentos de planejamento. Nada no Godot. |
| 1 | Fatia vertical | Protagonista em primeira e terceira pessoa, relógio 24 para 1 com 2x/4x/8x, dormir, salvar e carregar, um canteiro plano de alface com os modelos da fase 1 (`BASE_MODELOS` §3) e relatório de safra. |
| 2 | Horta completa e campanha em área plana | Gerador climático, palhada, ciclo do nitrogênio, fósforo e potássio, compostagem, aspersão e molhamento foliar, defensivos do Agrofit, microtrator com preparo do solo e deformação do terreno, tomate cereja, campanha básica. |
| 3 | Área inclinada e solo vivo | Escoamento e erosão no terreno, banco de sementes, consórcio, matéria orgânica multianual, descompactação biológica, compactação por tráfego, adubação verde, ZARC na escolha de época, modo semiaberto com eventos aleatórios. |
| 4 | Mundo semiaberto | Economia, combustível, NPCs, cidades, gases e deriva, epidemiologia espacial, hidráulica de gotejo, micronutrientes. |
| 5 | Animais | Galinha, tilápia, porcos e gado, ligados ao ciclo de esterco e composto. |

---

## 8. Riscos

| Risco | Mecanismo | Mitigação |
|---|---|---|
| Modelo agronômico complexo demais | Cada processo acoplado multiplica parâmetros sem fonte | Conjunto mínimo por fase (`BASE_MODELOS` §3 e §4) |
| Dado sem fonte entra no pacote | Preencher lacuna de memória parece inofensivo | Verificador de fontes no CI (§1.4) |
| Divergência entre velocidades do relógio | Passo variável muda o resultado e pode desestabilizar o integrador | Passo fixo com acumulador e teste 1x × 8x no CI |
| Simulação em malha pesada | Sem *compute shaders* no Compatibility | Malha grossa na CPU, GDExtension, adiamento para a fase 3 |
| Vegetação derruba o FPS | Milhares de nós de planta | MultiMesh, talhão agregado, LOD (§3.2) |
| Licença das publicações | Tabela reproduzida em repositório público | Guardar só valores com citação e URL; PDF fora do Git |

---

## 9. Perguntas abertas

| # | Pergunta | Recomendação |
|---|---|---|
| 1 | Duração de um ciclo de alface em tempo real. Na razão 24 para 1, um ciclo de cerca de 45 dias [VERIFICAR NA FONTE] leva cerca de 45 horas em 1x e cerca de 6 horas em 8x, sem contar o dormir. Isso serve para a campanha? | Manter 24 para 1. O dormir pula as noites e a campanha usa etapas curtas do ciclo |
| 2 | Determinismo entre plataformas | Aceitar "mesma build, mesma plataforma" na fase 1 e revisar depois da segunda pesquisa |
