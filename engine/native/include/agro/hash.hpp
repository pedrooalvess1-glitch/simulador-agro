// hash.hpp — stable 64-bit state hash (FNV-1a) over a canonical little-endian byte stream.
//
// Used by the daily state hash and by the determinism tests (1x == 8x == sleep == save/load,
// Linux == Windows). FNV-1a is not cryptographic; it only has to change when any bit of the
// state changes and to be identical on every platform. Values are fed as their IEEE/two's-
// complement bit patterns in little-endian order, independent of the host byte order.
#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>
#include <vector>

namespace agro {

class Fnv1a64 {
 public:
  static constexpr std::uint64_t kOffset = 0xcbf29ce484222325ULL;
  static constexpr std::uint64_t kPrime = 0x100000001b3ULL;

  void byte(std::uint8_t b) {
    h_ ^= b;
    h_ *= kPrime;
  }
  void u64(std::uint64_t v) {
    for (int i = 0; i < 8; ++i) byte(static_cast<std::uint8_t>(v >> (8 * i)));
  }
  void i64(std::int64_t v) { u64(static_cast<std::uint64_t>(v)); }
  void f64(double v) {
    std::uint64_t u;
    std::memcpy(&u, &v, sizeof u);
    u64(u);
  }
  void text(std::string_view s) {
    for (char c : s) byte(static_cast<std::uint8_t>(c));
  }
  void i64s(const std::vector<std::int64_t>& v) {
    u64(v.size());
    for (auto x : v) i64(x);
  }
  void f64s(const std::vector<double>& v) {
    u64(v.size());
    for (auto x : v) f64(x);
  }
  std::uint64_t value() const { return h_; }

 private:
  std::uint64_t h_ = kOffset;
};

}  // namespace agro
