// scheduler.hpp — one deterministic event queue for everything that changes the world.
//
// The world changes only when an event is processed: the daily model step at 00:00, the
// hourly step (phase 2+), scheduled world events (lab report arrives, a synthetic storm
// starts) and player actions. Each event has a unique key (tick, phase, seq):
//   tick   game second at which it happens (int64)
//   phase  fixed order of event classes inside the same second
//   seq    insertion counter, saved with the world
// Because keys are unique, the processing order is a total order that does not depend on
// the heap implementation, on the frame rate, on the speed multiplier, on sleep mode, or on
// where a save was made. Speed only changes how far the clock is allowed to run per frame.
#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <queue>
#include <stdexcept>
#include <vector>

namespace agro {

using Tick = std::int64_t;  // game seconds since the start of the world

inline constexpr Tick kSecondsPerHour = 3600;
inline constexpr Tick kSecondsPerDay = 86400;

enum class Phase : std::uint16_t {
  kDayBoundary = 0,   // daily step of the agronomic models (00:00)
  kHourBoundary = 1,  // hourly processes
  kWorldEvent = 2,    // scheduled world/game events
  kPlayerAction = 3,  // act(verb, target, input), in input order
};

struct Event {
  Tick tick;
  Phase phase;
  std::uint64_t seq;
  std::uint32_t type;  // meaning defined by the dispatcher
  std::array<std::int64_t, 4> arg;
};

inline bool event_before(const Event& a, const Event& b) {
  if (a.tick != b.tick) return a.tick < b.tick;
  if (a.phase != b.phase) return a.phase < b.phase;
  return a.seq < b.seq;
}

class Scheduler {
 public:
  void push(Tick tick, Phase phase, std::uint32_t type, std::array<std::int64_t, 4> arg = {0, 0, 0, 0}) {
    heap_.push(Event{tick, phase, next_seq_++, type, arg});
  }
  bool empty() const { return heap_.empty(); }
  const Event& peek() const { return heap_.top(); }
  Event pop() {
    Event e = heap_.top();
    heap_.pop();
    return e;
  }
  std::size_t size() const { return heap_.size(); }
  std::uint64_t next_seq() const { return next_seq_; }

  // Snapshot for saving: events in processing order, plus the sequence counter.
  std::vector<Event> snapshot() const {
    auto copy = heap_;
    std::vector<Event> out;
    out.reserve(copy.size());
    while (!copy.empty()) {
      out.push_back(copy.top());
      copy.pop();
    }
    return out;
  }
  void restore(const std::vector<Event>& events, std::uint64_t next_seq) {
    heap_ = Heap{};
    for (const Event& e : events) heap_.push(e);
    next_seq_ = next_seq;
  }

 private:
  struct After {
    bool operator()(const Event& a, const Event& b) const { return event_before(b, a); }
  };
  using Heap = std::priority_queue<Event, std::vector<Event>, After>;
  Heap heap_;
  std::uint64_t next_seq_ = 0;
};

// Processes every event with tick <= target, at most max_events of them, calling handle(e).
// The handler may push new events (they are processed in the same call if due).
// Returns the tick the world reached: target if all due events ran, otherwise the tick of
// the last processed event (the game slows down; the step never changes).
inline Tick run_until(Scheduler& s, Tick now, Tick target, std::size_t max_events,
                      const std::function<void(const Event&)>& handle) {
  if (target < now) throw std::invalid_argument("run_until: target in the past");
  std::size_t done = 0;
  while (!s.empty() && s.peek().tick <= target) {
    if (done == max_events) return now;  // budget exhausted: stop at the last processed tick
    Event e = s.pop();
    if (e.tick < now) throw std::logic_error("run_until: event scheduled in the past");
    now = e.tick;
    handle(e);
    ++done;
  }
  return target;
}

}  // namespace agro
