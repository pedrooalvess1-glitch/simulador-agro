// water.cpp — daily soil-water balance of one layer per cell (see water.hpp).
#include "agro/water.hpp"

#include <stdexcept>
#include <string>

#include "agro/allocate.hpp"

namespace agro::water {

double runoff_curve_number_mm(double rain_mm, double curve_number) {
  if (!(curve_number > 0.0 && curve_number <= 100.0)) throw std::invalid_argument("water: CN outside (0, 100]");
  if (!(rain_mm >= 0.0)) throw std::invalid_argument("water: negative rain");
  const double s = 25400.0 / curve_number - 254.0;  // NEH-630 Eq. 10-13 (S in mm)
  const double ia = 0.2 * s;                        // Eq. 10-10
  if (rain_mm <= ia) return 0.0;                    // Eq. 10-1
  const double excess = rain_mm - ia;
  return excess * excess / (excess + s);            // Eq. 10-9
}

double reduction_coefficient(double water, double floor, double field_capacity, double p) {
  const double span = (1.0 - p) * (field_capacity - floor);
  if (span <= 0.0) return water > floor ? 1.0 : 0.0;
  double r = (water - floor) / span;
  if (r > 1.0) r = 1.0;
  if (r < 0.0) r = 0.0;
  return r;
}

namespace {
std::int64_t volume_ml(double theta, const CellSoil& s) {
  return to_units(theta * s.depth_m * s.area_m2 * 1.0e6);  // m3 -> mL
}
void check_soil(const CellSoil& s) {
  if (!(s.area_m2 > 0.0 && s.depth_m > 0.0)) throw std::invalid_argument("water: area and depth must be positive");
  if (!(0.0 <= s.theta_dry && s.theta_dry <= s.theta_wp && s.theta_wp < s.theta_fc && s.theta_fc <= 1.0))
    throw std::invalid_argument("water: need 0 <= theta_dry <= theta_wp < theta_fc <= 1");
  if (!(s.curve_number > 0.0 && s.curve_number <= 100.0)) throw std::invalid_argument("water: CN outside (0, 100]");
}
}  // namespace

SingleLayer::SingleLayer(std::vector<CellSoil> cells, const std::vector<double>& initial_theta)
    : cells_(std::move(cells)) {
  if (initial_theta.size() != cells_.size()) throw std::invalid_argument("water: one initial theta per cell");
  rain_ = books_.open("rain", AccountKind::kBoundary);
  source_ = books_.open("irrigation_source", AccountKind::kBoundary);
  atmosphere_ = books_.open("atmosphere", AccountKind::kBoundary);
  runoff_ = books_.open("runoff", AccountKind::kBoundary);
  deep_ = books_.open("deep_drainage", AccountKind::kBoundary);
  initial_ = books_.open("initial_condition", AccountKind::kBoundary);
  for (std::uint32_t c = 0; c < cells_.size(); ++c) {
    const CellSoil& s = cells_[c];
    check_soil(s);
    if (!(s.theta_dry <= initial_theta[c] && initial_theta[c] <= s.theta_fc))
      throw std::invalid_argument("water: initial theta outside [theta_dry, theta_fc]");
    fc_.push_back(volume_ml(s.theta_fc, s));
    wp_.push_back(volume_ml(s.theta_wp, s));
    dry_.push_back(volume_ml(s.theta_dry, s));
    soil_.push_back(books_.open("soil" + std::to_string(c), AccountKind::kPool, c));
    applied_.push_back(books_.open("applied_today" + std::to_string(c), AccountKind::kPool, c));
    books_.transfer(initial_, soil_[c], volume_ml(initial_theta[c], s), Flux::kInitial);
  }
  if (!books_.close_day().empty()) throw std::logic_error("water: initial books do not close");
}

void SingleLayer::set_curve_number(std::uint32_t c, double cn) {
  if (!(cn > 0.0 && cn <= 100.0)) throw std::invalid_argument("water: CN outside (0, 100]");
  cells_.at(c).curve_number = cn;
}

void SingleLayer::apply_water(std::uint32_t c, std::int64_t to_soil_ml, std::int64_t to_air_ml,
                              std::int64_t to_runoff_ml) {
  books_.transfer(source_, applied_.at(c), to_soil_ml, Flux::kIrrigation);
  books_.transfer(source_, atmosphere_, to_air_ml, Flux::kIrrigationLoss);
  books_.transfer(source_, runoff_, to_runoff_ml, Flux::kIrrigationLoss);
}

std::vector<CellDay> SingleLayer::step_day(const std::vector<double>& rain_mm,
                                           const std::vector<std::vector<Claim>>& claims) {
  if (rain_mm.size() != cells_.size() || claims.size() != cells_.size())
    throw std::invalid_argument("water: one rain value and one claim list per cell");
  std::vector<CellDay> out(cells_.size());
  for (std::uint32_t c = 0; c < cells_.size(); ++c) {
    const CellSoil& s = cells_[c];
    CellDay& d = out[c];
    const std::vector<Claim>& cl = claims[c];
    d.storage_start = storage(c);

    // 1. Reduction coefficients from the water at the start of the day (FAO-56 Example 37).
    std::vector<std::int64_t> floor(cl.size());
    d.reduction.resize(cl.size());
    for (std::size_t i = 0; i < cl.size(); ++i) {
      if (!(cl[i].potential_mm >= 0.0) || !(cl[i].p >= 0.0 && cl[i].p < 1.0))
        throw std::invalid_argument("water: claim needs potential >= 0 and 0 <= p < 1");
      floor[i] = cl[i].sink == Sink::kTranspiration ? wp_[c] : dry_[c];
      d.reduction[i] = reduction_coefficient(static_cast<double>(d.storage_start), static_cast<double>(floor[i]),
                                             static_cast<double>(fc_[c]), cl[i].p);
    }

    // 2. Rain: runoff by the curve number, the rest infiltrates.
    d.rain = to_units(mm_to_ml(rain_mm[c], s.area_m2));
    if (d.rain < 0) throw std::invalid_argument("water: negative rain");
    d.runoff = to_units(mm_to_ml(runoff_curve_number_mm(rain_mm[c], s.curve_number), s.area_m2));
    if (d.runoff > d.rain) d.runoff = d.rain;
    books_.transfer(rain_, runoff_, d.runoff, Flux::kRunoff);
    books_.transfer(rain_, soil_[c], d.rain - d.runoff, Flux::kRain);

    // 3. Water applied during the day joins the layer (ARQUITETURA_SIMULACAO §4.1).
    d.applied = applied_today(c);
    books_.transfer(applied_[c], soil_[c], d.applied, Flux::kInfiltration);

    // 4. Sinks: reduced demand, capped by the water above each sink's floor, then divided by
    //    the arbiter when they exceed the water above the lowest floor.
    const std::int64_t w = storage(c);
    std::vector<std::int64_t> want(cl.size(), 0);
    std::int64_t lowest = w;
    for (std::size_t i = 0; i < cl.size(); ++i) {
      want[i] = to_units(d.reduction[i] * mm_to_ml(cl[i].potential_mm, s.area_m2));
      const std::int64_t above = w > floor[i] ? w - floor[i] : 0;
      if (want[i] > above) want[i] = above;
      if (want[i] > 0 && floor[i] < lowest) lowest = floor[i];
    }
    d.granted = allocate_largest_remainder(w - lowest, want);
    for (std::size_t i = 0; i < cl.size(); ++i) {
      const bool transp = cl[i].sink == Sink::kTranspiration;
      books_.transfer(soil_[c], atmosphere_, d.granted[i], transp ? Flux::kTranspiration : Flux::kEvaporation);
      (transp ? d.transpiration : d.evaporation) += d.granted[i];
    }

    // 5. Water above field capacity drains on the same day (FAO-56 Eq. 88).
    const std::int64_t after = storage(c);
    d.drainage = after > fc_[c] ? after - fc_[c] : 0;
    books_.transfer(soil_[c], deep_, d.drainage, Flux::kDrainage);
    d.storage_end = storage(c);
  }
  const std::string problems = books_.close_day();
  if (!problems.empty()) throw std::logic_error("water: books do not close: " + problems);
  return out;
}

}  // namespace agro::water
