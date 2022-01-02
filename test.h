#pragma once

#include <cxxabi.h>
#include <initializer_list> 
#include <iostream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <typeinfo>

#define REGISTER_TESTS static constexpr std::initializer_list<Test> __tests__ = 
#define REGISTER_TEST(test) { SHOULD_PASS, test, #test, __FILE__, __LINE__ }
#define REGISTER_TEST_EXPECT(expectation, test) { expectation, test, #test, __FILE__, __LINE__ }

namespace test {

namespace color {

constexpr const char *RESET  = "\e[0m";
constexpr const char *RED    = "\e[31m";
constexpr const char *GREEN  = "\e[32m";
constexpr const char *YELLOW = "\e[33m";
constexpr const char *CYAN   = "\e[36m";

} // namespace color

enum Status {
  PASSED,
  INCONCLUSIVE,
  FAILED,
};

enum Expectation {
  SHOULD_PASS,
  SHOULD_FAIL,
  SHOULD_BE_INCONCLUSIVE,
};

template<typename T>
struct Fixture;

class Tester {
public:
  bool is_true(bool test) {
    if (test) {
      return true;
    }

    set_status(FAILED);
    _report << color::RED << "\tExpected `true`." << color::RESET << std::endl;

    return false;
  }

  bool is_false(bool test) {
    if (!test) {
      return true;
    }

    set_status(FAILED);
    _report << color::RED << "\tExpected `false`." << color::RESET << std::endl;
    
    return false;
  }

  bool is_null(const void *ptr) {
    if (ptr == nullptr) {
      return true;
    }

    set_status(FAILED);
    _report << color::RED << "\tExpected `nullptr` but Actual = " << ptr << '.' << color::RESET << std::endl;

    return false;
  }

  bool not_null(const void *ptr) {
    if (ptr != nullptr) {
      return true;
    }

    set_status(FAILED);
    _report << color::RED << "\tUnexpected `nullptr`." << color::RESET << std::endl;

    return false;
  }

  template<typename T>
  bool is_eq(const T& actual, const T& expected) {
    if (actual == expected) {
      return true;
    }
    
    set_status(FAILED);
    _report << color::RED << "\tFailed equality!" << color::RESET << " Expected = " << expected << ", Actual = " << actual << '.' << std::endl;

    return false;
  }

  template<typename T>
  bool is_ne(const T& actual, const T& invalid) {
    if (actual != invalid) {
      return true;
    }

    set_status(FAILED);
    _report << color::RED << "\tFailed non-equality!" << color::RESET << " Actual value (" << actual << ") was invalid value (" << invalid << ")." << std::endl;

    return false;
  }

  template<typename T>
  bool is_lt(const T& a, const T& b) {
    if (a < b) {
      return true;
    }

    set_status(FAILED);
    _report << color::RED << "\tFailed less-than test!" << color::RESET << " a = " << a << ", b = " << b << '.' << std::endl;

    return false;
  }

  template<typename T>
  bool is_le(const T& a, const T& b) {
    if (a <= b) {
      return true;
    }

    set_status(FAILED);
    _report << color::RED << "\tFailed less-equal test!" << color::RESET << " a = " << a << ", b = " << b << '.' << std::endl;

    return false;
  }

  template<typename T>
  bool is_gt(const T& a, const T& b) {
    if (a > b) {
      return true;
    }

    set_status(FAILED);
    _report << color::RED << "\tFailed greater-than test!" << color::RESET << " a = " << a << ", b = " << b << '.' << std::endl;

    return false;
  }

  template<typename T>
  bool is_ge(const T& a, const T& b) {
    if (a >= b) {
      return true;
    }

    set_status(FAILED);
    _report << color::RED << "\tFailed greater-equal test!" << color::RESET << " a = " << a << ", b = " << b << '.' << std::endl;

    return false;
  }

  void fail() {
    set_status(FAILED);
  }

  void fail(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char *message;
    vasprintf(&message, fmt, args);

    set_status(FAILED);
    _report << color::RED << '\t' << message << color::RESET << std::endl;

    va_end(args);
    free(message);
  }

  void inconclusive() {
    set_status(INCONCLUSIVE);
  }

  void inconclusive(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char *message;
    vasprintf(&message, fmt, args);

    set_status(INCONCLUSIVE);
    _report << color::YELLOW << '\t' << message << color::RESET << std::endl;

    va_end(args);
    free(message);
  }

  bool meets_expectation(Expectation e) {
    switch (e) {
      case SHOULD_PASS: return _status == PASSED;
      case SHOULD_FAIL: return _status == FAILED;
      case SHOULD_BE_INCONCLUSIVE: return _status == INCONCLUSIVE;

      default:
        std::cerr << "Invalid `Expectation` " << e << std::endl;
        break;
    }
  }

  void report(const char *test_name, const char *test_file, int test_line) const {
    std::cout << test_file << ':' << test_line << ": " << test_name << "... ";

    switch (_status) {
      case PASSED:
        std::cout << color::GREEN << "Ok!" << color::RESET << std::endl;
        break;
      case INCONCLUSIVE:
        std::cout << color::YELLOW << "Inconclusive!" << color::RESET << std::endl;
        break;
      case FAILED:
        std::cout << color::RED << "Failed!" << color::RESET << std::endl;
        break;

      default:
        std::cerr << "Invalid `Status` " << _status << std::endl;
        break;
    }

    std::cout << _report.str();
  }

  void reset() {
    _status = PASSED;
    _report.str("");
  }

private:
  void set_status(Status status) {
    if (_status < status) {
      _status = status;
    }
  }

private:
  Status _status;
  std::stringstream _report;
};

struct Test {
  Expectation expectation;
  void (*fn)(Tester&);
  const char *name;
  const char *file;
  int line;
};

template<typename T>
int run_fixture_for() {
  const std::type_info& info = typeid(T);

  int demangle_status;
  const char *name = abi::__cxa_demangle(info.name(), NULL, NULL, &demangle_status);
  if (name == nullptr || demangle_status != 0) {
    return 0;
  } else {
    std::cout << color::CYAN << "Running " << name << "'s tests" << color::RESET << ':' << std::endl;
    free((void *)name);
  }

  auto tests = Fixture<T>::__tests__;
  size_t passed_tests = 0;
  Tester tester;
  for (auto&& test : tests) {
    tester.reset();

    test.fn(tester);
    tester.report(test.name, test.file, test.line);

    if (tester.meets_expectation(test.expectation)) {
      passed_tests++;
    } 
  }

  std::cout << std::endl;

  return passed_tests;
}

template<typename T, typename... Ts>
void run_tests() {
  std::cout << std::endl;

  size_t num_tests = Fixture<T>::__tests__.size();
  size_t passed_tests = run_fixture_for<T>();

  num_tests += (Fixture<Ts>::__tests__.size() + ... + 0);
  passed_tests += (run_fixture_for<Ts>() + ... + 0);

  auto color = passed_tests == num_tests ? color::GREEN : color::RED;
  std::cout << color << passed_tests << '/' << num_tests << " tests passed!" << color::RESET << std::endl << std::endl;
}

} // namespace test