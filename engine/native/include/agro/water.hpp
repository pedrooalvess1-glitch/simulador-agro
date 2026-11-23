// water.hpp — daily soil-water balance of one layer per cell, with exact integer books.
//
// Model (phase 1 of BASE_MODELOS §3.1, "bucket" of FAO-56 chapter 8):
//   - Rain becomes runoff by the NRCS curve number (NEH-630 ch. 10, Eqs. 10-9 to 10-13) and
//     the rest infiltrates. Water applied by the player does not run off (FAO-56 Eq. 85
//     subtracts runoff only from precipitation); the irrigation method may send part of it to
//     the air or to runoff when the action happens.
//   - Water applied during the day waits in the cell's "applied today" pool and joins the
//     balance at the next daily step (ARQUITETURA_SIMULACAO §4.1). Nothing is lost or delayed.
//   - Sinks (plant transpiration, soil evaporation) declare a potential demand. The soil owns
//     the water and reduces each demand by a coefficient computed from the water at the START
//     of the day (FAO-56 Example 37): r = (W - W_floor) / ((1 - p) (W_fc - W_floor)), clipped to
//     [0, 1]. With the wilting point as floor this is Ks of Eq. 84; with the air-dry content as
//     floor and p = REW/TEW it has the form of Kr of Eq. 74. In phase 1 the whole layer plays
//     the role of the evaporable layer [MODELO]; phase 2 splits it into layers.
//   - When the reduced demands exceed the water above the lowest floor, the largest-remainder
//     arbiter divides it (ARQUITETURA_SIMULACAO §4.3).
//   - Water above field capacity drains on the same day (FAO-56 Eq. 88).
// The module does not know any crop: demands, p and the soil parameters come from the caller
// (plant modules and the content pack). It only owns the water and keeps the books.
//
// Books (one water ledger for the farm, mL):
//   pools      per cell: soil layer, applied today
//   boundaries rain, irrigation source, atmosphere, runoff (leaves the cell's surface),
//              deep drainage, initial condition
#pragma once

#include <cstdint>
#include <vector>

#include "agro/hash.hpp"
#include "agro/ledger.hpp"

namespace agro::water {

struct CellSoil {
  double area_m2;
  double depth_m;       // layer thickness (phase 1: root zone of the bed)
  double theta_fc;      // m3/m3 at field capacity, from the retention curve of the pack
  double theta_wp;      // m3/m3 at the wilting point
  double theta_dry;     // m3/m3 air-dry, lower limit of soil evaporation
  double curve_number;  // NRCS CN of the cell's soil-cover complex
};

enum class Sink : std::uint8_t { kTranspiration, kEvaporation };

struct Claim {
  Sink sink;
  double potential_mm;  // demand without soil-water limitation, mm/day over the cell
  double p;             // fraction of the extractable water used without reduction, 0 <= p < 1
};

struct CellDay {
  std::int64_t rain = 0, runoff = 0, applied = 0;  // mL
  std::int64_t evaporation = 0, transpiration = 0, drainage = 0;
  std::int64_t storage_start = 0, storage_end = 0;
  std::vector<std::int64_t> granted;  // mL per claim, in the order of the input
  std::vector<double> reduction;      // Ks or Kr per claim
};

// NRCS runoff depth (mm) for a daily rain depth (mm): S = 25400/CN - 254, Ia = 0.2 S,
// Q = (P - Ia)^2 / (P - Ia + S) for P > Ia, else 0 (NEH-630 ch. 10, Eqs. 10-9 to 10-13).
double runoff_curve_number_mm(double rain_mm, double curve_number);

// Reduction coefficient of a sink (FAO-56 Eq. 84 form). All arguments in the same unit.
double reduction_coefficient(double water, double floor, double field_capacity, double p);

// Converts a depth over an area to mL (1 mm over 1 m2 = 1000 mL).
inline double mm_to_ml(double mm, double area_m2) { return mm * area_m2 * 1000.0; }

class SingleLayer {
 public:
  // initial_theta per cell, between theta_dry and theta_fc.
  SingleLayer(std::vector<CellSoil> cells, const std::vector<double>& initial_theta);

  std::size_t cells() const { return cells_.size(); }
  const CellSoil& soil(std::uint32_t c) const { return cells_.at(c); }
  void set_curve_number(std::uint32_t c, double cn);

  // Player action at any second: water reaching the surface of cell c. to_soil waits in
  // "applied today" until the daily step; to_air and to_runoff leave at once (losses of the
  // application method, from the pack).
  void apply_water(std::uint32_t c, std::int64_t to_soil_ml, std::int64_t to_air_ml = 0,
                   std::int64_t to_runoff_ml = 0);

  // Daily step. rain_mm per cell; claims per cell. Closes the books exactly or throws.
  std::vector<CellDay> step_day(const std::vector<double>& rain_mm,
                                const std::vector<std::vector<Claim>>& claims);

  std::int64_t storage(std::uint32_t c) const { return books_.balance(soil_.at(c)); }
  std::int64_t applied_today(std::uint32_t c) const { return books_.balance(applied_.at(c)); }
  std::int64_t field_capacity_ml(std::uint32_t c) const { return fc_.at(c); }
  std::int64_t wilting_point_ml(std::uint32_t c) const { return wp_.at(c); }
  std::int64_t air_dry_ml(std::uint32_t c) const { return dry_.at(c); }

  const Ledger& books() const { return books_; }
  AccountId rain_account() const { return rain_; }
  AccountId source_account() const { return source_; }
  AccountId atmosphere_account() const { return atmosphere_; }
  AccountId runoff_account() const { return runoff_; }
  AccountId deep_account() const { return deep_; }

  void hash_into(Fnv1a64& h) const { books_.hash_into(h); }
  std::vector<std::int64_t> save() const { return books_.balances(); }
  void load(const std::vector<std::int64_t>& balances) { books_.restore(balances); }

 private:
  std::vector<CellSoil> cells_;
  std::vector<std::int64_t> fc_, wp_, dry_;
  Ledger books_{Quantity::kWater};
  AccountId rain_{}, source_{}, atmosphere_{}, runoff_{}, deep_{}, initial_{};
  std::vector<AccountId> soil_, applied_;
};

}  // namespace agro::water
