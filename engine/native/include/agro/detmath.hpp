// detmath.hpp — deterministic elementary functions for the simulation core.
//
// Why this exists
//   IEEE 754 requires +, -, *, / and sqrt to be correctly rounded, so a fixed sequence of
//   these operations gives the same bits on every x86-64 compiler and OS, as long as the
//   compiler does not fuse a*b+c into one FMA (-ffp-contract=off) and does not reorder
//   operations (no -ffast-math). exp, log, sin, cos, atan and pow are NOT specified bit by
//   bit by the standard; glibc, the MinGW CRT and MSVC return different last bits.
//   Every function here is built only from the correctly rounded operations above, with a
//   fixed evaluation order, so its result is identical on Linux (CI) and Windows (player).
//
// Algorithms
//   Range reduction and polynomial kernels follow the public-domain fdlibm 5.3 (Sun
//   Microsystems, 1993) as cleaned up in musl libc (MIT). Measured error (tools/
//   medir_erro_detmath.py, mpmath at 120 bits): < 1 ulp for exp, log, sin, cos and atan;
//   < 2 ulp for tan, atan2 and acos; pow carries ln x in double-double and stays below
//   2 + 0.8*|y| ulp. Determinism, not correct rounding, is the requirement of the simulator.
//
// Build requirements (checked below and in the Makefile)
//   x86-64 or AArch64 with SSE2/NEON doubles (FLT_EVAL_METHOD == 0), -ffp-contract=off,
//   no -ffast-math. Domain of sin/cos/tan: |x| <= 1.6e6 rad (medium range reduction);
//   larger arguments return NaN, which the simulation never produces.
#pragma once

#include <cfloat>

#if defined(FLT_EVAL_METHOD) && FLT_EVAL_METHOD != 0
#error "agro::detmath requires FLT_EVAL_METHOD == 0 (SSE2 doubles, no x87 extended precision)"
#endif
#if defined(__FAST_MATH__)
#error "agro::detmath must not be compiled with -ffast-math"
#endif

namespace agro::detmath {

double exp(double x);
double log(double x);
double pow(double x, double y);  // x > 0, or x == 0, or x < 0 with integral y
double sin(double x);
double cos(double x);
double tan(double x);
double atan(double x);
double atan2(double y, double x);
double acos(double x);            // |x| <= 1
double sqrt(double x);            // IEEE correctly rounded (hardware sqrtsd)

// Largest |x| accepted by sin/cos/tan before returning NaN.
inline constexpr double kTrigMaxArg = 1.6e6;

}  // namespace agro::detmath
