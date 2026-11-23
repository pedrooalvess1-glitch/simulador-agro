# Núcleo C++ `agro_core`

Guarda e muda todo o estado do mundo calculado. Não depende do Godot. Na fase 1 entra no jogo por uma GDExtension fina. Arquitetura em [`docs/ARQUITETURA_SIMULACAO.md`](../../docs/ARQUITETURA_SIMULACAO.md).

| Arquivo | Conteúdo |
|---|---|
| `include/agro/detmath.hpp`, `src/detmath.cpp` | exp, log, pow, sin, cos, tan, atan, atan2, acos feitos só com operações exatas do IEEE 754 |
| `include/agro/fao56.hpp`, `src/fao56.cpp` | Astronomia, radiação e ET0 de Penman–Monteith do FAO-56 (diária e horária), com detmath |
| `include/agro/water.hpp`, `src/water.cpp` | Balanço hídrico diário de uma camada por célula: Curve Number, "aplicada hoje", Ks e Kr, árbitro e drenagem no mesmo dia, em mL no ledger |
| `include/agro/scheduler.hpp` | Agenda única de eventos com chave (tick, fase, seq) |
| `include/agro/clock.hpp` | Ritmo do relógio (24 para 1, multiplicador, teto por quadro, dormir) |
| `include/agro/ledger.hpp` | Contas inteiras de grandezas conservadas e conferência diária |
| `include/agro/allocate.hpp` | Divisão exata de uma oferta pelo método do maior resto |
| `include/agro/rng.hpp` | Sorteio por contador, só para eventos de jogo |
| `include/agro/hash.hpp` | Hash FNV-1a de estado, independente da plataforma |
| `tests/` | Testes unitários, teste de determinismo do tempo, exemplos numéricos do FAO-56 e do NEH-630 e fechamento do balanço hídrico sob ações aleatórias |
| `tools/detmath_vectors.cpp`, `golden/` | Vetores do detmath e o hash fixado |

Compilar e testar. `make test` (g++ -O2), `make CXX=clang++ test`, `make windows` (MinGW-w64), ou tudo com `tools/testar_nucleo.sh` na raiz. Código e comentários em inglês (`CONVENCOES` §3).
