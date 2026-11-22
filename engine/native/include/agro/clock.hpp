// clock.hpp — pacing of the game clock (24 game seconds per real second at 1x).
//
// The clock decides only HOW FAR the scheduler may run this frame. It never changes the size
// of a model step, so 1x, 8x and sleep give the same world. The real-time carry is pacing,
// not world state: it is not saved and restarts at zero after loading.
#pragma once

#include <cmath>
#include <cstdint>
#include <stdexcept>

#include "agro/scheduler.hpp"

namespace agro {

class Clock {
 public:
  static constexpr std::int64_t kRatio = 24;  // game seconds per real second at 1x

  explicit Clock(Tick start = 0) : now_(start) {}

  Tick now() const { return now_; }
  int multiplier() const { return multiplier_; }

  void set_multiplier(int m) {
    if (m != 1 && m != 2 && m != 4 && m != 8 && m != 16 && m != 32)
      throw std::invalid_argument("clock: multiplier must be 1, 2, 4, 8, 16 or 32");
    multiplier_ = m;
  }

  // Target tick after real_dt seconds of wall time, limited to max_advance game seconds per
  // frame (protects slow machines from the "spiral of death").
  Tick target_after(double real_dt, Tick max_advance) {
    if (real_dt < 0.0) real_dt = 0.0;
    carry_ += real_dt * static_cast<double>(kRatio * multiplier_);
    double whole = std::floor(carry_);
    if (whole > static_cast<double>(max_advance)) {
      whole = static_cast<double>(max_advance);
      carry_ = 0.0;  // drop the backlog instead of accumulating it
    } else {
      carry_ -= whole;
    }
    return now_ + static_cast<Tick>(whole);
  }

  // Sleep / skip: jump straight to a target (e.g. next 06:00). Pacing carry is discarded.
  Tick target_skip_to(Tick t) {
    carry_ = 0.0;
    return t < now_ ? now_ : t;
  }

  void reached(Tick t) {
    if (t < now_) throw std::logic_error("clock: time cannot go back");
    now_ = t;
  }

 private:
  Tick now_;
  int multiplier_ = 1;
  double carry_ = 0.0;
};

}  // namespace agro
