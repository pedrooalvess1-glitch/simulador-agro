# Estado do projeto

> Só o estado atual; o histórico fica no Git. Próximos passos: `docs/ROTEIRO.md`. Regras: `CLAUDE.md`.

## 1. Fase atual

**Fase 0, Etapa B.** As duas rodadas de pesquisa de modelos estão registradas no `BASE_MODELOS`. Ele traz a matriz C1–C7, a arquitetura de acoplamento mínimo, os dados da alface e as lacunas abertas. Nada é produzido no Godot nesta fase.

**Próximo passo.** B0.3 (validação do usuário das decisões 27 a 34). Depois B1 (registro de fontes), Opus 5.5 com esforço `alto`.

## 2. O que existe

| Item | Estado |
|---|---|
| Repositório `pedrooalvess1-glitch/simulador-agro` | Público, MIT. Todo trabalho entra por *branch* + PR. |
| CI (`.github/workflows/ci.yml`) | Links internos dos `.md`; registro de fontes e citações; Godot fixado (SHA-512, versão e testes gdUnit4 da cena de fumaça). |
| Documentos | `00_INDICE`, `BASE_MODELOS`, `ROTEIRO`, `DECISOES`, `CONVENCOES`, `GLOSSARIO_DO_PROJETO`, `_MODELO_DOCUMENTO`, este arquivo. Os documentos 01–21 ainda não existem. |
| Registro de fontes | `docs/referencias/fontes.json` vazio; formato em `docs/referencias/FONTES.md`. As referências da primeira pesquisa entram no passo B1. |
| Projeto Godot | `engine/`, `packs/` e `art/` só têm a estrutura de pastas. |
| Regra da `main` | Ainda não configurada (passo A2). |

## 3. Ambiente
Mesmo ambiente do `simulador-normas`. O notebook Windows 10 com Radeon R5 integrada e 7,5 GB de RAM é o alvo de desempenho. As ferramentas são instaladas na nuvem a cada sessão por `tools/ambiente/`, com versão e soma conferidas. A pasta de testes no notebook será `C:\projetos\simulador-agro-testes`, criada com o primeiro `.exe`.
