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
| 7 | 22/11/2026 | Embrapa como fonte primária; INMET, MAPA/Agrofit e outros órgãos públicos brasileiros para lacunas; equações podem vir de manual técnico, livro ou artigo | Todo elemento do modelo precisa de fundamentação; os dados brasileiros vêm de quem os mede | usuário | `00_INDICE` §1.4 |
| 8 | 22/11/2026 | PDFs de terceiros fora do Git; registro guarda referência, URL e data de acesso | Repositório público; a licença de cada publicação ainda não foi conferida | agente | `docs/referencias/FONTES.md` |
| 9 | 22/11/2026 | Modelos em camadas de taxa (tick, hora, dia, evento) com passo fixo em tempo simulado | Cada processo roda no passo que a sua constante de tempo pede; passo fixo mantém a estabilidade | agente | `BASE_MODELOS` §2.2 |
| 10 | 22/11/2026 | Fatia vertical = um canteiro plano de alface, ciclo completo | Atravessa todas as camadas com o menor conteúdo possível. Alface tem índice de colheita quase constante e dispensa florescimento e partição para frutos | usuário (alface: agente) | `00_INDICE` §6, `BASE_MODELOS` §3 |
| 11 | 22/11/2026 | Documentação só com o estado atual e o mínimo de texto | Quem lê precisa do que vale agora; o histórico está no Git | herdada | `CLAUDE.md` §3 |
| 12 | 22/11/2026 | IDs de dado em inglês `snake_case` ASCII; texto ao jogador em português em campo separado | O código compara IDs; acento e mistura de idiomas viram erro silencioso | herdada | `CONVENCOES` §3 |
| 13 | 22/11/2026 | CI fixado: ações node24 (`checkout@v7`, `cache@v6`, `setup-python@v7`), runner `ubuntu-24.04` | O CI só muda por PR, como as ferramentas | herdada | `.github/workflows/ci.yml` |
| 14 | 22/11/2026 | Brasil inteiro como escopo | O software não é só para Pernambuco; modelos parametrizados por região, solo e clima | usuário | `00_INDICE` §1 |
| 15 | 22/11/2026 | Defensivos químicos entram no simulador | Decisão do usuário; dose e carência vêm do Agrofit | usuário | `BASE_MODELOS` §4 |
| 16 | 22/11/2026 | Relógio de 24 para 1 com 2x, 4x e 8x; o multiplicador muda o número de passos por segundo real e nunca o tamanho do passo | Passo fixo deixa 1x e 8x com o mesmo resultado e evita divergência de integradores explícitos; o custo cabe no notebook | usuário (mecanismo: agente) | `BASE_MODELOS` §2.1 |
| 17 | 22/11/2026 | Física do protagonista e das máquinas em tempo real, fora do multiplicador | Acelerar a física do Godot exige mais passos por segundo (480 Hz em 8x) | agente | `BASE_MODELOS` §2.2 |
| 18 | 22/11/2026 | Gerador pseudoaleatório por subsistema com semente `hash(semente_mundo, subsistema, dia)` | Um subsistema não altera a sequência aleatória de outro; dias podem ser pulados sem divergência | usuário | `BASE_MODELOS` §2.4 |
| 19 | 22/11/2026 | Limite de passos por quadro e modo dormir | Evita a espiral da morte; dormir roda o mesmo laço sem renderizar | usuário | `BASE_MODELOS` §2.4 |
| 20 | 22/11/2026 | Salvar e carregar com registro de ações | O save guarda estado, tick, versões e semente; o registro de ações permite replay | usuário | `BASE_MODELOS` §2.4 |
| 21 | 22/11/2026 | Protagonista jogável em primeira e terceira pessoa | O jogador vive a propriedade como no Farming Simulator e no Medieval Dynasty | usuário | `00_INDICE` §1 |
| 22 | 22/11/2026 | Produto final é um mundo semiaberto; economia, combustível, NPCs, cidades e animais em fases futuras | A fase inicial foca o ambiente produtivo da propriedade | usuário | `00_INDICE` §7 |
| 23 | 22/11/2026 | Campanha ensina o básico e começa em área plana, depois inclinada; eventos aleatórios só no modo semiaberto | A declividade introduz escoamento e erosão depois que o básico foi aprendido | usuário | `00_INDICE` §1.3 |
| 24 | 22/11/2026 | Deformação da malha do terreno ao arar, passar o microtrator e cavar | Pedido do usuário; técnica a definir na segunda pesquisa | usuário | `00_INDICE` §2.1 |
| 25 | 22/11/2026 | Critérios de escolha de modelos C1–C7 | Baixa complexidade, escalabilidade, dados brasileiros, vários cultivos, estabilidade em acoplamento, poder explicativo, documentação aberta | usuário | `BASE_MODELOS` §1 |
| 26 | 22/11/2026 | Conjunto de modelos da fase 1 (FAO-56, INMET, bucket com Curve Number, calagem por saturação por bases, AquaCrop simplificado, Cousens) | Primeira pesquisa; atende C1, C3, C5 e C7 com o menor número de parâmetros | agente (a validar) | `BASE_MODELOS` §3 |
