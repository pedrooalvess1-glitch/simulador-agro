#!/usr/bin/env python3
"""Testes do tools/conferir_fontes.py: registro, citações em .md e dados com fonte em .json.

Uso: python3 tools/test_conferir_fontes.py  (sai com código 1 se algum caso falhar)
"""
import importlib.util
import json
import sys
import tempfile
from pathlib import Path

AQUI = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("conferir_fontes", AQUI / "conferir_fontes.py")
cf = importlib.util.module_from_spec(spec)
spec.loader.exec_module(cf)

BOA = {
    "id": "EMB-TESTE-2020-ALFACE", "titulo": "Título", "autoria": "Embrapa Teste",
    "tipo": "Circular Técnica", "ano": 2020, "url": "https://www.embrapa.br/x",
    "acesso": "2026-11-22", "licenca": "Embrapa, acesso aberto",
}


FILA = {"id_proposto": "EMB-FILA-2020-X", "referencia": "Autor. Título.", "falta": "ano"}


def rodar(fontes, arquivos, fila=None):
    """Monta um repositório temporário e devolve a lista de erros."""
    with tempfile.TemporaryDirectory() as d:
        raiz = Path(d)
        (raiz / "docs/referencias").mkdir(parents=True)
        registro = {"dominios_permitidos": ["embrapa.br"], "fontes": fontes}
        if fila is not None:
            registro["fila_de_verificacao"] = fila
        (raiz / "docs/referencias/fontes.json").write_text(json.dumps(registro), encoding="utf-8")
        for nome, conteudo in arquivos.items():
            p = raiz / nome
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text(conteudo if isinstance(conteudo, str) else json.dumps(conteudo), encoding="utf-8")
        cf.RAIZ = raiz
        return cf.checar()[0]


CASOS = [  # (nome, fontes, arquivos, trechos esperados nos erros[, fila]; vazio = nenhum erro)
    ("registro e citações válidos", [BOA], {
        "docs/a.md": "Dado [FONTE:EMB-TESTE-2020-ALFACE]. Legenda `[FONTE]` em código.\n```\n[FONTE:X-Y]\n```\n",
        "packs/p/crops/lettuce.json": {"spacing": {"value": 25, "unit": "cm", "fonte": "EMB-TESTE-2020-ALFACE", "local": "p. 7"},
                                       "name": "Alface", "bed_count": 3},
    }, []),
    ("registro vazio é válido", [], {}, []),
    ("citação a ID inexistente", [BOA], {"docs/a.md": "[FONTE:EMB-X-2020-Y]"}, ["EMB-X-2020-Y"]),
    ("rótulo [FONTE] sem ID", [BOA], {"packs/p/README.md": "isto é [FONTE] solto"}, ["sem ID"]),
    ("ID repetido", [BOA, BOA], {}, ["ID repetido"]),
    ("ID fora do formato", [dict(BOA, id="emb-teste")], {}, ["fora do formato"]),
    ("campo ausente", [{k: v for k, v in BOA.items() if k != "licenca"}], {}, ["licenca"]),
    ("domínio não permitido", [dict(BOA, url="https://exemplo.com/x")], {}, ["exemplo.com"]),
    ("URL sem https", [dict(BOA, url="http://www.embrapa.br/x")], {}, ["https"]),
    ("data de acesso inválida", [dict(BOA, acesso="22/11/2026")], {}, ["AAAA-MM-DD"]),
    ("ano inválido", [dict(BOA, ano="2020")], {}, ["'ano'"]),
    ("json com fonte inexistente e sem local", [BOA], {
        "packs/p/x.json": [{"value": 1, "unit": "t/ha", "fonte": "EMB-NAO-2020-EXISTE"}]}, ["não existe", "'local'"]),
    ("dado agronômico sem fonte", [BOA], {"packs/p/x.json": {"kc": {"value": 1.0, "unit": "-"}}}, ["sem 'fonte'"]),
    ("json inválido", [BOA], {"packs/p/x.json": "{nao json"}, ["JSON inválido"]),
    ("pendências como lista de textos", [dict(BOA, pendencias=["confirmar páginas"])], {}, []),
    ("pendências inválidas", [dict(BOA, pendencias="texto")], {}, ["'pendencias'"]),
    ("fila válida", [BOA], {}, [], [FILA]),
    ("item da fila incompleto", [BOA], {}, ["falta"], [{"id_proposto": "X-1", "referencia": "R"}]),
    ("item da fila não pode ser citado", [BOA], {"docs/a.md": "[FONTE:EMB-FILA-2020-X]"}, ["EMB-FILA-2020-X"], [FILA]),
    ("mesmo ID no registro e na fila", [BOA], {}, ["ao mesmo tempo"], [dict(FILA, id_proposto=BOA["id"])]),
    ("etapa da fila vazia", [BOA], {}, ["etapa"], [dict(FILA, etapa=" ")]),
    ("licença ainda a conferir", [dict(BOA, licenca="acesso aberto; licença a conferir")], {}, ["a conferir"]),
    ("etapa da fila preenchida", [BOA], {}, [], [dict(FILA, etapa="V1")]),
]

falhas = []
for caso in CASOS:
    nome, fontes, arquivos, esperado = caso[:4]
    erros = rodar(fontes, arquivos, caso[4] if len(caso) > 4 else None)
    if not esperado and erros:
        falhas.append(f"{nome}: esperava nenhum erro, veio {erros}")
    for trecho in esperado:
        if not any(trecho in e for e in erros):
            falhas.append(f"{nome}: esperava erro com {trecho!r}, veio {erros}")

for f in falhas:
    print("FALHOU:", f)
print(f"{len(CASOS) - len({f.split(':')[0] for f in falhas})}/{len(CASOS)} casos OK")
sys.exit(1 if falhas else 0)
