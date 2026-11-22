// test_determinism.cpp — metamorphic test of the time architecture with a toy world.
//
// The toy world is NOT an agronomic model (its numbers mean nothing); it only exercises the
// same mechanics the real core will use: event queue, daily step, player actions at
// arbitrary seconds, integer water ledger, arbiter with largest remainder, detmath in the
// fluxes and a counter-based RNG for one game event. The same world is driven by very
// different frame sequences (1x, 8x, irregular frames, sleep jumps, tiny event budgets and a
// save/load in the middle). The daily hash sequence must be identical in every run.
#include <cstdint>
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

namespace {

enum EventType : std::uint32_t { kDaily = 1, kIrrigate = 2, kStorm = 3 };

constexpr std::uint32_t kCells = 3;
constexpr std::uint64_t kSeed = 20261122;

struct ToyWorld {
  Ledger water{Quantity::kWater};
  AccountId rain{}, atm{}, deep{}, tap{}, init{};
  std::vector<AccountId> soil;  // one pool per cell
  Scheduler queue;
  Tick now = 0;
  std::vector<std::uint64_t> daily_hashes;

  ToyWorld() {
    rain = water.open("rain", AccountKind::kBoundary);
    atm = water.open("atmosphere", AccountKind::kBoundary);
    deep = water.open("deep_drainage", AccountKind::kBoundary);
    tap = water.open("irrigation_source", AccountKind::kBoundary);
    init = water.open("initial_condition", AccountKind::kBoundary);
    for (std::uint32_t c = 0; c < kCells; ++c) {
      soil.push_back(water.open("soil" + std::to_string(c), AccountKind::kPool, c));
      water.transfer(init, soil[c], 50000 + 1000 * c, Flux::kInitial);
    }
    water.close_day();
    queue.push(kSecondsPerDay, Phase::kDayBoundary, kDaily);
    // Player plan: irrigations at odd seconds of the day, two at the same second.
    const Tick plan[][3] = {{3 * 3600 + 17, 0, 7000}, {9 * 3600 + 1, 1, 3000}, {86400 + 7 * 3600, 2, 5000},
                            {86400 + 7 * 3600, 0, 1234}, {3 * 86400 + 86399, 1, 999}, {6 * 86400 + 5, 2, 4321}};
    for (const auto& p : plan) queue.push(p[0], Phase::kPlayerAction, kIrrigate, {p[1], p[2], 0, 0});
  }

  void handle(const Event& e) {
    now = e.tick;
    if (e.type == kIrrigate) {
      water.transfer(tap, soil[static_cast<std::size_t>(e.arg[0])], e.arg[1], Flux::kIrrigation);
    } else if (e.type == kStorm) {
      water.transfer(rain, soil[static_cast<std::size_t>(e.arg[0])], e.arg[1], Flux::kRain);
    } else if (e.type == kDaily) {
      daily_step(e.tick / kSecondsPerDay - 1);
      queue.push(e.tick + kSecondsPerDay, Phase::kDayBoundary, kDaily);
    }
  }

  void daily_step(std::int64_t day) {
    // Deterministic "climate" forcing from detmath.
    const double season = 0.5 + 0.5 * detmath::sin(2.0 * 3.141592653589793 * static_cast<double>(day) / 365.0);
    for (std::uint32_t c = 0; c < kCells; ++c) {
      const double s = static_cast<double>(water.balance(soil[c]));
      // Two "plants" compete for the evaporable water of the cell through the arbiter.
      const double demand_total = s * (1.0 - detmath::exp(-0.05 - 0.03 * season));
      const std::vector<std::int64_t> claims{to_units(demand_total * 0.6), to_units(demand_total * 0.7)};
      const std::int64_t supply = to_units(s * 0.08);
      for (std::int64_t share : allocate_largest_remainder(supply, claims))
        water.transfer(soil[c], atm, share, Flux::kTranspiration);
      const double after = static_cast<double>(water.balance(soil[c]));
      const double excess = after - 52000.0;
      if (excess > 0.0) water.transfer(soil[c], deep, to_units(excess * detmath::pow(0.5, 1.5)), Flux::kDrainage);
    }
    // Game event: a storm with probability 30% per day (open-mode style), scheduled at 15:00.
    if (rng::draw_bernoulli_ppm({kSeed, rng::Stream::kWeatherGenerator, day, 0, 0}, 300000)) {
      const auto r = rng::draw_u64({kSeed, rng::Stream::kWeatherGenerator, day, 0, 1});
      queue.push((day + 1) * kSecondsPerDay + 15 * 3600, Phase::kWorldEvent, kStorm,
                 {static_cast<std::int64_t>(r % kCells), static_cast<std::int64_t>(1000 + r % 9000), 0, 0});
    }
    if (!water.close_day().empty()) throw std::logic_error("toy world: ledger does not close");
    Fnv1a64 h;
    h.i64(day);
    water.hash_into(h);
    daily_hashes.push_back(h.value());
  }

  // Save = serialise the world state; load = rebuild a fresh world from it.
  struct Save {
    std::vector<std::int64_t> balances;
    std::vector<Event> events;
    std::uint64_t next_seq;
    Tick now;
    std::vector<std::uint64_t> hashes;
  };
  Save save() const {
    Save s;
    for (AccountId a = 0; a < water.size(); ++a) s.balances.push_back(water.balance(a));
    s.events = queue.snapshot();
    s.next_seq = queue.next_seq();
    s.now = now;
    s.hashes = daily_hashes;
    return s;
  }
  static ToyWorld load(const Save& s) {
    ToyWorld w;  // same accounts, opened in the same order
    // Rebuild every balance exactly through the initial-condition account (sum stays zero).
    Ledger rebuilt(Quantity::kWater);
    for (AccountId a = 0; a < w.water.size(); ++a) rebuilt.open(w.water.account(a).name, w.water.account(a).kind, w.water.account(a).cell);
    for (AccountId a = 0; a < w.water.size(); ++a) {
      if (a != w.init && s.balances[a] > 0) rebuilt.transfer(w.init, a, s.balances[a], Flux::kInitial);
    }
    for (AccountId a = 0; a < w.water.size(); ++a) {
      if (a == w.init) continue;
      if (s.balances[a] < 0) rebuilt.transfer(a, w.init, -s.balances[a], Flux::kInitial);
    }
    rebuilt.close_day();
    w.water = rebuilt;
    w.queue.restore(s.events, s.next_seq);
    w.now = s.now;
    w.daily_hashes = s.hashes;
    return w;
  }
};

constexpr int kDays = 10;
constexpr Tick kEnd = kDays * kSecondsPerDay + 1;

std::vector<std::uint64_t> run_frames(int multiplier, double frame_dt, std::size_t budget) {
  ToyWorld w;
  Clock clock;
  clock.set_multiplier(multiplier);
  while (clock.now() < kEnd) {
    const Tick target = std::min(kEnd, clock.target_after(frame_dt, 6 * 3600));
    clock.reached(run_until(w.queue, clock.now(), target, budget, [&](const Event& e) { w.handle(e); }));
  }
  return w.daily_hashes;
}

std::vector<std::uint64_t> run_irregular_with_sleep_and_save() {
  ToyWorld w;
  Clock clock;
  std::uint32_t frame = 0;
  bool saved = false;
  while (clock.now() < kEnd) {
    const auto r = rng::draw_u64({99, rng::Stream::kMarketEvent, frame, 0, 0});
    Tick target;
    if (r % 97 == 0) {
      target = clock.target_skip_to(clock.now() + 10 * 3600);  // sleep: jump 10 h at once
    } else {
      clock.set_multiplier(1 << (r % 4));
      const double dt = 0.002 + static_cast<double>(r % 1000) / 4000.0;  // 2 ms to 252 ms frames
      target = clock.target_after(dt, 6 * 3600);
    }
    target = std::min(kEnd, target);
    clock.reached(run_until(w.queue, clock.now(), target, 1 + r % 3, [&](const Event& e) { w.handle(e); }));
    if (!saved && clock.now() > 4 * kSecondsPerDay + 12345) {
      const auto s = w.save();
      w = ToyWorld::load(s);  // continue from the loaded copy
      clock = Clock(s.now > clock.now() ? s.now : clock.now());
      saved = true;
    }
    ++frame;
  }
  return w.daily_hashes;
}

}  // namespace

TEST("determinismo: 1x, 8x, quadros irregulares, dormir, orçamento mínimo e save/load dão o mesmo hash diário") {
  const auto ref = run_frames(1, 1.0 / 60.0, 1000);
  CHECK_EQ(ref.size(), static_cast<std::size_t>(kDays));
  CHECK(run_frames(8, 1.0 / 60.0, 1000) == ref);
  CHECK(run_frames(32, 1.0 / 30.0, 1) == ref);
  CHECK(run_frames(2, 1.0 / 144.0, 2) == ref);
  CHECK(run_irregular_with_sleep_and_save() == ref);
}
