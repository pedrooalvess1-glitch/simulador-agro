// rng.hpp — counter-based random numbers for GAME EVENTS ONLY.
//
// Process models (water, soil, plant, weeds) are deterministic expected-value models and
// never draw random numbers. Randomness is reserved for game events: synthetic weather in
// the open mode, pest outbreaks, breakdowns. Each draw is a pure function of
// (world seed, stream, day, cell, index): there is no generator state to save, a stream
// never shifts another stream's sequence, and skipping days (sleep, fast-forward) cannot
// diverge. Integer arithmetic only, so the result is identical on every platform.
//
// Mixer: the SplitMix64 finaliser (Steele, Lea & Flood 2014, "Fast splittable pseudorandom
// number generators", OOPSLA), constants of Stafford's "Mix13" variant.
#pragma once

#include <cstdint>

namespace agro::rng {

// Stream identifiers. New streams are appended; existing values never change, because they
// are part of every saved world.
enum class Stream : std::uint32_t {
  kWeatherGenerator = 1,  // synthetic weather (open mode only)
  kPestOutbreak = 2,
  kDiseaseOutbreak = 3,
  kEquipmentFailure = 4,
  kMarketEvent = 5,  // phase 4
};

struct Key {
  std::uint64_t world_seed;
  Stream stream;
  std::int64_t day;      // simulated day index
  std::uint32_t cell;    // cell index, or 0 for whole-farm events
  std::uint32_t index;   // n-th draw for this (stream, day, cell)
};

constexpr std::uint64_t kGolden = 0x9e3779b97f4a7c15ULL;

constexpr std::uint64_t mix64(std::uint64_t z) {
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  return z ^ (z >> 31);
}

// First output of SplitMix64 seeded with s: mix64(s + golden).
constexpr std::uint64_t splitmix64_first(std::uint64_t s) { return mix64(s + kGolden); }

constexpr std::uint64_t draw_u64(const Key& k) {
  std::uint64_t h = mix64(k.world_seed + kGolden);
  h = mix64(h ^ (static_cast<std::uint64_t>(k.stream) + 1 * kGolden));
  h = mix64(h ^ (static_cast<std::uint64_t>(k.day) + 2 * kGolden));
  h = mix64(h ^ (static_cast<std::uint64_t>(k.cell) + 3 * kGolden));
  h = mix64(h ^ (static_cast<std::uint64_t>(k.index) + 4 * kGolden));
  return h;
}

// Uniform double in [0, 1) with 53 random bits (exact conversion, no rounding).
constexpr double draw_unit(const Key& k) {
  return static_cast<double>(draw_u64(k) >> 11) * (1.0 / 9007199254740992.0);
}

// Bernoulli trial with probability p given in parts per million (integer, so the decision
// never depends on floating point).
constexpr bool draw_bernoulli_ppm(const Key& k, std::uint32_t p_ppm) {
  return (draw_u64(k) % 1000000ULL) < p_ppm;
}

}  // namespace agro::rng
