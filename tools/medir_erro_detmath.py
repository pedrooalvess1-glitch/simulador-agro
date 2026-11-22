#!/usr/bin/env python3
"""Mede o erro, em ulp, das funções de engine/native (detmath) contra precisão arbitrária.

Uso (na raiz do repositório):
    python3 tools/medir_erro_detmath.py engine/native/build/<cfg>/detmath_vectors.txt

Lê as linhas "<função> <x hex> [<y hex>] <resultado hex>" geradas por
engine/native/tools/detmath_vectors.cpp, recalcula cada valor com mpmath a 120 dígitos
binários e imprime o maior erro por função. Sai com código 1 se algum erro passar do limite
da tabela LIMITE_ULP (o contrato de precisão documentado em detmath.hpp).

Requisito: Python 3.9+ e mpmath (pip install mpmath).
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

try:
    import mpmath as mp
except ImportError:  # pragma: no cover - mensagem para quem roda sem o pacote
    print("mpmath não instalado: pip install mpmath")
    sys.exit(2)

mp.mp.prec = 120

# Limite de erro aceito, em ulp do resultado (contrato de detmath.hpp). pow = exp(y·ln x)
# carrega ln x em double-double; o erro cresce só com |y|: o limite de pow é 2 + 0,8·|y| ulp,
# conferido ponto a ponto. A coluna mostra o pior erro dividido pelo limite do ponto.
LIMITE_ULP = {"exp": 1.0, "log": 1.0, "sin": 1.0, "cos": 1.0, "tan": 2.0,
              "atan": 1.0, "atan2": 2.0, "acos": 2.0}


def limite(fn: str, args: list[float]) -> float:
    if fn == "pow":
        import math
        return 2.0 + 0.8 * abs(args[1])
    return LIMITE_ULP[fn]


def de_hex(h: str) -> float:
    return struct.unpack("<d", int(h, 16).to_bytes(8, "little"))[0]


def ulp(v: float) -> float:
    import math
    if v == 0.0:
        return 2.0 ** -1074
    return math.ulp(v)


def referencia(fn: str, args: list[float]) -> "mp.mpf":
    a = [mp.mpf(x) for x in args]
    if fn == "exp":
        return mp.exp(a[0])
    if fn == "log":
        return mp.log(a[0])
    if fn == "sin":
        return mp.sin(a[0])
    if fn == "cos":
        return mp.cos(a[0])
    if fn == "tan":
        return mp.tan(a[0])
    if fn == "atan":
        return mp.atan(a[0])
    if fn == "atan2":
        return mp.atan2(a[0], a[1])
    if fn == "acos":
        return mp.acos(a[0])
    if fn == "pow":
        return mp.power(a[0], a[1])
    raise ValueError(fn)


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__)
        return 2
    pior: dict[str, float] = {}
    maximo: dict[str, float] = {}
    contagem: dict[str, int] = {}
    for linha in Path(sys.argv[1]).read_text().splitlines():
        partes = linha.split()
        if not partes or partes[0] == "FNV":
            continue
        fn, valores = partes[0], [de_hex(h) for h in partes[1:]]
        args, obtido = valores[:-1], valores[-1]
        ref = referencia(fn, args)
        if obtido == 0.0 and abs(ref) < mp.mpf(2) ** -1074:
            erro = 0.0
        else:
            erro = float(abs(mp.mpf(obtido) - ref) / mp.mpf(ulp(float(ref))))
        pior[fn] = max(pior.get(fn, 0.0), erro / limite(fn, args))
        maximo[fn] = max(maximo.get(fn, 0.0), erro)
        contagem[fn] = contagem.get(fn, 0) + 1
    falhou = False
    for fn in sorted(pior):
        ok = pior[fn] <= 1.0
        falhou |= not ok
        lim = "2 + 0,8·|y|" if fn == "pow" else str(LIMITE_ULP[fn])
        print(f"{fn:6s} {contagem[fn]:6d} pontos  erro máximo {maximo[fn]:7.3f} ulp  (limite {lim} ulp; uso {pior[fn]:.0%})  {'ok' if ok else 'ACIMA DO LIMITE'}")
    return 1 if falhou else 0


if __name__ == "__main__":
    sys.exit(main())
