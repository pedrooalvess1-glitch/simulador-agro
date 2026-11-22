# Convenções do projeto

> Nomes, idioma, commits e versões. Vale para `engine/`, `packs/`, `art/`, `tools/` e `docs/`. Termos: `GLOSSARIO_DO_PROJETO.md`.

## 1. Arquivos e pastas

| O quê | Formato | Exemplo |
|---|---|---|
| Pastas | `snake_case`, sem acento | `engine/model/`, `packs/hortalicas/crops/` |
| Documento de planejamento | `NN_NOME_EM_MAIUSCULAS.md` (número da `00_INDICE` §5) | `05_MODELO_SOLO_PLANTA_ATMOSFERA.md` |
| Documento de apoio | `NOME_EM_MAIUSCULAS.md`; modelos com `_` no início | `CONVENCOES.md`, `_MODELO_DOCUMENTO.md` |
| Script GDScript | `snake_case.gd`, um `class_name` em `PascalCase` por script | `water_balance.gd` → `class_name WaterBalance` |
| Núcleo C++ | `snake_case.hpp` / `.cpp` em `engine/native/`; tudo no `namespace agro`; tipos em `PascalCase`, funções em `snake_case`, constantes `kNome` | `include/agro/ledger.hpp` → `agro::Ledger` |
| Cena e recurso Godot | `snake_case.tscn` / `.tres` | `canteiro.tscn` |
| Dado do pacote | `snake_case.json` | `packs/hortalicas/crops/lettuce.json` |
| Teste | `test_<alvo>.gd`, numa pasta `tests/` que espelha a do alvo | `tests/model/test_water_balance.gd` |
| Modelo 3D | `<objeto>.py`, `<objeto>.glb`, `<objeto>.png` com o mesmo nome; planta por estádio com sufixo | `art/blender/lettuce_head.py` |
| Ferramenta | `snake_case.py` ou `.sh`, nome em português (verbo + objeto) | `tools/conferir_fontes.py` |
| ID de fonte | `EMB-<UNIDADE>-<ANO>-<SIGLA>` para Embrapa; `<ORGAO>-<ANO>-<SIGLA>` para outros órgãos | `EMB-CNPH-2020-ALFACE` |

Sem espaço, acento ou maiúscula em nome de arquivo de código, dado ou arte: o Godot importa caminhos sensíveis a maiúsculas na exportação, e o Windows não.

## 2. Nós e cenas no Godot

1. Nome de nó em `PascalCase`, descrevendo o papel: `Canteiro`, `Regador`, `HUD`.
2. Nó que representa um item do catálogo leva o ID nos metadados (`set_meta("item_id", ...)`), não no nome.
3. Grupos em `snake_case` no plural: `beds`, `water_sources`.
4. Sinais em `snake_case`, no passado, nomeando o fato: `day_advanced`, `operation_done`, `report_ready`.

## 3. Idioma

| Onde | Idioma | Motivo |
|---|---|---|
| Código do motor e do pacote (identificadores, comentários, log), incluindo o núcleo C++ | Inglês | Padrão da API do Godot; evita mistura dentro da mesma linha |
| Mensagens dos testes do núcleo e das ferramentas | Português | Lidas pelo usuário no CI |
| Ferramentas de `tools/` | Português | Lidas pelo usuário; não entram no executável |
| IDs de dado (`crop_id`, `soil_id`, verbos) | Inglês, `snake_case`, só ASCII | São chaves comparadas pelo código; o texto exibido vem de outro campo |
| Texto ao jogador | Português do Brasil | Público e fontes em português |
| Documentos, issues, PRs, commits | Português do Brasil | Leitura do usuário |

Todo texto ao jogador fica em arquivo de dado do pacote ou de tradução, nunca em string dentro de script do motor.

## 4. Dado com fonte
Todo valor numérico do pacote é um objeto com valor, unidade, fonte e local:
```json
{ "value": 25, "unit": "cm", "fonte": "EMB-CNPH-2020-ALFACE", "local": "Tabela 2, p. 7" }
```
1. `fonte` é um ID existente em `docs/referencias/fontes.json`; `local` não pode ser vazio. O `tools/conferir_fontes.py` barra os dois casos no CI.
2. Valor derivado pelo simulador (interpolação, conversão de unidade, média de faixa) leva `"modelo": "<explicação curta>"` além da fonte de origem.
3. Parâmetro estrangeiro ou sem calibração brasileira leva `"provisorio": true` e continua com `fonte` e `local`.
4. Valor de cenário sem fonte (nome de propriedade, posição de canteiro) não tem `fonte`; ele não é conhecimento agronômico.

## 5. Mensagens de commit e títulos de PR

Formato `tipo(escopo): resumo no imperativo`, em português, até 72 caracteres.

| Tipo | Uso |
|---|---|
| `feat` | Funcionalidade nova no motor ou no pacote |
| `fix` | Correção de defeito |
| `docs` | Só documentação |
| `content` | Conteúdo agronômico no pacote ou no registro de fontes |
| `art` | Modelo 3D, script Blender, prévia |
| `perf` | Desempenho |
| `test` | Só testes |
| `ci` | Workflows, modelos de issue e de PR |
| `chore` | Manutenção sem efeito no comportamento |

Escopos usuais: `engine`, `pack`, `fontes`, `docs`, `art`, `tools`, `ambiente`, `ci`. PR mesclado por *squash*: o título do PR vira o commit na `main`.

## 6. Versões

Versionamento semântico, separado para o motor (`engine-vX.Y.Z`), para cada pacote (`<pacote>-vX.Y.Z`) e para a Release do aplicativo (`vX.Y.Z`).
1. Antes da 1.0, MENOR pode quebrar contrato.
2. O pacote declara a faixa de versões do motor com que funciona; o motor recusa pacote fora da faixa.
3. Toda safra gravada leva `(versão do pacote, versão do motor, semente, série climática)`.
4. Versões de ferramentas ficam fixadas num só lugar, `tools/ambiente/versoes.env`.
