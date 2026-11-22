// mini_test.hpp — minimal test registry (no external dependency, builds with any C++17 compiler).
#pragma once

#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace mini_test {

struct Case {
  const char* name;
  std::function<void()> body;
};

inline std::vector<Case>& registry() {
  static std::vector<Case> r;
  return r;
}
inline int& failures() {
  static int f = 0;
  return f;
}

struct Registrar {
  Registrar(const char* name, std::function<void()> body) { registry().push_back({name, std::move(body)}); }
};

inline void fail(const char* file, int line, const std::string& what) {
  ++failures();
  std::printf("  FALHA %s:%d: %s\n", file, line, what.c_str());
}

inline int run_all() {
  int failed_cases = 0;
  for (const Case& c : registry()) {
    const int before = failures();
    try {
      c.body();
    } catch (const std::exception& e) {
      fail(c.name, 0, std::string("exceção: ") + e.what());
    }
    const bool ok = failures() == before;
    if (!ok) ++failed_cases;
    std::printf("%s %s\n", ok ? "[ok]   " : "[FALHA]", c.name);
  }
  std::printf("%zu teste(s), %d com falha\n", registry().size(), failed_cases);
  return failed_cases == 0 ? 0 : 1;
}

}  // namespace mini_test

#define MT_CAT2(a, b) a##b
#define MT_CAT(a, b) MT_CAT2(a, b)
#define TEST(name)                                                                       \
  static void MT_CAT(test_fn_, __LINE__)();                                              \
  static mini_test::Registrar MT_CAT(test_reg_, __LINE__)(name, MT_CAT(test_fn_, __LINE__)); \
  static void MT_CAT(test_fn_, __LINE__)()
#define CHECK(cond) \
  do { if (!(cond)) mini_test::fail(__FILE__, __LINE__, #cond); } while (0)
#define CHECK_EQ(a, b)                                                                     \
  do {                                                                                     \
    const auto mt_a = (a);                                                                 \
    const auto mt_b = (b);                                                                 \
    if (!(mt_a == mt_b))                                                                   \
      mini_test::fail(__FILE__, __LINE__, std::string(#a " == " #b " (") +                 \
                                              std::to_string(mt_a) + " != " + std::to_string(mt_b) + ")"); \
  } while (0)
#define CHECK_THROWS(expr)                                        \
  do {                                                            \
    bool mt_threw = false;                                        \
    try { (void)(expr); } catch (...) { mt_threw = true; }        \
    if (!mt_threw) mini_test::fail(__FILE__, __LINE__, "esperava exceção: " #expr); \
  } while (0)
