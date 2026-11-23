# Estado do projeto

> Só o estado atual. O histórico fica no Git. Fases e próximos passos estão no `docs/MAPA_DO_PROJETO.md` e as regras no `CLAUDE.md`.

## 1. Fase atual

**Fase 0, Etapas B, V e C.** Quatro rodadas de pesquisa estão no `BASE_MODELOS`, com os valores da fase 1 conferidos na publicação original. A arquitetura de simulação está no `ARQUITETURA_SIMULACAO` e já tem núcleo C++ testado em `engine/native/`, com astronomia, ET0 e balanço hídrico de uma camada conferidos contra os exemplos do FAO-56 e do NRCS. Clima e solo passaram a ser a base do simulador, validada sem cultura na Etapa V. O fluxo de ingestão de cultura (documento 19) e a interface (documento 13) estão em rascunho. O registro de fontes tem 60 publicações registradas e 13 na fila de verificação. Nada é produzido no Godot nesta fase.

**Próximo passo.** B1 a B3 (registro de fontes). Depois V1 a V5 (cenários de referência, documento 22 e validação da ET0 e do balanço hídrico em solo nu).

## 2. O que existe

| Item | Estado |
|---|---|
| Repositório `pedrooalvess1-glitch/simulador-agro` | Público, MIT. Todo trabalho entra por *branch* + PR. |
| CI (`.github/workflows/ci.yml`) | Links internos; registro de fontes e citações; núcleo C++ no Linux (g++ -O0/-O2, clang++, g++ -march=haswell, erro em ulp) e no Windows (MinGW-w64); Godot fixado. |
| Núcleo C++ (`engine/native/`) | detmath, agenda de eventos, relógio, ledger, alocação por maior resto, RNG de eventos, hash de estado, astronomia e ET0 do FAO-56 (diária e horária) e balanço hídrico de uma camada. 34 testes, incluindo 1x = 8x = dormir = pular até o evento = save/load os exemplos 8 a 13, 17 a 19 e 37 do FAO-56, os exemplos 10-1 e 10-2 do NEH-630 e o fechamento do ledger sob ações aleatórias. Hash dos vetores do detmath `41cbfb563eb7b91d` igual em todas as configurações Linux e no Windows (MinGW g++ 14.2.0, job do CI). |
| Documentos | `00_INDICE`, `MAPA_DO_PROJETO`, `ARQUITETURA_SIMULACAO`, `BASE_MODELOS`, `DECISOES`, `CONVENCOES`, `GLOSSARIO_DO_PROJETO`, `_MODELO_DOCUMENTO`, `13_INTERFACE` (rascunho, com wireframes em `docs/interface/`), `19_INGESTAO_DE_CULTURA` (rascunho), este arquivo. Os demais documentos 01 a 21 ainda não existem. |
| Registro de fontes | 60 fontes registradas e 13 publicações na fila de verificação (`docs/referencias/fontes.json`). |
| Projeto Godot | Ainda não existe. `packs/` e `art/` só têm a estrutura de pastas. |
| Regra da `main` | *Ruleset* "Proteção da main" ativo. PR, os cinco jobs do CI, histórico linear e só *squash*; sem *push* forçado nem exclusão. *Branch* apagada no *merge*. |

## 3. Ambiente
Mesmo ambiente do `simulador-normas`. O notebook Windows 10 com Radeon R5 integrada e 7,5 GB de RAM é o alvo de desempenho. As ferramentas são instaladas na nuvem a cada sessão por `tools/ambiente/`, com versão e soma conferidas. O núcleo C++ compila com g++, clang++ e MinGW-w64 na nuvem. O Wine não roda na sessão da nuvem, então o teste no Windows é o job do CI. A pasta de testes no notebook será `C:\projetos\simulador-agro-testes`, criada com o primeiro `.exe`.
