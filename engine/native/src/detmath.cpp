// detmath.cpp — see detmath.hpp for the rationale.
//
// Derived from fdlibm 5.3: "Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
// Developed at SunPro, a Sun Microsystems, Inc. business. Permission to use, copy, modify, and
// distribute this software is freely granted, provided that this notice is preserved."
// and from the musl libc versions of the same routines (MIT licence, Copyright 2005-2020 Rich
// Felker et al.). Only correctly rounded IEEE operations are used; the order of every
// expression is part of the contract and must not be "simplified".
#include "agro/detmath.hpp"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace agro::detmath {
namespace {

inline std::uint64_t bits(double x) {
  std::uint64_t u;
  std::memcpy(&u, &x, sizeof u);
  return u;
}
inline double from_bits(std::uint64_t u) {
  double x;
  std::memcpy(&x, &u, sizeof x);
  return x;
}
inline std::uint32_t high_word(double x) { return static_cast<std::uint32_t>(bits(x) >> 32); }
inline std::uint32_t low_word(double x) { return static_cast<std::uint32_t>(bits(x)); }
inline double with_high_word(double x, std::uint32_t hi) {
  return from_bits((static_cast<std::uint64_t>(hi) << 32) | (bits(x) & 0xffffffffULL));
}
inline double nan_value() { return std::numeric_limits<double>::quiet_NaN(); }
inline double inf_value() { return std::numeric_limits<double>::infinity(); }

// x * 2^k for a finite x, built by exponent manipulation (exact unless the result is
// subnormal, where one rounding happens in the final multiplication).
double scale_by_pow2(double x, int k) {
  if (k > 1023) {
    x *= from_bits(0x7fe0000000000000ULL);  // 2^1023
    k -= 1023;
    if (k > 1023) {
      x *= from_bits(0x7fe0000000000000ULL);
      k -= 1023;
      if (k > 1023) k = 1023;
    }
  } else if (k < -1022) {
    x *= from_bits(0x0010000000000000ULL) * from_bits(0x4340000000000000ULL);  // 2^-1022 * 2^53
    k += 1022 - 53;
    if (k < -1022) {
      x *= from_bits(0x0010000000000000ULL) * from_bits(0x4340000000000000ULL);
      k += 1022 - 53;
      if (k < -1022) k = -1022;
    }
  }
  return x * from_bits(static_cast<std::uint64_t>(0x3ff + k) << 52);
}

// ---------------------------------------------------------------- exp (fdlibm e_exp.c)
constexpr double kHalf[2] = {0.5, -0.5};
constexpr double kLn2Hi[2] = {6.93147180369123816490e-01, -6.93147180369123816490e-01};
constexpr double kLn2Lo[2] = {1.90821492927058770002e-10, -1.90821492927058770002e-10};
constexpr double kInvLn2 = 1.44269504088896338700e+00;
constexpr double kP1 = 1.66666666666666019037e-01;
constexpr double kP2 = -2.77777777770155933842e-03;
constexpr double kP3 = 6.61375632143793436117e-05;
constexpr double kP4 = -1.65339022054652515390e-06;
constexpr double kP5 = 4.13813679705723846039e-08;
constexpr double kExpOverflow = 7.09782712893383973096e+02;
constexpr double kExpUnderflow = -7.45133219101941108420e+02;

// --------------------------------------------------------------- log (fdlibm e_log.c)
constexpr double kTwo54 = 1.80143985094819840000e+16;
constexpr double kLg1 = 6.666666666666735130e-01;
constexpr double kLg2 = 3.999999999940941908e-01;
constexpr double kLg3 = 2.857142874366239149e-01;
constexpr double kLg4 = 2.222219843214978396e-01;
constexpr double kLg5 = 1.818357216161805012e-01;
constexpr double kLg6 = 1.531383769920937332e-01;
constexpr double kLg7 = 1.479819860511658591e-01;
constexpr double kLogLn2Hi = 6.93147180369123816490e-01;
constexpr double kLogLn2Lo = 1.90821492927058770002e-10;

// ---------------------------------------------- sin/cos kernels (musl __sin.c, __cos.c)
constexpr double kS1 = -1.66666666666666324348e-01;
constexpr double kS2 = 8.33333333332248946124e-03;
constexpr double kS3 = -1.98412698298579493134e-04;
constexpr double kS4 = 2.75573137070700676789e-06;
constexpr double kS5 = -2.50507602534068634195e-08;
constexpr double kS6 = 1.58969099521155010221e-10;
constexpr double kC1 = 4.16666666666666019037e-02;
constexpr double kC2 = -1.38888888888741095749e-03;
constexpr double kC3 = 2.48015872894767294178e-05;
constexpr double kC4 = -2.75573143513906633035e-07;
constexpr double kC5 = 2.08757232129817482790e-09;
constexpr double kC6 = -1.13596475577881948265e-11;

double kernel_sin(double x, double y, int iy) {
  const double z = x * x;
  const double w = z * z;
  const double r = kS2 + z * (kS3 + z * kS4) + z * w * (kS5 + z * kS6);
  const double v = z * x;
  if (iy == 0) return x + v * (kS1 + z * r);
  return x - ((z * (0.5 * y - v * r) - y) - v * kS1);
}

double kernel_cos(double x, double y) {
  const double z = x * x;
  double w = z * z;
  const double r = z * (kC1 + z * (kC2 + z * kC3)) + w * w * (kC4 + z * (kC5 + z * kC6));
  const double hz = 0.5 * z;
  w = 1.0 - hz;
  return w + (((1.0 - w) - hz) + (z * r - x * y));
}

// tan kernel (fdlibm k_tan.c), |x| <= pi/4, x + y the reduced argument, iy = 1 for tan,
// -1 for -1/tan.
constexpr double kT[13] = {
    3.33333333333334091986e-01, 1.33333333333201242699e-01, 5.39682539762260521377e-02,
    2.18694882948595424599e-02, 8.86323982359930005737e-03, 3.59207910759131235356e-03,
    1.45620945432529025516e-03, 5.88041240820264096874e-04, 2.46463134818469906812e-04,
    7.81794442939557092300e-05, 7.14072491382608190305e-05, -1.85586374855275456654e-05,
    2.59073051863633712884e-05,
};
constexpr double kPio4 = 7.85398163397448278999e-01;
constexpr double kPio4Lo = 3.06161699786838301793e-17;

double kernel_tan(double x, double y, int iy) {
  const std::int32_t hx = static_cast<std::int32_t>(high_word(x));
  const std::uint32_t ix = static_cast<std::uint32_t>(hx) & 0x7fffffffU;
  const bool big = ix >= 0x3FE59428U;  // |x| >= 0.6744
  if (big) {
    if (hx < 0) {
      x = -x;
      y = -y;
    }
    x = (kPio4 - x) + (kPio4Lo - y);
    y = 0.0;
  }
  double z = x * x;
  double w = z * z;
  double r = kT[1] + w * (kT[3] + w * (kT[5] + w * (kT[7] + w * (kT[9] + w * kT[11]))));
  double v = z * (kT[2] + w * (kT[4] + w * (kT[6] + w * (kT[8] + w * (kT[10] + w * kT[12])))));
  double s = z * x;
  r = y + z * (s * (r + v) + y);
  r += kT[0] * s;
  w = x + r;
  if (big) {
    v = static_cast<double>(iy);
    return static_cast<double>(1 - ((hx >> 30) & 2)) * (v - 2.0 * (x - (w * w / (w + v) - r)));
  }
  if (iy == 1) return w;
  // -1/(x+r) computed carefully (fdlibm)
  z = from_bits(bits(w) & 0xffffffff00000000ULL);
  v = r - (z - x);
  const double a = -1.0 / w;
  const double t = from_bits(bits(a) & 0xffffffff00000000ULL);
  s = 1.0 + t * z;
  return t + a * (s + t * v);
}

// ------------------------------- medium range reduction for trig (musl __rem_pio2.c)
constexpr double kToInt = 6755399441055744.0;  // 1.5 / DBL_EPSILON
constexpr double kInvPio2 = 6.36619772367581382433e-01;
constexpr double kPio2_1 = 1.57079632673412561417e+00;
constexpr double kPio2_1t = 6.07710050650619224932e-11;
constexpr double kPio2_2 = 6.07710050630396597660e-11;
constexpr double kPio2_2t = 2.02226624879595063154e-21;
constexpr double kPio2_3 = 2.02226624871116645580e-21;
constexpr double kPio2_3t = 8.47842766036889956997e-32;

// Returns n and y[0] + y[1] = x - n*pi/2 with |y| <= ~pi/4.
int rem_pio2(double x, double y[2]) {
  const std::uint32_t ix = high_word(x) & 0x7fffffffU;
  double fn = x * kInvPio2 + kToInt - kToInt;
  int n = static_cast<int>(fn);
  double r = x - fn * kPio2_1;
  double w = fn * kPio2_1t;
  if (r - w < -kPio4) {
    n--;
    fn -= 1.0;
    r = x - fn * kPio2_1;
    w = fn * kPio2_1t;
  } else if (r - w > kPio4) {
    n++;
    fn += 1.0;
    r = x - fn * kPio2_1;
    w = fn * kPio2_1t;
  }
  y[0] = r - w;
  const int ex = static_cast<int>(ix >> 20);
  int ey = static_cast<int>((bits(y[0]) >> 52) & 0x7ff);
  if (ex - ey > 16) {
    double t = r;
    w = fn * kPio2_2;
    r = t - w;
    w = fn * kPio2_2t - ((t - r) - w);
    y[0] = r - w;
    ey = static_cast<int>((bits(y[0]) >> 52) & 0x7ff);
    if (ex - ey > 49) {
      t = r;
      w = fn * kPio2_3;
      r = t - w;
      w = fn * kPio2_3t - ((t - r) - w);
      y[0] = r - w;
    }
  }
  y[1] = (r - y[0]) - w;
  return n;
}

// ------------------------------------------------------------- atan (fdlibm s_atan.c)
constexpr double kAtanHi[4] = {4.63647609000806093515e-01, 7.85398163397448278999e-01,
                               9.82793723247329054082e-01, 1.57079632679489655800e+00};
constexpr double kAtanLo[4] = {2.26987774529616870924e-17, 3.06161699786838301793e-17,
                               1.39033110312309984516e-17, 6.12323399573676603587e-17};
constexpr double kAT[11] = {
    3.33333333333329318027e-01, -1.99999999998764832476e-01, 1.42857142725034663711e-01,
    -1.11111104054623557880e-01, 9.09088713343650656196e-02, -7.69187620504482999495e-02,
    6.66107313738753120669e-02, -5.83357013379057348645e-02, 4.97687799461593236017e-02,
    -3.65315727442169155270e-02, 1.62858201153657823623e-02,
};
constexpr double kPi = 3.1415926535897931160e+00;
constexpr double kPiLo = 1.2246467991473531772e-16;
constexpr double kPio2 = 1.5707963267948965580e+00;

}  // namespace

double sqrt(double x) { return std::sqrt(x); }

double exp(double x) {
  if (x != x) return x;  // NaN
  if (x > kExpOverflow) return inf_value();
  if (x < kExpUnderflow) return 0.0;
  const std::uint32_t hx = high_word(x);
  const int sign = static_cast<int>(hx >> 31);
  const std::uint32_t ax = hx & 0x7fffffffU;
  double hi = 0.0, lo = 0.0;
  int k = 0;
  if (ax > 0x3fd62e42U) {        // |x| > 0.5 ln2
    if (ax < 0x3FF0A2B2U) {      // |x| < 1.5 ln2
      hi = x - kLn2Hi[sign];
      lo = kLn2Lo[sign];
      k = 1 - sign - sign;
    } else {
      k = static_cast<int>(kInvLn2 * x + kHalf[sign]);
      const double t = static_cast<double>(k);
      hi = x - t * kLn2Hi[0];
      lo = t * kLn2Lo[0];
    }
    x = hi - lo;
  } else if (ax < 0x3e300000U) {  // |x| < 2^-28
    return 1.0 + x;
  }
  const double t = x * x;
  const double c = x - t * (kP1 + t * (kP2 + t * (kP3 + t * (kP4 + t * kP5))));
  if (k == 0) return 1.0 - ((x * c) / (c - 2.0) - x);
  const double y = 1.0 - ((lo - (x * c) / (2.0 - c)) - hi);
  return scale_by_pow2(y, k);
}

double log(double x) {
  std::int32_t hx = static_cast<std::int32_t>(high_word(x));
  const std::uint32_t lx = low_word(x);
  int k = 0;
  if (hx < 0x00100000) {  // x < 2^-1022
    if (((hx & 0x7fffffff) | static_cast<std::int32_t>(lx)) == 0) return -inf_value();
    if (hx < 0) return nan_value();
    k -= 54;
    x *= kTwo54;
    hx = static_cast<std::int32_t>(high_word(x));
  }
  if (hx >= 0x7ff00000) return x + x;  // inf or NaN
  k += (hx >> 20) - 1023;
  hx &= 0x000fffff;
  std::int32_t i = (hx + 0x95f64) & 0x100000;
  x = with_high_word(x, static_cast<std::uint32_t>(hx | (i ^ 0x3ff00000)));  // x or x/2 in [sqrt2/2, sqrt2]
  k += (i >> 20);
  const double f = x - 1.0;
  const double dk = static_cast<double>(k);
  if ((0x000fffff & (2 + hx)) < 3) {  // -2^-20 <= f < 2^-20
    if (f == 0.0) {
      if (k == 0) return 0.0;
      return dk * kLogLn2Hi + dk * kLogLn2Lo;
    }
    const double R = f * f * (0.5 - 0.33333333333333333 * f);
    if (k == 0) return f - R;
    return dk * kLogLn2Hi - ((R - dk * kLogLn2Lo) - f);
  }
  const double s = f / (2.0 + f);
  const double z = s * s;
  i = hx - 0x6147a;
  const double w = z * z;
  const std::int32_t j = 0x6b851 - hx;
  const double t1 = w * (kLg2 + w * (kLg4 + w * kLg6));
  const double t2 = z * (kLg1 + w * (kLg3 + w * (kLg5 + w * kLg7)));
  i |= j;
  const double R = t2 + t1;
  if (i > 0) {
    const double hfsq = 0.5 * f * f;
    if (k == 0) return f - (hfsq - s * (hfsq + R));
    return dk * kLogLn2Hi - ((hfsq - (s * (hfsq + R) + dk * kLogLn2Lo)) - f);
  }
  if (k == 0) return f - s * (f - R);
  return dk * kLogLn2Hi - ((s * (f - R) - dk * kLogLn2Lo) - f);
}

namespace {

// Error-free transformations (Knuth two-sum, Dekker two-product with Veltkamp splitting).
// They need correctly rounded + - * and no FMA contraction, both guaranteed by the build.
inline void two_sum(double a, double b, double& s, double& e) {
  s = a + b;
  const double bb = s - a;
  e = (a - (s - bb)) + (b - bb);
}
inline void split(double a, double& hi, double& lo) {
  const double c = 134217729.0 * a;  // 2^27 + 1
  hi = c - (c - a);
  lo = a - hi;
}
inline void two_prod(double a, double b, double& p, double& e) {
  p = a * b;
  double ah, al, bh, bl;
  split(a, ah, al);
  split(b, bh, bl);
  e = ((ah * bh - p) + ah * bl + al * bh) + al * bl;
}

// x > 0, finite, y finite and not a small integer. x = 2^k * m with m in [sqrt(1/2), sqrt(2)),
// ln x = k*ln2 + ln m carried in double-double, so the error does not grow with |ln x|;
// it grows only with |y| (about 2 + 0.8*|y| ulp).
double pow_positive(double x, double y) {
  int k = 0;
  if (x < 2.2250738585072014e-308) {  // subnormal: scale by 2^54 first
    x *= kTwo54;
    k = -54;
  }
  const std::uint64_t u = bits(x);
  k += static_cast<int>((u >> 52) & 0x7ff) - 1023;
  double m = from_bits((u & 0x000fffffffffffffULL) | 0x3ff0000000000000ULL);  // [1, 2)
  if (m > 1.4142135623730951) {
    m *= 0.5;  // exact
    k += 1;
  }
  const double lm = log(m);                          // |lm| <= 0.347
  const double dk = static_cast<double>(k);
  double s_hi, s_lo;
  two_sum(dk * kLogLn2Hi, lm, s_hi, s_lo);           // dk*ln2_hi is exact (ln2_hi has 21 trailing zero bits)
  s_lo += dk * kLogLn2Lo;
  double z_hi, z_lo;
  two_prod(y, s_hi, z_hi, z_lo);
  z_lo += y * s_lo;
  double z, zz;
  two_sum(z_hi, z_lo, z, zz);
  if (z > kExpOverflow) return inf_value();
  if (z < kExpUnderflow) return 0.0;
  const double e = exp(z);
  return e + e * zz;
}

}  // namespace

double pow(double x, double y) {
  if (y == 0.0) return 1.0;
  if (x != x || y != y) return nan_value();
  if (x == 1.0) return 1.0;
  if (x == 0.0) return y > 0.0 ? 0.0 : inf_value();
  const bool integral = std::floor(y) == y;  // floor is exact
  if (x < 0.0 && !integral) return nan_value();
  const double half = y * 0.5;
  const bool odd = integral && std::floor(half) != half;
  const double ax = std::fabs(x);
  double m;
  if (integral && std::fabs(y) <= 1024.0) {
    // Binary exponentiation: exact whenever the result is representable (2^10, 3^4...),
    // a fixed sequence of correctly rounded products otherwise.
    std::uint32_t n = static_cast<std::uint32_t>(std::fabs(y));
    double base = ax;
    m = 1.0;
    while (n != 0) {
      if (n & 1U) m *= base;
      n >>= 1;
      if (n != 0) base *= base;
    }
    if (y < 0.0) m = 1.0 / m;
  } else {
    m = pow_positive(ax, y);
  }
  return (x < 0.0 && odd) ? -m : m;
}

double sin(double x) {
  const std::uint32_t ix = high_word(x) & 0x7fffffffU;
  if (ix <= 0x3fe921fbU) {           // |x| ~<= pi/4
    if (ix < 0x3e500000U) return x;  // |x| < 2^-26
    return kernel_sin(x, 0.0, 0);
  }
  if (ix >= 0x7ff00000U) return nan_value();
  if (!(x <= kTrigMaxArg && x >= -kTrigMaxArg)) return nan_value();
  double y[2];
  const int n = rem_pio2(x, y);
  switch (n & 3) {
    case 0: return kernel_sin(y[0], y[1], 1);
    case 1: return kernel_cos(y[0], y[1]);
    case 2: return -kernel_sin(y[0], y[1], 1);
    default: return -kernel_cos(y[0], y[1]);
  }
}

double cos(double x) {
  const std::uint32_t ix = high_word(x) & 0x7fffffffU;
  if (ix <= 0x3fe921fbU) {
    if (ix < 0x3e46a09eU) return 1.0;  // |x| < 2^-27 * sqrt(2)
    return kernel_cos(x, 0.0);
  }
  if (ix >= 0x7ff00000U) return nan_value();
  if (!(x <= kTrigMaxArg && x >= -kTrigMaxArg)) return nan_value();
  double y[2];
  const int n = rem_pio2(x, y);
  switch (n & 3) {
    case 0: return kernel_cos(y[0], y[1]);
    case 1: return -kernel_sin(y[0], y[1], 1);
    case 2: return -kernel_cos(y[0], y[1]);
    default: return kernel_sin(y[0], y[1], 1);
  }
}

double tan(double x) {
  const std::uint32_t ix = high_word(x) & 0x7fffffffU;
  if (ix <= 0x3fe921fbU) {
    if (ix < 0x3e400000U) return x;  // |x| < 2^-27
    return kernel_tan(x, 0.0, 1);
  }
  if (ix >= 0x7ff00000U) return nan_value();
  if (!(x <= kTrigMaxArg && x >= -kTrigMaxArg)) return nan_value();
  double y[2];
  const int n = rem_pio2(x, y);
  return kernel_tan(y[0], y[1], 1 - ((n & 1) << 1));
}

double atan(double x) {
  const std::int32_t hx = static_cast<std::int32_t>(high_word(x));
  const std::uint32_t ix = static_cast<std::uint32_t>(hx) & 0x7fffffffU;
  int id;
  if (ix >= 0x44100000U) {  // |x| >= 2^66
    if (x != x) return x;
    return hx > 0 ? kAtanHi[3] + kAtanLo[3] : -kAtanHi[3] - kAtanLo[3];
  }
  if (ix < 0x3fdc0000U) {             // |x| < 0.4375
    if (ix < 0x3e400000U) return x;   // |x| < 2^-27
    id = -1;
  } else {
    x = std::fabs(x);
    if (ix < 0x3ff30000U) {           // |x| < 1.1875
      if (ix < 0x3fe60000U) {         // 7/16 <= |x| < 11/16
        id = 0;
        x = (2.0 * x - 1.0) / (2.0 + x);
      } else {                        // 11/16 <= |x| < 19/16
        id = 1;
        x = (x - 1.0) / (x + 1.0);
      }
    } else {
      if (ix < 0x40038000U) {         // |x| < 2.4375
        id = 2;
        x = (x - 1.5) / (1.0 + 1.5 * x);
      } else {                        // 2.4375 <= |x| < 2^66
        id = 3;
        x = -1.0 / x;
      }
    }
  }
  const double z = x * x;
  const double w = z * z;
  const double s1 = z * (kAT[0] + w * (kAT[2] + w * (kAT[4] + w * (kAT[6] + w * (kAT[8] + w * kAT[10])))));
  const double s2 = w * (kAT[1] + w * (kAT[3] + w * (kAT[5] + w * (kAT[7] + w * kAT[9]))));
  if (id < 0) return x - x * (s1 + s2);
  const double r = kAtanHi[id] - ((x * (s1 + s2) - kAtanLo[id]) - x);
  return hx < 0 ? -r : r;
}

double atan2(double y, double x) {
  if (x != x || y != y) return x + y;
  if (x == 1.0) return atan(y);
  const bool ys = std::signbit(y);
  const bool xs = std::signbit(x);
  const int m = (ys ? 1 : 0) | (xs ? 2 : 0);
  if (y == 0.0) {
    switch (m) {
      case 0:
      case 1: return y;      // atan(+-0, +x) = +-0
      case 2: return kPi;    // atan(+0, -x) = pi
      default: return -kPi;  // atan(-0, -x) = -pi
    }
  }
  if (x == 0.0) return ys ? -kPio2 : kPio2;
  const bool xinf = std::fabs(x) == inf_value();
  const bool yinf = std::fabs(y) == inf_value();
  if (xinf) {
    if (yinf) {
      switch (m) {
        case 0: return kPio4;
        case 1: return -kPio4;
        case 2: return 3.0 * kPio4;
        default: return -3.0 * kPio4;
      }
    }
    switch (m) {
      case 0: return 0.0;
      case 1: return -0.0;
      case 2: return kPi;
      default: return -kPi;
    }
  }
  if (yinf) return ys ? -kPio2 : kPio2;
  const int ey = static_cast<int>((high_word(y) >> 20) & 0x7ff);
  const int ex = static_cast<int>((high_word(x) >> 20) & 0x7ff);
  const int k = ey - ex;
  double z;
  int mm = m;
  if (k > 60) {
    z = kPio2 + 0.5 * kPiLo;
    mm &= 1;
  } else if (xs && k < -60) {
    z = 0.0;
  } else {
    z = atan(std::fabs(y / x));
  }
  switch (mm) {
    case 0: return z;
    case 1: return -z;
    case 2: return kPi - (z - kPiLo);
    default: return (z - kPiLo) - kPi;
  }
}

double acos(double x) {
  if (x != x) return x;
  if (x > 1.0 || x < -1.0) return nan_value();
  if (x == 1.0) return 0.0;
  if (x == -1.0) return kPi;
  return atan2(sqrt((1.0 - x) * (1.0 + x)), x);
}

}  // namespace agro::detmath
