#!/usr/bin/env python3
"""Confere o registro de fontes e todas as citações do repositório.

Uso (na raiz do repositório):
    python3 tools/conferir_fontes.py

O que faz:
  1. Lê docs/referencias/fontes.json e confere cada fonte: campos obrigatórios, ID no formato
     da CONVENCOES §1, ID único, ano inteiro, data de acesso AAAA-MM-DD, URL https num domínio
     da lista "dominios_permitidos" e, se houver, "pendencias" como lista de textos.
     Confere também a "fila_de_verificacao" (publicações achadas e ainda não registradas):
     cada item tem "id_proposto", "referencia" e "falta". Item da fila não pode ser citado.
  2. Percorre os .md de docs/ e packs/ (fora de blocos de código) e confere cada citação
     [FONTE:ID]: o ID precisa existir no registro. Rótulo [FONTE] sem ID é erro.
  3. Percorre os .json de packs/ e confere cada objeto:
       - com a chave "fonte": o ID existe no registro e "local" não está vazio;
       - no formato de dado agronômico ({"value": número, "unit": ...}): precisa ter "fonte".
  4. Sai com código 1 e lista os erros, para o CI barrar o PR.

Só lê arquivos; não altera nada. Requisito: Python 3.9+ (só biblioteca padrão).
"""
from __future__ import annotations

import json
import os
import re
import sys
from datetime import date
from pathlib import Path
from urllib.parse import urlparse

RAIZ = Path(__file__).resolve().parent.parent
REGISTRO = Path("docs/referencias/fontes.json")
PASTAS_MD = ("docs", "packs")
PASTA_JSON = "packs"
IGNORAR = {".git", "node_modules", ".godot", "__pycache__", "pdf"}

CAMPOS = ("id", "titulo", "autoria", "tipo", "ano", "url", "acesso", "licenca")
FORMATO_ID = re.compile(r"^[A-Z0-9]+(?:-[A-Z0-9]+)+$")
CITACAO = re.compile(r"\[FONTE:([^\]\s]+)\]")
FONTE_SEM_ID = re.compile(r"\[FONTE\]")
CODIGO = re.compile(r"```.*?```|`[^`\n]*`", re.S)


def _arquivos(pasta: str, sufixo: str) -> list[Path]:
    base = RAIZ / pasta
    if not base.is_dir():
        return []
    return sorted(
        p for p in base.rglob(f"*{sufixo}")
        if not any(parte in IGNORAR for parte in p.relative_to(RAIZ).parts)
    )


def conferir_registro(erros: list[str]) -> set[str]:
    """Valida o registro e devolve o conjunto de IDs válidos."""
    caminho = RAIZ / REGISTRO
    try:
        dados = json.loads(caminho.read_text(encoding="utf-8"))
    except FileNotFoundError:
        erros.append(f"{REGISTRO}: registro de fontes não existe")
        return set()
    except json.JSONDecodeError as e:
        erros.append(f"{REGISTRO}: JSON inválido ({e})")
        return set()

    dominios = dados.get("dominios_permitidos")
    fontes = dados.get("fontes")
    if not isinstance(dominios, list) or not all(isinstance(d, str) and d for d in dominios):
        erros.append(f"{REGISTRO}: 'dominios_permitidos' precisa ser uma lista de domínios")
        dominios = []
    if not isinstance(fontes, list):
        erros.append(f"{REGISTRO}: 'fontes' precisa ser uma lista")
        return set()

    ids: set[str] = set()
    fila = dados.get("fila_de_verificacao", [])
    if not isinstance(fila, list):
        erros.append(f"{REGISTRO}: 'fila_de_verificacao' precisa ser uma lista")
        fila = []
    for i, item in enumerate(fila):
        onde = f"{REGISTRO} fila_de_verificacao[{i}]"
        if not isinstance(item, dict):
            erros.append(f"{onde}: cada item precisa ser um objeto")
            continue
        faltam = [c for c in ("id_proposto", "referencia", "falta") if item.get(c) in (None, "")]
        if faltam:
            erros.append(f"{onde}: campos obrigatórios ausentes: {', '.join(faltam)}")
    for i, f in enumerate(fontes):
        onde = f"{REGISTRO} fontes[{i}]"
        if not isinstance(f, dict):
            erros.append(f"{onde}: cada fonte precisa ser um objeto")
            continue
        faltam = [c for c in CAMPOS if f.get(c) in (None, "")]
        if faltam:
            erros.append(f"{onde}: campos obrigatórios ausentes: {', '.join(faltam)}")
        fid = f.get("id")
        if isinstance(fid, str) and fid:
            onde = f"{REGISTRO} {fid}"
            if not FORMATO_ID.match(fid):
                erros.append(f"{onde}: ID fora do formato (maiúsculas, números e hífens; ex.: EMB-CNPH-2020-ALFACE)")
            if fid in ids:
                erros.append(f"{onde}: ID repetido")
            ids.add(fid)
        ano = f.get("ano")
        if ano is not None and (not isinstance(ano, int) or isinstance(ano, bool) or not 1900 <= ano <= date.today().year):
            erros.append(f"{onde}: 'ano' precisa ser um inteiro entre 1900 e o ano atual")
        acesso = f.get("acesso")
        if isinstance(acesso, str) and acesso:
            try:
                if date.fromisoformat(acesso) > date.today():
                    erros.append(f"{onde}: data de acesso no futuro")
            except ValueError:
                erros.append(f"{onde}: 'acesso' precisa estar no formato AAAA-MM-DD")
        pend = f.get("pendencias")
        if pend is not None and (not isinstance(pend, list) or not all(isinstance(x, str) and x.strip() for x in pend)):
            erros.append(f"{onde}: 'pendencias' precisa ser uma lista de textos")
        url = f.get("url")
        if isinstance(url, str) and url:
            u = urlparse(url)
            host = (u.hostname or "").lower()
            if u.scheme != "https":
                erros.append(f"{onde}: URL precisa ser https")
            elif not any(host == d or host.endswith("." + d) for d in dominios):
                erros.append(f"{onde}: domínio '{host}' fora de 'dominios_permitidos'")
    na_fila = {item.get("id_proposto") for item in fila if isinstance(item, dict)}
    for fid in sorted(na_fila & ids):
        erros.append(f"{REGISTRO}: '{fid}' está no registro e na fila ao mesmo tempo")
    return ids


def conferir_md(ids: set[str], erros: list[str]) -> int:
    total = 0
    for pasta in PASTAS_MD:
        for md in _arquivos(pasta, ".md"):
            texto = CODIGO.sub("", md.read_text(encoding="utf-8"))
            onde = md.relative_to(RAIZ)
            for fid in CITACAO.findall(texto):
                total += 1
                if fid not in ids:
                    erros.append(f"{onde}: citação [FONTE:{fid}] — ID não existe no registro")
            if FONTE_SEM_ID.search(texto):
                erros.append(f"{onde}: rótulo [FONTE] sem ID; use [FONTE:ID]")
    return total


def _percorrer(no, caminho: str, arquivo: Path, ids: set[str], erros: list[str]) -> int:
    total = 0
    if isinstance(no, dict):
        onde = f"{arquivo.relative_to(RAIZ)} {caminho or '/'}"
        if "fonte" in no:
            total += 1
            if no["fonte"] not in ids:
                erros.append(f"{onde}: fonte '{no['fonte']}' não existe no registro")
            local = no.get("local")
            if not isinstance(local, str) or not local.strip():
                erros.append(f"{onde}: 'local' (página, tabela ou seção) ausente")
        elif "value" in no and "unit" in no and isinstance(no["value"], (int, float)) and not isinstance(no["value"], bool):
            erros.append(f"{onde}: dado agronômico sem 'fonte' (CONVENCOES §4)")
        for k, v in no.items():
            total += _percorrer(v, f"{caminho}/{k}", arquivo, ids, erros)
    elif isinstance(no, list):
        for i, v in enumerate(no):
            total += _percorrer(v, f"{caminho}/{i}", arquivo, ids, erros)
    return total


def conferir_json(ids: set[str], erros: list[str]) -> int:
    total = 0
    for arq in _arquivos(PASTA_JSON, ".json"):
        try:
            dados = json.loads(arq.read_text(encoding="utf-8"))
        except json.JSONDecodeError as e:
            erros.append(f"{arq.relative_to(RAIZ)}: JSON inválido ({e})")
            continue
        total += _percorrer(dados, "", arq, ids, erros)
    return total


def checar() -> tuple[list[str], int, int]:
    erros: list[str] = []
    ids = conferir_registro(erros)
    citacoes = conferir_md(ids, erros) + conferir_json(ids, erros)
    return erros, len(ids), citacoes


def main() -> int:
    erros, n_fontes, n_citacoes = checar()
    if erros:
        print(f"{len(erros)} problema(s) no registro de fontes ou nas citações:")
        anotar = bool(os.environ.get("GITHUB_ACTIONS"))
        for e in erros:
            print(f"::error::{e}" if anotar else f"  - {e}")
        return 1
    print(f"Fontes OK: {n_fontes} fonte(s) no registro, {n_citacoes} citação(ões) conferida(s).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
