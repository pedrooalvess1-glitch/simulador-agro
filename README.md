# simulador-agro

Simulador 3D de microgestão de propriedade rural, com aparência e mecânicas de *farming simulator* e foco no manejo técnico de horta e de várias culturas. O jogador testa técnicas de manejo de solo, água, nutrição e fitossanidade sem precisar de uma propriedade real.

Todo o conhecimento agronômico do simulador vem de publicações registradas e citadas, com a **Embrapa** como fonte primária ([`docs/referencias/FONTES.md`](docs/referencias/FONTES.md)).

## Estado
Em planejamento. Estado em [`docs/STATUS.md`](docs/STATUS.md); planejamento em [`docs/00_INDICE.md`](docs/00_INDICE.md).

## Desenvolvimento
Conduzido no Claude Code. Regras permanentes em [`CLAUDE.md`](CLAUDE.md); mapa do projeto, fases e passo a passo em [`docs/MAPA_DO_PROJETO.md`](docs/MAPA_DO_PROJETO.md). A infraestrutura (Godot 4.7.2 fixado, CI, pipeline Blender e convenções) vem do repositório [`simulador-normas`](https://github.com/pedrooalvess1-glitch/simulador-normas).

Motor: Godot 4.7.2 (renderizador Compatibility), estilo low-poly com cores chapadas, alvo de GPU integrada. Todo o estado do mundo calculado fica no núcleo C++ determinístico [`engine/native/`](engine/native/README.md), que dá o mesmo resultado no Linux e no Windows. Código sob licença MIT.
