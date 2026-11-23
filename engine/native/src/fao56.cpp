// fao56.cpp — FAO-56 astronomy, radiation and Penman-Monteith ET0 (see fao56.hpp).
// Equation numbers refer to FAO Irrigation and Drainage Paper 56 [FONTE:FAO-1998-FAO56].
#include "agro/fao56.hpp"

#include "agro/detmath.hpp"

namespace agro::fao56 {

namespace dm = agro::detmath;

double deg_to_rad(double degrees) { return degrees * (kPi / 180.0); }

// ---------------------------------------------------------------- atmosphere and humidity
double atmospheric_pressure(double altitude_m) {
  return 101.3 * dm::pow((293.0 - 0.0065 * altitude_m) / 293.0, 5.26);
}

double psychrometric_constant(double pressure_kpa) { return 0.665e-3 * pressure_kpa; }

double saturation_vapour_pressure(double t_c) { return 0.6108 * dm::exp(17.27 * t_c / (t_c + 237.3)); }

double mean_saturation_vapour_pressure(double tmax_c, double tmin_c) {
  return (saturation_vapour_pressure(tmax_c) + saturation_vapour_pressure(tmin_c)) / 2.0;
}

double saturation_slope(double t_c) {
  const double d = t_c + 237.3;
  return 4098.0 * saturation_vapour_pressure(t_c) / (d * d);
}

double ea_from_dewpoint(double tdew_c) { return saturation_vapour_pressure(tdew_c); }

double ea_from_rh_max_min(double tmax_c, double tmin_c, double rhmax_pct, double rhmin_pct) {
  return (saturation_vapour_pressure(tmin_c) * rhmax_pct / 100.0 +
          saturation_vapour_pressure(tmax_c) * rhmin_pct / 100.0) /
         2.0;
}

double ea_from_rh_max(double tmin_c, double rhmax_pct) {
  return saturation_vapour_pressure(tmin_c) * rhmax_pct / 100.0;
}

double ea_from_rh_mean(double tmax_c, double tmin_c, double rhmean_pct) {
  return rhmean_pct / 100.0 * mean_saturation_vapour_pressure(tmax_c, tmin_c);
}

double ea_from_rh_hour(double t_c, double rh_pct) { return saturation_vapour_pressure(t_c) * rh_pct / 100.0; }

// ------------------------------------------------------------------------- astronomy
double inverse_relative_distance(int day_of_year) {
  return 1.0 + 0.033 * dm::cos(2.0 * kPi / 365.0 * static_cast<double>(day_of_year));
}

double solar_declination(int day_of_year) {
  return 0.409 * dm::sin(2.0 * kPi / 365.0 * static_cast<double>(day_of_year) - 1.39);
}

double sunset_hour_angle(double latitude_rad, double declination) {
  // Eq. 25. The argument is clamped to [-1, 1] so that polar day and night stay defined;
  // it never leaves that range at Brazilian latitudes.
  double x = -dm::tan(latitude_rad) * dm::tan(declination);
  if (x > 1.0) x = 1.0;
  if (x < -1.0) x = -1.0;
  return dm::acos(x);
}

double daylight_hours(double sunset_angle) { return 24.0 / kPi * sunset_angle; }

SolarDay solar_day(double latitude_rad, int day_of_year) {
  SolarDay s{};
  s.dr = inverse_relative_distance(day_of_year);
  s.declination = solar_declination(day_of_year);
  s.sunset_angle = sunset_hour_angle(latitude_rad, s.declination);
  const double ws = s.sunset_angle;
  s.ra = 24.0 * 60.0 / kPi * kSolarConstant * s.dr *
         (ws * dm::sin(latitude_rad) * dm::sin(s.declination) +
          dm::cos(latitude_rad) * dm::cos(s.declination) * dm::sin(ws));  // Eq. 21
  s.daylight_hours = daylight_hours(ws);
  return s;
}

double seasonal_correction_hours(int day_of_year) {
  const double b = 2.0 * kPi * static_cast<double>(day_of_year - 81) / 364.0;  // Eq. 33
  return 0.1645 * dm::sin(2.0 * b) - 0.1255 * dm::cos(b) - 0.025 * dm::sin(b);  // Eq. 32
}

double solar_time_angle(double t_mid_hours, double lz_deg_west, double lm_deg_west, int day_of_year) {
  return kPi / 12.0 *
         ((t_mid_hours + 0.06667 * (lz_deg_west - lm_deg_west) + seasonal_correction_hours(day_of_year)) - 12.0);
}

double extraterrestrial_radiation_period(double latitude_rad, int day_of_year, double omega,
                                         double period_hours) {
  const double dr = inverse_relative_distance(day_of_year);
  const double decl = solar_declination(day_of_year);
  const double ws = sunset_hour_angle(latitude_rad, decl);
  double w1 = omega - kPi * period_hours / 24.0;  // Eq. 29
  double w2 = omega + kPi * period_hours / 24.0;  // Eq. 30
  // Edges clipped to the sunlit arc [-omega_s, omega_s] [MODELO]. A period wholly below the
  // horizon gives zero, as the text of Eq. 28 requires; the sunrise and sunset hours keep
  // only their sunlit part, so the 24 hourly values add up to the daily Ra of Eq. 21.
  if (w1 < -ws) w1 = -ws;
  if (w2 < -ws) w2 = -ws;
  if (w1 > ws) w1 = ws;
  if (w2 > ws) w2 = ws;
  if (w1 > w2) w1 = w2;
  return 12.0 * 60.0 / kPi * kSolarConstant * dr *
         ((w2 - w1) * dm::sin(latitude_rad) * dm::sin(decl) +
          dm::cos(latitude_rad) * dm::cos(decl) * (dm::sin(w2) - dm::sin(w1)));  // Eq. 28
}

// ------------------------------------------------------------------------- radiation
double rs_angstrom(double sunshine_h, double daylight_h, double ra, double a_s, double b_s) {
  return (a_s + b_s * sunshine_h / daylight_h) * ra;
}

double rso_from_altitude(double altitude_m, double ra) { return (0.75 + 2e-5 * altitude_m) * ra; }

double rso_from_angstrom(double a_s, double b_s, double ra) { return (a_s + b_s) * ra; }

double net_shortwave(double rs, double albedo) { return (1.0 - albedo) * rs; }

namespace {
double kelvin4(double t_c) {
  const double k = t_c + 273.16;  // FAO-56 uses K = degC + 273.16 in Eq. 39
  const double k2 = k * k;
  return k2 * k2;
}
double cloudiness(double rs_over_rso) {
  double r = rs_over_rso;
  if (r > 1.0) r = 1.0;  // Rs/Rso <= 1.0 (text of Eq. 39)
  return 1.35 * r - 0.35;
}
}  // namespace

double net_longwave_day(double tmax_c, double tmin_c, double ea, double rs, double rso) {
  const double t4 = (kStefanBoltzmannDay * kelvin4(tmax_c) + kStefanBoltzmannDay * kelvin4(tmin_c)) / 2.0;
  return t4 * (0.34 - 0.14 * dm::sqrt(ea)) * cloudiness(rs / rso);
}

double net_longwave_hour(double t_c, double ea, double rs_over_rso) {
  return kStefanBoltzmannHour * kelvin4(t_c) * (0.34 - 0.14 * dm::sqrt(ea)) * cloudiness(rs_over_rso);
}

// ------------------------------------------------------------------------ soil heat flux
double soil_heat_flux_month(double t_next_month_c, double t_prev_month_c) {
  return 0.07 * (t_next_month_c - t_prev_month_c);
}

double soil_heat_flux_month_prev(double t_month_c, double t_prev_month_c) {
  return 0.14 * (t_month_c - t_prev_month_c);
}

double soil_heat_flux_hour(double rn_hour, bool daylight) { return (daylight ? 0.1 : 0.5) * rn_hour; }

// ------------------------------------------------------------------------------- wind
double wind_at_2m(double u_z, double height_m) { return u_z * 4.87 / dm::log(67.8 * height_m - 5.42); }

// --------------------------------------------------------------------------------- ET0
double et0_penman_monteith_day(double delta, double gamma, double rn, double g, double t_mean_c,
                               double u2, double es, double ea) {
  const double num = kInverseLatentHeat * delta * (rn - g) + gamma * (900.0 / (t_mean_c + 273.0)) * u2 * (es - ea);
  return num / (delta + gamma * (1.0 + 0.34 * u2));
}

double et0_penman_monteith_hour(double delta, double gamma, double rn, double g, double t_c,
                                double u2, double es, double ea) {
  const double num = kInverseLatentHeat * delta * (rn - g) + gamma * (37.0 / (t_c + 273.0)) * u2 * (es - ea);
  return num / (delta + gamma * (1.0 + 0.34 * u2));
}

Et0Day et0_day(const Site& site, const DailyWeather& w) {
  Et0Day r{};
  r.sun = solar_day(site.latitude_rad, w.day_of_year);
  r.pressure = atmospheric_pressure(site.altitude_m);
  r.gamma = psychrometric_constant(r.pressure);
  r.t_mean = (w.tmax_c + w.tmin_c) / 2.0;  // Eq. 9
  r.es = mean_saturation_vapour_pressure(w.tmax_c, w.tmin_c);
  r.delta = saturation_slope(r.t_mean);
  r.vpd = r.es - w.ea_kpa;
  r.rso = rso_from_altitude(site.altitude_m, r.sun.ra);
  r.rns = net_shortwave(w.rs, kReferenceAlbedo);
  r.rnl = net_longwave_day(w.tmax_c, w.tmin_c, w.ea_kpa, w.rs, r.rso);
  r.rn = r.rns - r.rnl;  // Eq. 40
  r.et0 = et0_penman_monteith_day(r.delta, r.gamma, r.rn, w.g, r.t_mean, w.u2_ms, r.es, w.ea_kpa);
  return r;
}

}  // namespace agro::fao56
