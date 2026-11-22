// detmath_vectors.cpp — prints detmath results for a fixed input set and their FNV-1a hash.
//
// The input set is generated with integer arithmetic only (counter RNG and exact grids), so
// it is identical everywhere. Each line is "<function> <x hex> [<y hex>] <result hex>".
// The last line is "FNV <hash>". The determinism check compares this hash across compilers,
// optimisation levels and operating systems with engine/native/golden/detmath_vectors.fnv;
// tools/medir_erro_detmath.py reads the lines and measures the error in ulp against
// arbitrary precision.
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "agro/detmath.hpp"
#include "agro/hash.hpp"
#include "agro/rng.hpp"

namespace {

agro::Fnv1a64 g_hash;

std::uint64_t bits(double x) {
  std::uint64_t u;
  std::memcpy(&u, &x, sizeof u);
  return u;
}

void emit1(const char* fn, double x, double y) {
  std::printf("%s %016" PRIx64 " %016" PRIx64 "\n", fn, bits(x), bits(y));
  g_hash.text(fn);
  g_hash.f64(x);
  g_hash.f64(y);
}
void emit2(const char* fn, double a, double b, double y) {
  std::printf("%s %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n", fn, bits(a), bits(b), bits(y));
  g_hash.text(fn);
  g_hash.f64(a);
  g_hash.f64(b);
  g_hash.f64(y);
}

// Uniform double in [lo, hi) from the counter RNG (exact arithmetic up to the final scaling).
double uniform(std::uint32_t stream_tag, std::uint32_t i, double lo, double hi) {
  const double u = agro::rng::draw_unit({0xdeadbeefULL, agro::rng::Stream::kMarketEvent, stream_tag, 0, i});
  return lo + (hi - lo) * u;
}

}  // namespace

int main() {
  using namespace agro;
  constexpr std::uint32_t kN = 4000;
  for (std::uint32_t i = 0; i < kN; ++i) {
    double x = uniform(1, i, -40.0, 40.0);
    emit1("exp", x, detmath::exp(x));
    x = uniform(2, i, -708.0, 709.0);
    emit1("exp", x, detmath::exp(x));
    x = uniform(3, i, 1e-6, 10.0);
    emit1("log", x, detmath::log(x));
    x = detmath::exp(uniform(4, i, -700.0, 700.0));
    emit1("log", x, detmath::log(x));
    x = uniform(5, i, 0.98, 1.02);
    emit1("log", x, detmath::log(x));
    x = uniform(6, i, -7.0, 7.0);
    emit1("sin", x, detmath::sin(x));
    emit1("cos", x, detmath::cos(x));
    emit1("tan", x, detmath::tan(x));
    x = uniform(7, i, -2000.0, 2000.0);
    emit1("sin", x, detmath::sin(x));
    emit1("cos", x, detmath::cos(x));
    x = uniform(8, i, -30.0, 30.0);
    emit1("atan", x, detmath::atan(x));
    const double a = uniform(9, i, -5.0, 5.0);
    const double b = uniform(10, i, -5.0, 5.0);
    emit2("atan2", a, b, detmath::atan2(a, b));
    x = uniform(11, i, -1.0, 1.0);
    emit1("acos", x, detmath::acos(x));
    const double base = uniform(12, i, 0.05, 3.0);
    const double expo = uniform(13, i, -6.0, 6.0);
    emit2("pow", base, expo, detmath::pow(base, expo));
  }
  std::printf("FNV %016" PRIx64 "\n", g_hash.value());
  return 0;
}
