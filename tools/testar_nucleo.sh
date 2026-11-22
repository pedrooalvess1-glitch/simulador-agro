#!/usr/bin/env bash
# Compila e testa o núcleo C++ (engine/native) em várias configurações e confere o determinismo.
#
# Uso (na raiz do repositório):
#   tools/testar_nucleo.sh            todas as configurações disponíveis nesta máquina
#   tools/testar_nucleo.sh --rapido   só g++ -O2
#
# Para cada configuração: roda os testes unitários e o teste de determinismo do relógio
# (1x = 8x = dormir = save/load) e gera os vetores do detmath. O hash FNV dos vetores precisa
# ser igual ao valor fixado em engine/native/golden/detmath_vectors.fnv. Configurações:
#   g++ -O2, g++ -O0, clang++ -O2, g++ -O3 -march=haswell (FMA no alvo, contração desligada)
#   e MinGW-w64 (só compila; o executável roda no job Windows do CI).
# No fim, mede o erro em ulp contra precisão arbitrária (tools/medir_erro_detmath.py), se o
# mpmath estiver instalado.
set -euo pipefail

RAIZ="$(cd "$(dirname "$0")/.." && pwd)"
NUCLEO="$RAIZ/engine/native"
ESPERADO="$(tr -d '[:space:]' < "$NUCLEO/golden/detmath_vectors.fnv")"
falhas=0

rodar() {  # rodar <rótulo> <args do make>
  local rotulo="$1"; shift
  echo "== $rotulo"
  if ! make -s -C "$NUCLEO" "$@" test > "/tmp/nucleo_$rotulo.log" 2>&1; then
    cat "/tmp/nucleo_$rotulo.log"; echo "   testes FALHARAM"; falhas=$((falhas + 1)); return
  fi
  tail -1 "/tmp/nucleo_$rotulo.log" | sed 's/^/   /'
  local hash
  hash="$(make -s -C "$NUCLEO" "$@" vectors | tail -1 | awk '{print $2}')"
  if [ "$hash" = "$ESPERADO" ]; then
    echo "   vetores detmath: $hash (igual ao fixado)"
  else
    echo "   vetores detmath: $hash DIFERENTE do fixado $ESPERADO"; falhas=$((falhas + 1))
  fi
}

rodar gcc-O2 CXX=g++ OPT=-O2
if [ "${1:-}" != "--rapido" ]; then
  rodar gcc-O0 CXX=g++ OPT=-O0
  command -v clang++ > /dev/null && rodar clang-O2 CXX=clang++ OPT=-O2
  if grep -qw fma /proc/cpuinfo 2> /dev/null && grep -qw avx2 /proc/cpuinfo; then
    rodar gcc-O3-haswell CXX=g++ OPT=-O3 ARCH=-march=haswell
  fi
  if command -v x86_64-w64-mingw32-g++ > /dev/null; then
    echo "== mingw-O2 (compilação para Windows)"
    if make -s -C "$NUCLEO" windows > /tmp/nucleo_mingw.log 2>&1; then
      echo "   compilou; execução no job Windows do CI"
    else
      cat /tmp/nucleo_mingw.log; falhas=$((falhas + 1))
    fi
  fi
fi

if python3 -c "import mpmath" 2> /dev/null; then
  echo "== erro em ulp (mpmath, 120 bits)"
  python3 "$RAIZ/tools/medir_erro_detmath.py" "$NUCLEO/build/g++-O2/detmath_vectors.txt" | sed 's/^/   /' || falhas=$((falhas + 1))
fi

if [ "$falhas" -ne 0 ]; then echo "$falhas configuração(ões) com problema"; exit 1; fi
echo "Núcleo OK em todas as configurações."
