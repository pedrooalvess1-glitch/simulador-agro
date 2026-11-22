# Estado do projeto

> Só o estado atual; o histórico fica no Git. Próximos passos: `docs/ROTEIRO.md`. Regras: `CLAUDE.md`.

## 1. Fase atual

**Etapa A (repositório).** Estrutura criada a partir da infraestrutura do `simulador-normas`. Faltam a regra da `main` e os rótulos (A2) e as respostas às perguntas abertas da `00_INDICE` §9 (A3).

**Próximo passo:** B1 (acervo de fontes Embrapa), Opus 5.5 com esforço `alto`, depois da resposta à pergunta 2 da `00_INDICE` §9.

## 2. O que existe

| Item | Estado |
|---|---|
| Repositório `pedrooalvess1-glitch/simulador-agro` | Público, MIT. Todo trabalho entra por *branch* + PR. |
| CI (`.github/workflows/ci.yml`) | Links internos dos `.md`; Godot fixado (SHA-512, versão e testes gdUnit4 da cena de fumaça); registro de fontes e citações. |
| Documentos | `00_INDICE`, `ROTEIRO`, `DECISOES`, `CONVENCOES`, `GLOSSARIO_DO_PROJETO`, `_MODELO_DOCUMENTO`, este arquivo. Os documentos 01–20 ainda não existem. |
| Registro de fontes | `docs/referencias/fontes.json` vazio; formato em `docs/referencias/FONTES.md`. |
| Projeto Godot | `engine/`, `packs/` e `art/` só têm a estrutura de pastas. |
| Regra da `main` | Ainda não configurada (passo A2). |

## 3. Ambiente
Mesmo ambiente do `simulador-normas`: notebook Windows 10 com Radeon R5 integrada e 7,5 GB de RAM como alvo de desempenho; ferramentas instaladas na nuvem a cada sessão por `tools/ambiente/`, com versão e soma conferidas. Pasta de testes no notebook: `C:\projetos\simulador-agro-testes` (a criar quando houver o primeiro `.exe`).
