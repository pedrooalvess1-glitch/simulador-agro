// fao56.hpp — astronomy, radiation and FAO Penman-Monteith reference evapotranspiration.
//
// Source: Allen, Pereira, Raes & Smith (1998), FAO Irrigation and Drainage Paper 56
// (registry id FAO-1998-FAO56), chapter 3 (equations 7 to 54) and chapter 4 (equations 6
// and 53). Every function cites its equation. The numeric constants below are part of the
// published equations (or of the definition of the reference grass surface: albedo 0.23,
// height 0.12 m, surface resistance 70 s/m, folded into the 900, 37 and 0.34 of Eq. 6/53).
// They are not agronomic content of a crop. Site calibration (Angstrom a and b) and the
// weather come from the content pack and are passed in.
//
// Determinism: only +, -, *, /, sqrt and agro::detmath functions, in a fixed order, so the
// result is bit-identical on Linux (CI) and Windows (player). Tests compare against the
// worked Examples 8 to 13 and 17 to 19 of FAO-56 and fix a hash over an input grid.
//
// Units follow FAO-56: temperature degC, pressure and vapour pressure kPa, radiation
// MJ m-2 per period (day or hour), wind m/s at 2 m, angles in radians, ET0 mm per period.
#pragma once

namespace agro::fao56 {

inline constexpr double kPi = 3.141592653589793;
inline constexpr double kSolarConstant = 0.0820;           // Gsc, MJ m-2 min-1 (Eq. 21)
inline constexpr double kStefanBoltzmannDay = 4.903e-9;    // MJ K-4 m-2 day-1 (Eq. 39)
inline constexpr double kStefanBoltzmannHour = 2.043e-10;  // MJ K-4 m-2 hour-1 (Eq. 39, hourly)
inline constexpr double kReferenceAlbedo = 0.23;           // reference grass (Eq. 38)
inline constexpr double kInverseLatentHeat = 0.408;        // 1/lambda, kg MJ-1 (Eq. 20)

double deg_to_rad(double degrees);  // Eq. 22

// ---------------------------------------------------------------- atmosphere and humidity
double atmospheric_pressure(double altitude_m);                    // Eq. 7, kPa
double psychrometric_constant(double pressure_kpa);                // Eq. 8, kPa/degC
double saturation_vapour_pressure(double t_c);                     // Eq. 11, e°(T), kPa
double mean_saturation_vapour_pressure(double tmax_c, double tmin_c);  // Eq. 12, es
double saturation_slope(double t_c);                               // Eq. 13, Delta, kPa/degC
double ea_from_dewpoint(double tdew_c);                            // Eq. 14
double ea_from_rh_max_min(double tmax_c, double tmin_c, double rhmax_pct, double rhmin_pct);  // Eq. 17
double ea_from_rh_max(double tmin_c, double rhmax_pct);            // Eq. 18
double ea_from_rh_mean(double tmax_c, double tmin_c, double rhmean_pct);  // Eq. 19
double ea_from_rh_hour(double t_c, double rh_pct);                 // Eq. 54

// ------------------------------------------------------------------------- astronomy
double inverse_relative_distance(int day_of_year);                  // Eq. 23, dr
double solar_declination(int day_of_year);                          // Eq. 24, delta, rad
double sunset_hour_angle(double latitude_rad, double declination);  // Eq. 25, omega_s, rad
double daylight_hours(double sunset_angle);                         // Eq. 34, N, h

struct SolarDay {
  double dr;              // inverse relative distance Earth-Sun
  double declination;     // rad
  double sunset_angle;    // omega_s, rad
  double ra;              // extraterrestrial radiation, MJ m-2 day-1 (Eq. 21)
  double daylight_hours;  // N, h
};
SolarDay solar_day(double latitude_rad, int day_of_year);  // Eqs. 21, 23-25, 34

// Hourly or shorter periods. Longitudes in degrees WEST of Greenwich, as in FAO-56:
// lz = centre of the local time zone, lm = site.
double seasonal_correction_hours(int day_of_year);  // Eqs. 32-33, Sc
double solar_time_angle(double t_mid_hours, double lz_deg_west, double lm_deg_west,
                        int day_of_year);  // Eq. 31, omega at the midpoint of the period
// Extraterrestrial radiation of a period of length period_hours centred on omega (Eqs. 28-30).
// The period edges are clipped to the sunlit arc [-omega_s, omega_s] [MODELO]: zero for a
// period wholly below the horizon, only the sunlit part at sunrise and sunset, so that the
// hourly values add up to the daily Ra of Eq. 21.
double extraterrestrial_radiation_period(double latitude_rad, int day_of_year, double omega,
                                         double period_hours);

// ------------------------------------------------------------------------- radiation
double rs_angstrom(double sunshine_h, double daylight_h, double ra, double a_s, double b_s);  // Eq. 35
double rso_from_altitude(double altitude_m, double ra);             // Eq. 37
double rso_from_angstrom(double a_s, double b_s, double ra);        // Eq. 36
double net_shortwave(double rs, double albedo);                     // Eq. 38
double net_longwave_day(double tmax_c, double tmin_c, double ea, double rs, double rso);  // Eq. 39
// Hourly Eq. 39 with sigma per hour and the hourly temperature; rs_over_rso is the measured
// ratio (at night FAO-56 uses the ratio of a period shortly before sunset, passed by the caller).
double net_longwave_hour(double t_c, double ea, double rs_over_rso);

// ------------------------------------------------------------------------ soil heat flux
double soil_heat_flux_month(double t_next_month_c, double t_prev_month_c);  // Eq. 43
double soil_heat_flux_month_prev(double t_month_c, double t_prev_month_c);  // Eq. 44
double soil_heat_flux_hour(double rn_hour, bool daylight);                  // Eqs. 45-46

// ------------------------------------------------------------------------------- wind
double wind_at_2m(double u_z, double height_m);  // Eq. 47

// --------------------------------------------------------------------------------- ET0
// FAO Penman-Monteith, daily or monthly step (Eq. 6), mm/day.
double et0_penman_monteith_day(double delta, double gamma, double rn, double g, double t_mean_c,
                               double u2, double es, double ea);
// FAO Penman-Monteith, hourly step (Eq. 53), mm/hour.
double et0_penman_monteith_hour(double delta, double gamma, double rn, double g, double t_c,
                                double u2, double es, double ea);

// Whole daily chain (FAO-56 Boxes 7, 9, 10 and 11) from the variables of a daily weather
// grid such as BR-DWGD (Tmax, Tmin, ea, u2, Rs). All intermediate values are returned for
// tests, the field notebook and the reference-scenario CSV.
struct Site {
  double latitude_rad;
  double altitude_m;
};
struct DailyWeather {
  int day_of_year;
  double tmax_c;
  double tmin_c;
  double ea_kpa;
  double u2_ms;
  double rs;  // MJ m-2 day-1
  double g;   // MJ m-2 day-1 (0 for a daily step, Eq. 42; Eq. 43/44 for monthly steps)
};
struct Et0Day {
  SolarDay sun;
  double pressure, gamma, t_mean, es, delta, vpd;
  double rso, rns, rnl, rn;
  double et0;  // mm/day
};
Et0Day et0_day(const Site& site, const DailyWeather& w);

}  // namespace agro::fao56
