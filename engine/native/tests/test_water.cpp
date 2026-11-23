// test_water.cpp — one-layer soil-water balance: published examples, exact books under random
// actions, and the same daily hash at any speed and after save/load.
//
// Published references: NRCS NEH-630 chapter 10 (July 2004), Examples 10-1 and 10-2
// (registry id NRCS-2004-NEH630-CH10); FAO-56 Example 37 (FAO-1998-FAO56). Every other number
// in this file is a synthetic test value with no agronomic meaning.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include "agro/clock.hpp"
#include "agro/rng.hpp"
#include "agro/scheduler.hpp"
#include "agro/water.hpp"
#include "mini_test.hpp"

using namespace agro;
using namespace agro::water;

namespace {

CellSoil synthetic_soil(double area = 1.0) { return CellSoil{area, 0.25, 0.30, 0.15, 0.06, 80.0}; }

}  // namespace

TEST("água: Curve Number reproduz os Exemplos 10-1 e 10-2 do NEH-630 (P = 4,3 pol)") {
  const double p = 4.3 * 25.4;
  CHECK(std::fabs(runoff_curve_number_mm(p, 74.0) / 25.4 - 1.82) <= 0.005);
  CHECK(std::fabs(runoff_curve_number_mm(p, 55.0) / 25.4 - 0.65) <= 0.005);
  CHECK(std::fabs(runoff_curve_number_mm(p, 88.0) / 25.4 - 3.01) <= 0.005);
  // Abaixo da abstração inicial (Ia = 0,2 S) não há escoamento; com CN = 100 tudo escoa.
  const double s74 = 25400.0 / 74.0 - 254.0;
  CHECK(runoff_curve_number_mm(0.2 * s74, 74.0) == 0.0);
  CHECK(runoff_curve_number_mm(0.2 * s74 + 1.0, 74.0) > 0.0);
  CHECK(std::fabs(runoff_curve_number_mm(37.0, 100.0) - 37.0) < 1e-12);
  CHECK_THROWS(runoff_curve_number_mm(10.0, 0.0));
  CHECK_THROWS(runoff_curve_number_mm(-1.0, 70.0));
}

TEST("água: FAO-56 Exemplo 37 (tomate, TAW 160 mm, p 0,40, ETc 6 mm/dia, Dr inicial 55 mm)") {
  // Solo siltoso, theta_CC 0,32, theta_PMP 0,12, Zr 0,8 m. Dr = W_CC - W.
  SingleLayer w({CellSoil{1.0, 0.8, 0.32, 0.12, 0.06, 75.0}}, {0.32 - 55.0 / 800.0});
  const double ks[] = {1.00, 1.00, 0.97, 0.91, 0.85, 0.80, 0.75, 0.70, 0.66, 0.62};
  const double dr_end[] = {61.0, 67.0, 72.8, 78.3, 83.4, 88.2, 92.6, 96.9, 100.8, 104.5};
  for (int day = 0; day < 10; ++day) {
    const auto r = w.step_day({0.0}, {{Claim{Sink::kTranspiration, 6.0, 0.40}}});
    const double dr = static_cast<double>(w.field_capacity_ml(0) - w.storage(0)) / 1000.0;
    if (std::fabs(r[0].reduction[0] - ks[day]) > 0.005) mini_test::fail(__FILE__, __LINE__, "Ks do dia " + std::to_string(day + 1));
    // Meia unidade da última casa impressa mais 1 mL (0,001 mm em 1 m²) da contagem inteira:
    // no dia 7 o valor exato é 92,650 mm e o livro imprime 92,6.
    if (std::fabs(dr - dr_end[day]) > 0.051) mini_test::fail(__FILE__, __LINE__, "Dr do dia " + std::to_string(day + 1) + " = " + std::to_string(dr));
  }
}

TEST("água: chuva acima da capacidade de campo drena no mesmo dia (FAO-56 Eq. 88)") {
  CellSoil s = synthetic_soil();
  s.curve_number = 40.0;  // Ia = 76,2 mm: 30 mm não escoam
  SingleLayer w({s}, {s.theta_fc});
  const auto r = w.step_day({30.0}, {{Claim{Sink::kTranspiration, 4.0, 0.5}}});
  CHECK_EQ(r[0].runoff, std::int64_t{0});
  CHECK_EQ(r[0].transpiration, std::int64_t{4000});
  CHECK_EQ(r[0].drainage, std::int64_t{26000});
  CHECK_EQ(w.storage(0), w.field_capacity_ml(0));
  CHECK_EQ(w.books().balance(w.rain_account()), std::int64_t{-30000});
  CHECK_EQ(w.books().balance(w.deep_account()), std::int64_t{26000});
}

TEST("água: rega fica em \"aplicada hoje\" até o passo diário e não escoa") {
  CellSoil s = synthetic_soil(0.25);
  s.curve_number = 100.0;  // toda chuva escoaria; a água aplicada não
  SingleLayer w({s}, {0.20});
  const std::int64_t before = w.storage(0);
  w.apply_water(0, 5000, 300, 200);
  CHECK_EQ(w.storage(0), before);
  CHECK_EQ(w.applied_today(0), std::int64_t{5000});
  CHECK_EQ(w.books().balance(w.atmosphere_account()), std::int64_t{300});
  CHECK_EQ(w.books().balance(w.runoff_account()), std::int64_t{200});
  const auto r = w.step_day({0.0}, {{}});
  CHECK_EQ(r[0].applied, std::int64_t{5000});
  CHECK_EQ(w.applied_today(0), std::int64_t{0});
  CHECK_EQ(w.storage(0), before + 5000);
  CHECK_THROWS(w.apply_water(0, -1));
}

TEST("água: evaporação seca até o seco ao ar; transpiração para no ponto de murcha") {
  const CellSoil s = synthetic_soil();
  SingleLayer w({s, s}, {s.theta_fc, s.theta_fc});
  std::int64_t last0 = w.storage(0), last1 = w.storage(1);
  for (int day = 0; day < 400; ++day) {
    w.step_day({0.0, 0.0}, {{Claim{Sink::kEvaporation, 5.0, 0.2}}, {Claim{Sink::kTranspiration, 5.0, 0.5}}});
    CHECK(w.storage(0) <= last0 && w.storage(0) >= w.air_dry_ml(0));
    CHECK(w.storage(1) <= last1 && w.storage(1) >= w.wilting_point_ml(1));
    last0 = w.storage(0);
    last1 = w.storage(1);
  }
  CHECK(w.storage(0) - w.air_dry_ml(0) < 1000);    // perto do seco ao ar
  CHECK(w.storage(1) - w.wilting_point_ml(1) < 1000);  // perto do ponto de murcha
}

// ------------------------------------------------------------- random actions and speeds
namespace {

enum EventType : std::uint32_t { kDaily = 1, kApply = 2 };
constexpr std::uint32_t kCells = 4;
constexpr int kDays = 40;
constexpr Tick kEnd = kDays * kSecondsPerDay + 1;

std::uint64_t draw(std::int64_t day, std::uint32_t cell, std::uint32_t index) {
  return rng::draw_u64({20261123, rng::Stream::kMarketEvent, day, cell, index});
}
double unit(std::int64_t day, std::uint32_t cell, std::uint32_t index) {
  return static_cast<double>(draw(day, cell, index) >> 11) * (1.0 / 9007199254740992.0);
}

struct Farm {
  SingleLayer water;
  Scheduler queue;
  Tick now = 0;
  std::vector<std::uint64_t> daily_hashes;

  static std::vector<CellSoil> soils() {
    std::vector<CellSoil> v;
    for (std::uint32_t c = 0; c < kCells; ++c)
      v.push_back(CellSoil{0.25 + 0.5 * c, 0.15 + 0.05 * c, 0.28 + 0.02 * c, 0.12 + 0.01 * c, 0.05, 60.0 + 10.0 * c});
    return v;
  }

  Farm() : water(soils(), {0.20, 0.25, 0.18, 0.32}) {
    queue.push(kSecondsPerDay, Phase::kDayBoundary, kDaily);
    // 300 actions at random seconds, some at exactly midnight, with gross and lost volumes.
    for (std::uint32_t k = 0; k < 300; ++k) {
      const std::uint64_t r = draw(-1, 0, k);
      Tick t = static_cast<Tick>(r % static_cast<std::uint64_t>(kDays * kSecondsPerDay));
      if (k % 25 == 0) t -= t % kSecondsPerDay;  // exactly 00:00: belongs to the new day
      const std::int64_t to_soil = static_cast<std::int64_t>((r >> 20) % 60000);
      const std::int64_t to_air = static_cast<std::int64_t>((r >> 40) % 3000);
      const std::int64_t to_runoff = (k % 3 == 0) ? static_cast<std::int64_t>((r >> 8) % 2000) : 0;
      queue.push(t, Phase::kPlayerAction, kApply, {static_cast<std::int64_t>(r % kCells), to_soil, to_air, to_runoff});
    }
  }

  void handle(const Event& e) {
    now = e.tick;
    if (e.type == kApply) {
      water.apply_water(static_cast<std::uint32_t>(e.arg[0]), e.arg[1], e.arg[2], e.arg[3]);
    } else if (e.type == kDaily) {
      daily_step(e.tick / kSecondsPerDay - 1);
      queue.push(e.tick + kSecondsPerDay, Phase::kDayBoundary, kDaily);
    }
  }

  void daily_step(std::int64_t day) {
    std::vector<double> rain(kCells, 0.0);
    std::vector<std::vector<Claim>> claims(kCells);
    for (std::uint32_t c = 0; c < kCells; ++c) {
      if (unit(day, c, 0) < 0.4) rain[c] = 150.0 * unit(day, c, 1) * unit(day, c, 2);  // up to 150 mm
      const std::uint32_t n = static_cast<std::uint32_t>(draw(day, c, 3) % 4);
      for (std::uint32_t i = 0; i < n; ++i)
        claims[c].push_back(Claim{(draw(day, c, 10 + i) & 1) ? Sink::kEvaporation : Sink::kTranspiration,
                                  12.0 * unit(day, c, 20 + i), 0.9 * unit(day, c, 30 + i)});
    }
    std::vector<std::int64_t> applied(kCells);
    for (std::uint32_t c = 0; c < kCells; ++c) applied[c] = water.applied_today(c);
    const auto r = water.step_day(rain, claims);  // throws if the books do not close
    for (std::uint32_t c = 0; c < kCells; ++c) {
      const CellDay& d = r[c];
      const std::int64_t delta = d.storage_end - d.storage_start;
      const std::int64_t net = (d.rain - d.runoff) + d.applied - d.evaporation - d.transpiration - d.drainage;
      if (delta != net) throw std::logic_error("cell balance does not close");
      if (d.applied != applied[c]) throw std::logic_error("applied today lost");
      if (d.storage_end > water.field_capacity_ml(c) || d.storage_end < water.air_dry_ml(c))
        throw std::logic_error("storage outside [air dry, field capacity]");
      const std::int64_t w1 = d.storage_start + d.rain - d.runoff + d.applied;
      const std::int64_t above_wp = w1 > water.wilting_point_ml(c) ? w1 - water.wilting_point_ml(c) : 0;
      if (d.transpiration > above_wp) throw std::logic_error("transpiration below the wilting point");
    }
    Fnv1a64 h;
    h.i64(day);
    water.hash_into(h);
    daily_hashes.push_back(h.value());
  }

  struct Save {
    std::vector<std::int64_t> balances;
    std::vector<Event> events;
    std::uint64_t next_seq;
    Tick now;
    std::vector<std::uint64_t> hashes;
  };
  Save save() const { return Save{water.save(), queue.snapshot(), queue.next_seq(), now, daily_hashes}; }
  static Farm load(const Save& s) {
    Farm f;
    f.water.load(s.balances);
    f.queue.restore(s.events, s.next_seq);
    f.now = s.now;
    f.daily_hashes = s.hashes;
    return f;
  }
};

std::vector<std::uint64_t> run(int multiplier, double frame_dt, std::size_t budget, bool irregular) {
  Farm f;
  Clock clock;
  clock.set_multiplier(multiplier);
  bool saved = false;
  std::uint32_t frame = 0;
  while (clock.now() < kEnd) {
    Tick target;
    const std::uint64_t r = draw(-2, 0, frame);
    if (irregular && r % 89 == 0) {
      target = clock.target_skip_to(clock.now() + 9 * kSecondsPerHour);  // dormir
    } else if (irregular && r % 113 == 0 && !f.queue.empty()) {
      target = clock.target_skip_to(f.queue.peek().tick);  // pular até o evento
    } else {
      if (irregular) clock.set_multiplier(1 << (r % 4));
      target = clock.target_after(irregular ? 0.002 + static_cast<double>(r % 1000) / 2000.0 : frame_dt, 12 * kSecondsPerHour);
    }
    target = std::min(kEnd, target);
    clock.reached(run_until(f.queue, clock.now(), target, irregular ? 1 + r % 3 : budget, [&](const Event& e) { f.handle(e); }));
    if (irregular && !saved && clock.now() > 17 * kSecondsPerDay + 4321) {
      const auto s = f.save();
      f = Farm::load(s);
      saved = true;
    }
    ++frame;
  }
  return f.daily_hashes;
}

}  // namespace

TEST("água: 300 ações aleatórias em 40 dias fecham o ledger todo dia; 1x = 8x = irregular com dormir e save/load") {
  const auto ref = run(1, 1.0 / 30.0, 100000, false);
  CHECK_EQ(ref.size(), static_cast<std::size_t>(kDays));
  CHECK(run(8, 1.0 / 60.0, 100000, false) == ref);
  CHECK(run(4, 1.0 / 20.0, 1, false) == ref);
  CHECK(run(1, 0.0, 0, true) == ref);
}

TEST("ledger: save e carga recusam livros que criariam ou destruiriam água") {
  SingleLayer w({synthetic_soil()}, {0.2});
  auto b = w.save();
  b[0] -= 1;  // soma deixa de ser zero
  CHECK_THROWS(w.load(b));
  b = w.save();
  b.pop_back();
  CHECK_THROWS(w.load(b));
}

TEST("água: o cenário aleatório exercita escoamento, drenagem, perdas e as duas demandas") {
  Farm f;
  run_until(f.queue, 0, kEnd, 1000000, [&](const Event& e) { f.handle(e); });
  const Ledger& b = f.water.books();
  CHECK(b.balance(f.water.runoff_account()) > 0);
  CHECK(b.balance(f.water.deep_account()) > 0);
  CHECK(b.balance(f.water.atmosphere_account()) > 0);
  CHECK(b.balance(f.water.rain_account()) < 0);
  CHECK(b.balance(f.water.source_account()) < 0);
  CHECK_EQ(f.daily_hashes.size(), static_cast<std::size_t>(kDays));
}

TEST("água: hash do último dia do cenário aleatório é o mesmo em toda plataforma (Linux = Windows)") {
  Farm f;
  run_until(f.queue, 0, kEnd, 1000000, [&](const Event& e) { f.handle(e); });
  const std::uint64_t expected = 0xf41d005c89fd58e7ULL;  // gerado com g++ -O2 no Linux
  if (f.daily_hashes.back() != expected)
    std::printf("  hash do dia %d: 0x%016llx\n", kDays, static_cast<unsigned long long>(f.daily_hashes.back()));
  CHECK(f.daily_hashes.back() == expected);
}
