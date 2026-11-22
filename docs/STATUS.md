# Estado do projeto

> Só o estado atual. O histórico fica no Git. Fases e próximos passos estão no `docs/MAPA_DO_PROJETO.md` e as regras no `CLAUDE.md`.

## 1. Fase atual

**Fase 0, Etapas B e C.** Três rodadas de pesquisa estão no `BASE_MODELOS`. A arquitetura de simulação foi refeita no `ARQUITETURA_SIMULACAO` e já tem núcleo C++ testado em `engine/native/`. O registro de fontes tem 16 publicações registradas e 31 na fila de verificação. O documento 13 (interface) está em rascunho. Nada é produzido no Godot nesta fase.

**Próximo passo.** B0.5 (quarta rodada de pesquisa). O pedido está pronto em `docs/referencias/PEDIDO_PESQUISA_RODADA4.md` e não rodou por falta de cota de pesquisa. Depois B1 (esvaziar a fila do registro), Opus 5.5 com esforço `alto`, e B5 (astronomia e ET0 no núcleo), esforço `extra`.

## 2. O que existe

| Item | Estado |
|---|---|
| Repositório `pedrooalvess1-glitch/simulador-agro` | Público, MIT. Todo trabalho entra por *branch* + PR. |
| CI (`.github/workflows/ci.yml`) | Links internos; registro de fontes e citações; núcleo C++ no Linux (g++ -O0/-O2, clang++, g++ -march=haswell, erro em ulp) e no Windows (MinGW-w64); Godot fixado. |
| Núcleo C++ (`engine/native/`) | detmath, agenda de eventos, relógio, ledger, alocação por maior resto, RNG de eventos e hash de estado. 15 testes, incluindo 1x = 8x = dormir = save/load. Hash dos vetores do detmath `41cbfb563eb7b91d` igual em todas as configurações Linux e no Windows (MinGW g++ 14.2.0, job do CI). |
| Documentos | `00_INDICE`, `MAPA_DO_PROJETO`, `ARQUITETURA_SIMULACAO`, `BASE_MODELOS`, `DECISOES`, `CONVENCOES`, `GLOSSARIO_DO_PROJETO`, `_MODELO_DOCUMENTO`, `13_INTERFACE` (rascunho, com wireframes em `docs/interface/`), este arquivo. Os demais documentos 01 a 21 ainda não existem. |
| Registro de fontes | 16 fontes registradas e 31 publicações na fila de verificação (`docs/referencias/fontes.json`). |
| Projeto Godot | Ainda não existe. `packs/` e `art/` só têm a estrutura de pastas. |
| Regra da `main` | Ainda não configurada (passo A2). |

## 3. Ambiente
Mesmo ambiente do `simulador-normas`. O notebook Windows 10 com Radeon R5 integrada e 7,5 GB de RAM é o alvo de desempenho. As ferramentas são instaladas na nuvem a cada sessão por `tools/ambiente/`, com versão e soma conferidas. O núcleo C++ compila com g++, clang++ e MinGW-w64 na nuvem. O Wine não roda na sessão da nuvem, então o teste no Windows é o job do CI. A pasta de testes no notebook será `C:\projetos\simulador-agro-testes`, criada com o primeiro `.exe`.
