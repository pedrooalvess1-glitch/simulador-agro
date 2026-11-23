// ledger.hpp — exact double-entry accounting of conserved quantities.
//
// Every conserved quantity (water, N, P, K, C, soil mass) lives in integer accounts. A model
// computes a flux in double precision, converts it ONCE to integer units (to_units) and moves
// it with transfer(from, to, amount): the same integer leaves one account and enters another.
// Integer addition is exact and associative, so the books close with zero tolerance in any
// evaluation order and on any platform.
//
// Units (chosen so the smallest daily flux of interest is >> 1 unit):
//   water      mL    (1 mL on 1 m2 = 1 um of depth; volumes, not depths, so transfers between
//                     cells of different area stay exact)
//   N, P, K    ug
//   C          mg
//   soil mass  g
//
// Account kinds:
//   pool      physical stock inside the simulated system (a soil layer of a cell, ponded water,
//             mineral N...). A transfer that would make a pool negative is a model bug and throws.
//   boundary  the outside world (atmosphere, rain, deep drainage, off-farm, market, initial
//             condition). It may go negative; its balance is the cumulative net exchange, which
//             is what the "where did my water go?" panel shows.
#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "agro/hash.hpp"

namespace agro {

enum class Quantity : std::uint8_t { kWater, kNitrogen, kPhosphorus, kPotassium, kCarbon, kSoilMass };

enum class AccountKind : std::uint8_t { kPool, kBoundary };

// Journal labels. Appended only; values are stored in saves.
enum class Flux : std::uint16_t {
  kInitial = 0,
  kRain = 1,
  kIrrigation = 2,
  kIrrigationLoss = 3,
  kRunoff = 4,
  kRunon = 5,
  kInfiltration = 6,
  kDrainage = 7,
  kEvaporation = 8,
  kTranspiration = 9,
  kUptake = 10,
  kLeaching = 11,
  kFertilizer = 12,
  kMineralisation = 13,
  kImmobilisation = 14,
  kHarvest = 15,
  kResidue = 16,
  kRespiration = 17,
  kLateral = 18,
  kSoilMove = 19,
};

using AccountId = std::uint32_t;

// Converts a flux computed in double precision to integer units exactly once.
// llround rounds half away from zero and does not depend on the FPU rounding mode.
inline std::int64_t to_units(double value) {
  if (!(value == value)) throw std::domain_error("to_units: NaN flux");
  if (std::fabs(value) > 9.0e18) throw std::overflow_error("to_units: flux out of range");
  return std::llround(value);
}

class Ledger {
 public:
  struct Account {
    std::string name;
    AccountKind kind;
    std::uint32_t cell;  // owning cell, or 0xffffffff for farm-wide boundaries
    std::int64_t balance;
    std::int64_t opening;  // balance at the last close_day()
  };
  struct Entry {
    AccountId from;
    AccountId to;
    std::int64_t amount;
    Flux label;
  };

  explicit Ledger(Quantity q) : quantity_(q) {}

  AccountId open(std::string name, AccountKind kind, std::uint32_t cell = 0xffffffffU) {
    accounts_.push_back(Account{std::move(name), kind, cell, 0, 0});
    return static_cast<AccountId>(accounts_.size() - 1);
  }

  std::int64_t balance(AccountId a) const { return at(a).balance; }
  const Account& account(AccountId a) const { return at(a); }
  std::size_t size() const { return accounts_.size(); }
  Quantity quantity() const { return quantity_; }
  const std::vector<Entry>& journal() const { return journal_; }

  void transfer(AccountId from, AccountId to, std::int64_t amount, Flux label) {
    if (amount < 0) throw std::invalid_argument("ledger: negative amount (swap from/to)");
    if (amount == 0) return;
    if (from == to) throw std::invalid_argument("ledger: transfer to the same account");
    Account& f = at(from);
    Account& t = at(to);
    if (f.kind == AccountKind::kPool && f.balance < amount)
      throw std::logic_error("ledger: pool '" + f.name + "' would become negative");
    if (f.balance < std::numeric_limits<std::int64_t>::min() + amount ||
        t.balance > std::numeric_limits<std::int64_t>::max() - amount)
      throw std::overflow_error("ledger: int64 overflow");
    f.balance -= amount;
    t.balance += amount;
    journal_.push_back(Entry{from, to, amount, label});
  }

  // Checks the books for the period since the last close_day() and starts a new period.
  // Returns an empty string when everything closes exactly; otherwise a description.
  std::string close_day() {
    std::string problems;
    std::int64_t total = 0;
    std::vector<std::int64_t> net(accounts_.size(), 0);
    for (const Entry& e : journal_) {
      net[e.from] -= e.amount;
      net[e.to] += e.amount;
    }
    for (std::size_t i = 0; i < accounts_.size(); ++i) {
      const Account& a = accounts_[i];
      total += a.balance;
      if (a.opening + net[i] != a.balance) problems += "account '" + a.name + "' does not close; ";
      if (a.kind == AccountKind::kPool && a.balance < 0) problems += "pool '" + a.name + "' negative; ";
    }
    if (total != 0) problems += "sum of all accounts is not zero; ";
    for (Account& a : accounts_) a.opening = a.balance;
    journal_.clear();
    return problems;
  }

  // Save and load. A save may happen between any two events (ARQUITETURA_SIMULACAO §10), so
  // only the balances are stored: restore() starts a new checking period at the loaded
  // balances, and the next close_day() checks the transfers made after loading. It refuses
  // books of another chart of accounts, books that do not sum to zero and negative pools, so
  // a corrupt save cannot create or destroy a conserved quantity.
  std::vector<std::int64_t> balances() const {
    std::vector<std::int64_t> out;
    out.reserve(accounts_.size());
    for (const Account& a : accounts_) out.push_back(a.balance);
    return out;
  }
  void restore(const std::vector<std::int64_t>& b) {
    if (b.size() != accounts_.size()) throw std::invalid_argument("ledger: snapshot of another chart of accounts");
    std::int64_t total = 0;
    for (std::size_t i = 0; i < b.size(); ++i) {
      if (accounts_[i].kind == AccountKind::kPool && b[i] < 0) throw std::invalid_argument("ledger: negative pool in snapshot");
      total += b[i];
    }
    if (total != 0) throw std::invalid_argument("ledger: snapshot does not sum to zero");
    for (std::size_t i = 0; i < b.size(); ++i) accounts_[i].balance = accounts_[i].opening = b[i];
    journal_.clear();
  }

  void hash_into(Fnv1a64& h) const {
    h.u64(static_cast<std::uint64_t>(quantity_));
    h.u64(accounts_.size());
    for (const Account& a : accounts_) h.i64(a.balance);
  }

 private:
  Account& at(AccountId a) {
    if (a >= accounts_.size()) throw std::out_of_range("ledger: unknown account");
    return accounts_[a];
  }
  const Account& at(AccountId a) const {
    if (a >= accounts_.size()) throw std::out_of_range("ledger: unknown account");
    return accounts_[a];
  }

  Quantity quantity_;
  std::vector<Account> accounts_;
  std::vector<Entry> journal_;
};

}  // namespace agro
