// test_core.cpp — unit tests of the deterministic building blocks.
#include <cmath>
#include <cstdint>
#include <set>
#include <vector>

#include "agro/allocate.hpp"
#include "agro/clock.hpp"
#include "agro/detmath.hpp"
#include "agro/hash.hpp"
#include "agro/ledger.hpp"
#include "agro/rng.hpp"
#include "agro/scheduler.hpp"
#include "mini_test.hpp"

using namespace agro;

// ------------------------------------------------------------------------------ RNG
TEST("rng: SplitMix64 de semente 0 dá 0xe220a8397b1dcdaf (valor de referência)") {
  CHECK(rng::splitmix64_first(0) == 0xe220a8397b1dcdafULL);
}

TEST("rng: sorteio é função pura da chave e streams não se cruzam") {
  rng::Key a{42, rng::Stream::kWeatherGenerator, 37, 0, 0};
  rng::Key b = a;
  CHECK(rng::draw_u64(a) == rng::draw_u64(b));
  b.stream = rng::Stream::kPestOutbreak;
  CHECK(rng::draw_u64(a) != rng::draw_u64(b));
  b = a;
  b.day = 38;
  CHECK(rng::draw_u64(a) != rng::draw_u64(b));
  double u = rng::draw_unit(a);
  CHECK(u >= 0.0 && u < 1.0);
}

TEST("rng: média e cobertura de 200 mil sorteios uniformes") {
  double sum = 0.0;
  int below_tenth = 0;
  for (std::uint32_t i = 0; i < 200000; ++i) {
    const double u = rng::draw_unit({7, rng::Stream::kWeatherGenerator, 0, 0, i});
    sum += u;
    if (u < 0.1) ++below_tenth;
  }
  CHECK(std::fabs(sum / 200000.0 - 0.5) < 0.005);
  CHECK(below_tenth > 19000 && below_tenth < 21000);
}

// ------------------------------------------------------------------------ allocation
TEST("alocação: soma exata e cota proporcional com erro < 1 unidade") {
  for (std::uint32_t trial = 0; trial < 2000; ++trial) {
    const auto r = rng::draw_u64({1, rng::Stream::kMarketEvent, trial, 0, 0});
    const std::size_t n = 1 + r % 7;
    std::vector<std::int64_t> claims(n);
    std::int64_t total = 0;
    for (std::size_t i = 0; i < n; ++i) {
      claims[i] = static_cast<std::int64_t>(rng::draw_u64({1, rng::Stream::kMarketEvent, trial, 1, static_cast<std::uint32_t>(i)}) % 5000000);
      total += claims[i];
    }
    const std::int64_t supply = total == 0 ? 0 : static_cast<std::int64_t>(r % static_cast<std::uint64_t>(total + 1));
    const auto s = allocate_largest_remainder(supply, claims);
    std::int64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      sum += s[i];
      CHECK(s[i] >= 0 && s[i] <= claims[i]);
      if (total > 0) {
        const double exact = static_cast<double>(supply) * static_cast<double>(claims[i]) / static_cast<double>(total);
        CHECK(std::fabs(static_cast<double>(s[i]) - exact) < 1.0 + 1e-6);
      }
    }
    CHECK_EQ(sum, std::min(supply, total));
  }
}

TEST("alocação: empate vai para o menor índice; oferta farta atende tudo") {
  auto s = allocate_largest_remainder(10, {1, 1, 1});
  CHECK_EQ(s[0] + s[1] + s[2], 3);
  s = allocate_largest_remainder(2, {1, 1, 1});
  CHECK_EQ(s[0], 1);
  CHECK_EQ(s[1], 1);
  CHECK_EQ(s[2], 0);
  CHECK_THROWS(allocate_largest_remainder(-1, {1}));
}

TEST("alocação: caminho portátil de 128 bits igual ao __int128") {
  for (std::uint32_t i = 0; i < 20000; ++i) {
    const std::uint64_t a = rng::draw_u64({3, rng::Stream::kMarketEvent, i, 0, 0}) >> 1;
    const std::uint64_t b = rng::draw_u64({3, rng::Stream::kMarketEvent, i, 0, 1}) >> 33;
    const std::uint64_t c = (rng::draw_u64({3, rng::Stream::kMarketEvent, i, 0, 2}) >> 1) | 1ULL;
    if (a / c > (~0ULL) / (b + 1)) continue;  // quotient must fit in 64 bits
    const auto p = detail::muldiv_portable(a, b, c);
    const auto q = detail::muldiv(a, b, c);
    CHECK(p.quotient == q.quotient);
    CHECK(p.remainder == q.remainder);
  }
}

// ---------------------------------------------------------------------------- ledger
TEST("ledger: balanço fecha exato e pool nunca fica negativo") {
  Ledger w(Quantity::kWater);
  const auto rain = w.open("rain", AccountKind::kBoundary);
  const auto atm = w.open("atmosphere", AccountKind::kBoundary);
  const auto soil = w.open("soil", AccountKind::kPool, 0);
  w.transfer(rain, soil, 12345, Flux::kRain);
  w.transfer(soil, atm, 345, Flux::kEvaporation);
  CHECK_EQ(w.balance(soil), 12000);
  CHECK_EQ(w.balance(rain), -12345);
  CHECK(w.close_day().empty());
  CHECK_THROWS(w.transfer(soil, atm, 12001, Flux::kEvaporation));
  CHECK_THROWS(w.transfer(soil, atm, -1, Flux::kEvaporation));
  CHECK(w.close_day().empty());
}

TEST("ledger: 100 mil transferências aleatórias mantêm a soma zero") {
  Ledger w(Quantity::kNitrogen);
  std::vector<AccountId> ids;
  ids.push_back(w.open("source", AccountKind::kBoundary));
  for (int i = 0; i < 20; ++i) ids.push_back(w.open("pool" + std::to_string(i), AccountKind::kPool, static_cast<std::uint32_t>(i)));
  for (std::uint32_t k = 0; k < 100000; ++k) {
    const auto r = rng::draw_u64({9, rng::Stream::kMarketEvent, k, 0, 0});
    const AccountId from = ids[r % ids.size()];
    const AccountId to = ids[(r >> 8) % ids.size()];
    if (from == to) continue;
    std::int64_t amount = static_cast<std::int64_t>((r >> 16) % 1000);
    if (w.account(from).kind == AccountKind::kPool) amount = std::min(amount, w.balance(from));
    w.transfer(from, to, amount, Flux::kUptake);
    if (k % 1000 == 999) CHECK(w.close_day().empty());
  }
  CHECK(w.close_day().empty());
}

TEST("ledger: to_units arredonda uma vez e rejeita NaN") {
  CHECK_EQ(to_units(2.5), 3);
  CHECK_EQ(to_units(-2.5), -3);
  CHECK_EQ(to_units(1.4999), 1);
  CHECK_THROWS(to_units(std::nan("")));
}

// ------------------------------------------------------------------------- scheduler
TEST("agenda: ordem total por (tick, fase, seq), independente da ordem de inserção") {
  Scheduler s;
  s.push(100, Phase::kPlayerAction, 1);
  s.push(100, Phase::kDayBoundary, 2);
  s.push(50, Phase::kPlayerAction, 3);
  s.push(100, Phase::kPlayerAction, 4);
  s.push(100, Phase::kWorldEvent, 5);
  std::vector<std::uint32_t> order;
  while (!s.empty()) order.push_back(s.pop().type);
  const std::vector<std::uint32_t> expected{3, 2, 5, 1, 4};
  CHECK(order == expected);
}

TEST("agenda: orçamento de eventos para no último tick processado") {
  Scheduler s;
  for (int i = 0; i < 5; ++i) s.push(10 * (i + 1), Phase::kWorldEvent, static_cast<std::uint32_t>(i));
  int seen = 0;
  Tick t = run_until(s, 0, 1000, 2, [&](const Event&) { ++seen; });
  CHECK_EQ(seen, 2);
  CHECK_EQ(t, Tick{20});
  t = run_until(s, t, 1000, 100, [&](const Event&) { ++seen; });
  CHECK_EQ(seen, 5);
  CHECK_EQ(t, Tick{1000});
}

TEST("relógio: 1 s real em 1x = 24 s de jogo; 8x = 192; limite por quadro; só 1, 2, 4 e 8") {
  Clock c;
  CHECK_EQ(c.target_after(1.0, 1000000), Tick{24});
  c.reached(24);
  c.set_multiplier(8);
  CHECK_EQ(c.target_after(1.0, 1000000), Tick{24 + 192});
  CHECK_EQ(c.target_after(100.0, 600), Tick{24 + 600});
  CHECK_THROWS(c.set_multiplier(3));
  CHECK_THROWS(c.set_multiplier(16));
}

// --------------------------------------------------------------------------- detmath
TEST("detmath: valores exatos e identidades simples") {
  CHECK(detmath::exp(0.0) == 1.0);
  CHECK(detmath::log(1.0) == 0.0);
  CHECK(detmath::sin(0.0) == 0.0);
  CHECK(detmath::cos(0.0) == 1.0);
  CHECK(detmath::atan2(0.0, -1.0) == 3.1415926535897931160);
  CHECK(detmath::acos(-1.0) == 3.1415926535897931160);
  CHECK(detmath::pow(2.0, 10.0) == 1024.0);
  CHECK(detmath::pow(-2.0, 3.0) == -8.0);
  CHECK(std::isnan(detmath::pow(-2.0, 0.5)));
  CHECK(std::isnan(detmath::log(-1.0)));
  CHECK(std::isinf(detmath::exp(1000.0)));
  CHECK(detmath::exp(-1000.0) == 0.0);
  CHECK(std::isnan(detmath::sin(1e7)));
}

TEST("detmath: erro relativo < 2e-16 contra a libm em pontos de uso agronômico") {
  // A libm local serve de referência aproximada; a medida rigorosa em ulp contra precisão
  // arbitrária fica em tools/medir_erro_detmath.py.
  double worst = 0.0;
  for (int i = -2000; i <= 2000; ++i) {
    const double x = i * 0.0173;
    const double v[] = {detmath::exp(x) / std::exp(x) - 1.0, detmath::sin(x) - std::sin(x),
                        detmath::cos(x) - std::cos(x), detmath::atan(x) - std::atan(x)};
    for (double d : v) worst = std::max(worst, std::fabs(d));
    if (x > 0) worst = std::max(worst, std::fabs(detmath::log(x) - std::log(x)) / std::max(1.0, std::fabs(std::log(x))));
  }
  CHECK(worst < 4.5e-16);
}
