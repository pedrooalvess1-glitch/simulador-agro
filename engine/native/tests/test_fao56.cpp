// test_fao56.cpp — astronomy, radiation and ET0 against the worked examples of FAO-56.
//
// Reference values: Allen et al. (1998), FAO Irrigation and Drainage Paper 56 (registry id
// FAO-1998-FAO56), Examples 8 to 13 (chapter 3) and 17 to 19 (chapter 4), read on the FAO
// web edition (x0490e07.htm and x0490e08.htm) on 2026-11-23. The book prints rounded values
// and sometimes rounds intermediate steps; expect_printed() accepts the computed value when
// it rounds to the printed one. Where the book carried a rounded intermediate into a later
// line, the test allows one unit of the last printed digit and says why.
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <string>

#include "agro/fao56.hpp"
#include "agro/hash.hpp"
#include "mini_test.hpp"

using namespace agro;
using namespace agro::fao56;

namespace {

// |value - printed| <= units * 10^-decimals (units = 0.5 means "rounds to the printed value").
bool printed(double value, double expected, int decimals, double units = 0.5) {
  const double step = std::pow(10.0, -decimals);
  return std::fabs(value - expected) <= units * step + 1e-12;
}

#define EXPECT_PRINTED(value, expected, decimals)                                                  \
  do {                                                                                              \
    const double v_ = (value);                                                                      \
    if (!printed(v_, (expected), (decimals)))                                                       \
      mini_test::fail(__FILE__, __LINE__, std::string(#value " = ") + std::to_string(v_) +          \
                                              " não arredonda para " + std::to_string(expected));   \
  } while (0)
#define EXPECT_WITHIN_UNITS(value, expected, decimals, units)                                      \
  do {                                                                                              \
    const double v_ = (value);                                                                      \
    if (!printed(v_, (expected), (decimals), (units)))                                              \
      mini_test::fail(__FILE__, __LINE__, std::string(#value " = ") + std::to_string(v_) +          \
                                              " fora da tolerância de " + std::to_string(expected)); \
  } while (0)

double lat_deg_min(int deg, int min, bool south) {
  const double d = static_cast<double>(deg) + static_cast<double>(min) / 60.0;
  return deg_to_rad(south ? -d : d);
}

}  // namespace

TEST("FAO-56 Exemplos 8 e 9: Ra e N a 20° S em 3 de setembro (J = 246)") {
  const SolarDay s = solar_day(deg_to_rad(-20.0), 246);
  EXPECT_PRINTED(s.dr, 0.985, 3);
  EXPECT_PRINTED(s.declination, 0.120, 3);
  EXPECT_PRINTED(s.sunset_angle, 1.527, 3);
  EXPECT_PRINTED(s.ra, 32.2, 1);
  EXPECT_PRINTED(kInverseLatentHeat * s.ra, 13.1, 1);  // evaporação equivalente, mm/dia
  EXPECT_PRINTED(s.daylight_hours, 11.7, 1);
}

TEST("FAO-56 Exemplo 10: Rs pela insolação, Rio de Janeiro (22°54' S), maio (J = 135)") {
  const SolarDay s = solar_day(lat_deg_min(22, 54, true), 135);
  EXPECT_PRINTED(s.ra, 25.1, 1);
  EXPECT_PRINTED(s.daylight_hours, 10.9, 1);
  const double rs = rs_angstrom(220.0 / 31.0, s.daylight_hours, s.ra, 0.25, 0.50);
  EXPECT_PRINTED(rs, 14.5, 1);
  EXPECT_PRINTED(kInverseLatentHeat * rs, 5.9, 1);
}

TEST("FAO-56 Exemplos 11 e 12: Rnl e Rn, Rio de Janeiro, maio") {
  const double rnl = net_longwave_day(25.1, 19.1, 2.1, 14.5, 18.8);
  EXPECT_PRINTED(rnl, 3.5, 1);
  EXPECT_PRINTED(kInverseLatentHeat * rnl, 1.4, 1);
  const double rns = net_shortwave(14.5, kReferenceAlbedo);
  // O livro imprime Rns = 11,1 (0,77 × 14,5 = 11,165) e Rn = 11,1 − 3,5 = 7,6.
  EXPECT_WITHIN_UNITS(rns, 11.1, 1, 1.0);
  EXPECT_PRINTED(rns - rnl, 7.6, 1);
  EXPECT_PRINTED(kInverseLatentHeat * (rns - rnl), 3.1, 1);
}

TEST("FAO-56 Exemplo 13: G mensal de abril em Argel (março 14,1 °C, maio 18,8 °C)") {
  EXPECT_PRINTED(soil_heat_flux_month(18.8, 14.1), 0.33, 2);
}

TEST("FAO-56 Exemplo 17: ET0 com médias mensais, Bangkok (13°44' N, 2 m), abril (J = 105)") {
  const Site site{lat_deg_min(13, 44, false), 2.0};
  const SolarDay s = solar_day(site.latitude_rad, 105);
  const double rs = rs_angstrom(8.5, s.daylight_hours, s.ra, 0.25, 0.50);
  const DailyWeather w{105, 34.8, 25.6, 2.85, 2.0, rs, soil_heat_flux_month_prev(30.2, 29.2)};
  const Et0Day r = et0_day(site, w);
  EXPECT_PRINTED(r.sun.daylight_hours, 12.31, 2);
  EXPECT_PRINTED(r.sun.ra, 38.06, 2);
  EXPECT_PRINTED(rs, 22.65, 2);
  EXPECT_PRINTED(r.rso, 28.54, 2);
  EXPECT_PRINTED(r.rns, 17.44, 2);
  EXPECT_PRINTED(r.rnl, 3.11, 2);
  EXPECT_PRINTED(r.rn, 14.33, 2);
  EXPECT_PRINTED(w.g, 0.14, 2);
  EXPECT_PRINTED(saturation_vapour_pressure(34.8), 5.56, 2);
  EXPECT_PRINTED(saturation_vapour_pressure(25.6), 3.28, 2);
  EXPECT_PRINTED(r.es, 4.42, 2);
  EXPECT_PRINTED(r.vpd, 1.57, 2);
  EXPECT_PRINTED(r.delta, 0.246, 3);
  EXPECT_PRINTED(r.pressure, 101.3, 1);
  // O livro calcula gamma com P já arredondada (0,665e-3 × 101,3 = 0,06736 → 0,0674);
  // com P sem arredondar (101,276 kPa) gamma = 0,06735.
  EXPECT_WITHIN_UNITS(r.gamma, 0.0674, 4, 1.0);
  EXPECT_PRINTED(r.et0, 5.72, 2);
}

TEST("FAO-56 Exemplo 18: ET0 com dados diários, Uccle (50°48' N, 100 m), 6 de julho (J = 187)") {
  const Site site{lat_deg_min(50, 48, false), 100.0};
  const SolarDay s = solar_day(site.latitude_rad, 187);
  const double rs = rs_angstrom(9.25, s.daylight_hours, s.ra, 0.25, 0.50);
  const double u2 = wind_at_2m(10.0 / 3.6, 10.0);  // 10 km/h a 10 m
  const double ea = ea_from_rh_max_min(21.5, 12.3, 84.0, 63.0);
  const DailyWeather w{187, 21.5, 12.3, ea, u2, rs, 0.0};
  const Et0Day r = et0_day(site, w);
  EXPECT_PRINTED(u2, 2.078, 3);
  EXPECT_PRINTED(r.sun.daylight_hours, 16.1, 1);
  EXPECT_PRINTED(r.sun.ra, 41.09, 2);
  EXPECT_PRINTED(rs, 22.07, 2);
  EXPECT_PRINTED(r.rso, 30.90, 2);
  EXPECT_PRINTED(r.rns, 17.00, 2);
  EXPECT_PRINTED(r.rnl, 3.71, 2);
  EXPECT_PRINTED(r.rn, 13.28, 2);
  EXPECT_PRINTED(saturation_vapour_pressure(21.5), 2.564, 3);
  EXPECT_PRINTED(saturation_vapour_pressure(12.3), 1.431, 3);
  EXPECT_PRINTED(r.es, 1.997, 3);
  EXPECT_PRINTED(ea, 1.409, 3);
  EXPECT_PRINTED(r.vpd, 0.589, 3);
  EXPECT_PRINTED(r.delta, 0.122, 3);
  EXPECT_PRINTED(r.gamma, 0.0666, 4);
  EXPECT_PRINTED(r.pressure, 100.1, 1);
  EXPECT_PRINTED(r.et0, 3.88, 2);
}

TEST("FAO-56 Exemplo 19: ET0 horária, N'Diaye (16°13' N, 16°15' O, 8 m), 1º de outubro (J = 274)") {
  const double lat = lat_deg_min(16, 13, false);
  const int day = 274;
  const double gamma = psychrometric_constant(atmospheric_pressure(8.0));
  EXPECT_PRINTED(seasonal_correction_hours(day), 0.1889, 4);
  EXPECT_PRINTED(inverse_relative_distance(day), 1.0001, 4);
  EXPECT_PRINTED(solar_declination(day), -0.0753, 4);
  EXPECT_PRINTED(gamma, 0.0673, 4);

  // 02:00-03:00: noite, sem radiação; Rs/Rso = 0,8 suposto pelo livro.
  {
    const double w = solar_time_angle(2.5, 15.0, 16.25, day);
    EXPECT_PRINTED(w, -2.46, 2);
    const double ra = extraterrestrial_radiation_period(lat, day, w, 1.0);
    CHECK(ra == 0.0);
    const double eo = saturation_vapour_pressure(28.0);
    const double ea = ea_from_rh_hour(28.0, 90.0);
    const double delta = saturation_slope(28.0);
    const double rnl = net_longwave_hour(28.0, ea, 0.8);
    const double rn = net_shortwave(0.0, kReferenceAlbedo) - rnl;
    const double g = soil_heat_flux_hour(rn, false);
    EXPECT_PRINTED(eo, 3.780, 3);
    EXPECT_PRINTED(ea, 3.402, 3);
    EXPECT_PRINTED(delta, 0.220, 3);
    EXPECT_PRINTED(rnl, 0.100, 3);
    EXPECT_PRINTED(rn, -0.100, 3);
    EXPECT_PRINTED(g, -0.050, 3);
    EXPECT_PRINTED(et0_penman_monteith_hour(delta, gamma, rn, g, 28.0, 1.9, eo, ea), 0.00, 2);
  }
  // 14:00-15:00.
  {
    const double w = solar_time_angle(14.5, 15.0, 16.25, day);
    EXPECT_PRINTED(w, 0.682, 3);
    EXPECT_PRINTED(w - kPi / 24.0, 0.5512, 4);
    EXPECT_PRINTED(w + kPi / 24.0, 0.8130, 4);
    const double ra = extraterrestrial_radiation_period(lat, day, w, 1.0);
    const double rso = rso_from_altitude(8.0, ra);
    const double rs = 2.450;
    const double eo = saturation_vapour_pressure(38.0);
    const double ea = ea_from_rh_hour(38.0, 52.0);
    const double delta = saturation_slope(38.0);
    const double rns = net_shortwave(rs, kReferenceAlbedo);
    const double rnl = net_longwave_hour(38.0, ea, rs / rso);
    const double rn = rns - rnl;
    const double g = soil_heat_flux_hour(rn, true);
    EXPECT_PRINTED(ra, 3.543, 3);
    EXPECT_PRINTED(rso, 2.658, 3);
    EXPECT_PRINTED(rs / rso, 0.922, 3);
    EXPECT_PRINTED(eo, 6.625, 3);
    EXPECT_PRINTED(ea, 3.445, 3);
    EXPECT_PRINTED(delta, 0.358, 3);
    EXPECT_WITHIN_UNITS(rns, 1.887, 3, 1.0);  // 0,77 × 2,450 = 1,8865 (meio exato)
    EXPECT_PRINTED(rnl, 0.137, 3);
    EXPECT_PRINTED(rn, 1.749, 3);
    EXPECT_PRINTED(g, 0.175, 3);
    EXPECT_PRINTED(et0_penman_monteith_hour(delta, gamma, rn, g, 38.0, 3.3, eo, ea), 0.63, 2);
  }
}

TEST("astronomia: a soma das 24 Ra horárias é a Ra diária em todas as latitudes do Brasil") {
  // Integrar a Eq. 28 de -omega_s a +omega_s dá a Eq. 21. A soma das horas, com as bordas
  // cortadas no nascer e no pôr do sol, precisa reproduzir a Ra diária. O deslocamento entre
  // hora local e hora solar (fuso e Sc) não muda a soma, porque as 24 horas cobrem 2*pi.
  double worst = 0.0;
  for (int lat10 = -340; lat10 <= 60; lat10 += 20) {
    const double lat = deg_to_rad(lat10 / 10.0);
    for (int day = 1; day <= 365; day += 7) {
      double sum = 0.0;
      for (int h = 0; h < 24; ++h) {
        const double w = solar_time_angle(h + 0.5, 45.0, 47.5, day);
        sum += extraterrestrial_radiation_period(lat, day, w, 1.0);
      }
      worst = std::fmax(worst, std::fabs(sum - solar_day(lat, day).ra));
    }
  }
  CHECK(worst < 1e-9);
}

TEST("FAO-56: casos-limite (Rs/Rso limitado a 1, latitude polar sem NaN)") {
  CHECK(net_longwave_day(30.0, 20.0, 2.0, 30.0, 25.0) == net_longwave_day(30.0, 20.0, 2.0, 25.0, 25.0));
  const SolarDay polar_night = solar_day(deg_to_rad(80.0), 355);
  CHECK(polar_night.ra == 0.0 && polar_night.daylight_hours == 0.0);
  const SolarDay polar_day = solar_day(deg_to_rad(80.0), 172);
  CHECK(polar_day.daylight_hours == 24.0 && polar_day.ra > 0.0);
}

TEST("FAO-56: hash de uma grade de entradas é o mesmo em toda plataforma (Linux = Windows)") {
  // Os bits de saída dependem só de +, -, *, /, sqrt e do detmath. O valor abaixo foi gerado
  // com g++ -O2 no Linux e tem de sair igual em g++ -O0, clang++, -march=haswell e MinGW.
  Fnv1a64 h;
  for (int lat10 = -340; lat10 <= 60; lat10 += 35) {
    const Site site{deg_to_rad(lat10 / 10.0), 10.0 * (lat10 + 340)};
    for (int day = 1; day <= 366; day += 13) {
      const SolarDay s = solar_day(site.latitude_rad, day);
      for (int k = 0; k < 6; ++k) {
        const double tmax = 18.0 + 3.1 * k, tmin = tmax - 4.0 - 1.7 * k;
        const double ea = ea_from_rh_max_min(tmax, tmin, 95.0 - 5.0 * k, 60.0 - 7.0 * k);
        const double rs = rs_angstrom(1.0 + 1.9 * k, s.daylight_hours, s.ra, 0.25, 0.50);
        const Et0Day r = et0_day(site, DailyWeather{day, tmax, tmin, ea, 0.5 + 0.7 * k, rs, 0.0});
        h.f64(r.sun.ra);
        h.f64(r.sun.daylight_hours);
        h.f64(r.rnl);
        h.f64(r.et0);
        const double w = solar_time_angle(7.5 + 1.5 * k, 45.0, 38.0 + k, day);
        h.f64(extraterrestrial_radiation_period(site.latitude_rad, day, w, 1.0));
      }
    }
  }
  const std::uint64_t expected = 0xe5db0f8758b3502eULL;
  if (h.value() != expected) std::printf("  hash da grade FAO-56: 0x%016llx\n", static_cast<unsigned long long>(h.value()));
  CHECK(h.value() == expected);
}
