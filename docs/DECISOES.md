# Decisões vigentes

> Índice das decisões em vigor. O detalhe de cada uma fica num só lugar, indicado na última coluna; quando uma decisão deixa de valer, ela sai da tabela (o registro fica no Git). Os números são identificadores fixos.
> **Origem:** *usuário* = decisão dele; *agente* = decidida pelo agente sem perguntar, por não ser essencial ([INFERÊNCIA]); o usuário pode revertê-la a qualquer momento. *Herdada* = decisão do `simulador-normas` que vale aqui pelo mesmo mecanismo.

| # | Data | Decisão | Motivo | Origem | Detalhe em |
|---|---|---|---|---|---|
| 1 | 22/11/2026 | Repositório novo derivado do `simulador-normas`, que fica intacto | O `simulador-normas` guarda um projeto ativo; copiar a infraestrutura reaproveita o que já foi testado sem misturar os dois | usuário | `README.md` |
| 2 | 22/11/2026 | Nome `simulador-agro`; público, licença MIT | Nome curto que cobre horta, lavoura e fruticultura; repositório público tem Actions sem cota | usuário | `CLAUDE.md` §3 |
| 3 | 22/11/2026 | Godot 4.7.2 (GDScript), renderizador Compatibility, Blender 4.5.10 LTS, gdUnit4 v6.2.3 | Gratuitos, cenas em texto legíveis pelo agente, leve em GPU integrada; versões já testadas na nuvem e no notebook | herdada | `tools/ambiente/versoes.env` |
| 4 | 22/11/2026 | Sem texturas e sem sombras em tempo real | Medição na Radeon R5: ~496 FPS sem sombra × ~75 FPS com sombra; não ensinam nada | herdada | `00_INDICE` §3.2 |
| 5 | 22/11/2026 | Agente com permissão total; usuário só em U1–U4; trabalho contínuo com pontos de parada P1–P4 | O usuário não quer curva de aprendizado em ferramenta nenhuma nem aprovar passo a passo | herdada | `CLAUDE.md` §2, `ROTEIRO` §1 |
| 6 | 22/11/2026 | Conhecimento agronômico é dado; todo número com fonte registrada e local | Uma só fonte verificável por parâmetro; o CI barra número sem fonte | agente | `00_INDICE` §1.4 |
| 7 | 22/11/2026 | Embrapa como fonte primária | Pedido do usuário: aplicar o conhecimento publicado pela Embrapa | usuário | `00_INDICE` §1.4 |
| 8 | 22/11/2026 | PDFs de terceiros fora do Git; registro guarda referência, URL e data de acesso | Repositório público; a licença de cada publicação ainda não foi conferida | agente | `docs/referencias/FONTES.md` |
| 9 | 22/11/2026 | Modelo diário solo-planta-atmosfera como núcleo do mundo calculado | Microgestão exige que cada operação mude o estado e que a consequência apareça dias depois | agente | `00_INDICE` §2 |
| 10 | 22/11/2026 | Fatia vertical = um canteiro, uma cultura, ciclo completo | Atravessa todas as camadas com o menor conteúdo possível | agente | `00_INDICE` §6 |
| 11 | 22/11/2026 | Documentação só com o estado atual e o mínimo de texto | Quem lê precisa do que vale agora; o histórico está no Git | herdada | `CLAUDE.md` §3 |
| 12 | 22/11/2026 | IDs de dado em inglês `snake_case` ASCII; texto ao jogador em português em campo separado | O código compara IDs; acento e mistura de idiomas viram erro silencioso | herdada | `CONVENCOES` §3 |
| 13 | 22/11/2026 | CI fixado: ações node24 (`checkout@v7`, `cache@v6`, `setup-python@v7`), runner `ubuntu-24.04` | O CI só muda por PR, como as ferramentas | herdada | `.github/workflows/ci.yml` |
