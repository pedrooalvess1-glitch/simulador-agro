// allocate.hpp — exact integer division of a conserved supply among competing claims.
//
// Used by the resource arbiters (water, nitrogen) when several species or sinks compete for
// one stock. Rounding each share separately could make the shares add up to supply +- 1 and
// break the exact balance. The largest-remainder (Hamilton) method gives every claimant the
// floor of its exact proportional quota and hands the few leftover units to the largest
// remainders, ties broken by the lower index. Integer arithmetic only: same result on every
// platform and in every order of evaluation.
#pragma once

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace agro {

namespace detail {

// floor(a * b / c) and (a * b) mod c for non-negative a, b and positive c, without overflow.
struct MulDiv {
  std::uint64_t quotient;
  std::uint64_t remainder;
};

// Portable 128-bit product and shift-subtract division (compilers without __int128, e.g. MSVC).
inline MulDiv muldiv_portable(std::uint64_t a, std::uint64_t b, std::uint64_t c) {
  const std::uint64_t a_lo = a & 0xffffffffULL, a_hi = a >> 32;
  const std::uint64_t b_lo = b & 0xffffffffULL, b_hi = b >> 32;
  const std::uint64_t p0 = a_lo * b_lo, p1 = a_lo * b_hi, p2 = a_hi * b_lo, p3 = a_hi * b_hi;
  const std::uint64_t mid = (p0 >> 32) + (p1 & 0xffffffffULL) + (p2 & 0xffffffffULL);
  std::uint64_t hi = p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);
  std::uint64_t lo = (mid << 32) | (p0 & 0xffffffffULL);
  std::uint64_t q = 0, r = 0;
  for (int i = 127; i >= 0; --i) {
    const std::uint64_t bit = i >= 64 ? (hi >> (i - 64)) & 1ULL : (lo >> i) & 1ULL;
    const bool carry = (r >> 63) != 0;
    r = (r << 1) | bit;
    q <<= 1;
    if (carry || r >= c) {
      r -= c;
      q |= 1ULL;
    }
  }
  return {q, r};
}

inline MulDiv muldiv(std::uint64_t a, std::uint64_t b, std::uint64_t c) {
#if defined(__SIZEOF_INT128__)
  const unsigned __int128 p = static_cast<unsigned __int128>(a) * b;
  return {static_cast<std::uint64_t>(p / c), static_cast<std::uint64_t>(p % c)};
#else
  return muldiv_portable(a, b, c);
#endif
}

}  // namespace detail

// Returns shares s_i with sum(s) == min(supply, sum(claims)), s_i <= claims_i, and
// |s_i - supply*claims_i/sum(claims)| < 1 when supply < sum(claims).
inline std::vector<std::int64_t> allocate_largest_remainder(std::int64_t supply,
                                                            const std::vector<std::int64_t>& claims) {
  if (supply < 0) throw std::invalid_argument("allocate: negative supply");
  std::uint64_t total = 0;
  for (auto c : claims) {
    if (c < 0) throw std::invalid_argument("allocate: negative claim");
    total += static_cast<std::uint64_t>(c);
  }
  std::vector<std::int64_t> share(claims.size(), 0);
  if (total == 0) return share;
  if (total <= static_cast<std::uint64_t>(supply)) return claims;  // everyone fully served

  std::vector<std::uint64_t> rem(claims.size(), 0);
  std::uint64_t given = 0;
  for (std::size_t i = 0; i < claims.size(); ++i) {
    const auto md = detail::muldiv(static_cast<std::uint64_t>(supply),
                                   static_cast<std::uint64_t>(claims[i]), total);
    share[i] = static_cast<std::int64_t>(md.quotient);
    rem[i] = md.remainder;
    given += md.quotient;
  }
  std::uint64_t left = static_cast<std::uint64_t>(supply) - given;  // < claims.size()
  std::vector<std::size_t> order(claims.size());
  std::iota(order.begin(), order.end(), std::size_t{0});
  std::stable_sort(order.begin(), order.end(),
                   [&](std::size_t x, std::size_t y) { return rem[x] > rem[y]; });
  for (std::size_t n = 0; n < order.size() && left > 0; ++n) {
    if (rem[order[n]] == 0) break;
    share[order[n]] += 1;
    --left;
  }
  return share;
}

}  // namespace agro
