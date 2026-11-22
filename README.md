# simulador-agro

Simulador 3D de microgestão de propriedade rural, com aparência e mecânicas de *farming simulator* e foco no manejo técnico de horta e de várias culturas. O jogador testa técnicas de manejo de solo, água, nutrição e fitossanidade sem precisar de uma propriedade real.

Todo o conhecimento agronômico do simulador vem de publicações registradas e citadas, com a **Embrapa** como fonte primária ([`docs/referencias/FONTES.md`](docs/referencias/FONTES.md)).

## Estado
Em planejamento. Estado em [`docs/STATUS.md`](docs/STATUS.md); planejamento em [`docs/00_INDICE.md`](docs/00_INDICE.md).

## Desenvolvimento
Conduzido no Claude Code. Regras permanentes em [`CLAUDE.md`](CLAUDE.md); passos em [`docs/ROTEIRO.md`](docs/ROTEIRO.md). A infraestrutura (Godot 4.7.2 fixado, CI, pipeline Blender e convenções) vem do repositório [`simulador-normas`](https://github.com/pedrooalvess1-glitch/simulador-normas).

Motor: Godot 4.7.2 (renderizador Compatibility), estilo low-poly com cores chapadas, alvo de GPU integrada. Código sob licença MIT.
