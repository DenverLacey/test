#pragma once

#include <cxxabi.h>
#include <initializer_list> 
#include <iostream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <typeinfo>

#define REGISTER_TESTS static constexpr std::initializer_list<Test> __tests__ = 
#define REGISTER_TEST(test) { SHOULD_PASS, test, #test }
#define REGISTER_TEST_EXPECT(expectation, test) { expectation, test, #test }

namespace test {

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
    _report << "\t\e[31mExpected `true`.\e[0m" << std::endl;

    return false;
  }

  bool is_false(bool test) {
    if (!test) {
      return true;
    }

    set_status(FAILED);
    _report << "\t\e[31mExpected `false`.\e[0m" << std::endl;
    
    return false;
  }

  bool is_null(const void *ptr) {
    if (ptr == nullptr) {
      return true;
    }

    set_status(FAILED);
    _report << "\t\e[31mExpected `nullptr` but Actual = " << ptr << ".\e[0m" << std::endl;

    return false;
  }

  bool not_null(const void *ptr) {
    if (ptr != nullptr) {
      return true;
    }

    set_status(FAILED);
    _report << "\t\e[31mUnexpected `nullptr`.\e[0m" << std::endl;

    return false;
  }

  template<typename T>
  bool is_eq(const T& actual, const T& expected) {
    if (actual == expected) {
      return true;
    }
    
    set_status(FAILED);
    _report << "\t\e[31mFailed equality!\e[0m Expected = " << expected << ", Actual = " << actual << '.' << std::endl;

    return false;
  }

  template<typename T>
  bool is_ne(const T& actual, const T& invalid) {
    if (actual != invalid) {
      return true;
    }

    set_status(FAILED);
    _report << "\t\e[31mFailed non-equality!\e[0m Actual value (" << actual << ") was invalid value (" << invalid << ")." << std::endl;

    return false;
  }

  template<typename T>
  bool is_lt(const T& a, const T& b) {
    if (a < b) {
      return true;
    }

    set_status(FAILED);
    _report << "\t\e[31mFailed less-than test!\e[0m a = " << a << ", b = " << b << '.' << std::endl;

    return false;
  }

  template<typename T>
  bool is_le(const T& a, const T& b) {
    if (a <= b) {
      return true;
    }

    set_status(FAILED);
    _report << "\t\e[31mFailed less-equal test!\e[0m a = " << a << ", b = " << b << '.' << std::endl;

    return false;
  }

  template<typename T>
  bool is_gt(const T& a, const T& b) {
    if (a > b) {
      return true;
    }

    set_status(FAILED);
    _report << "\t\e[31mFailed greater-than test!\e[0m a = " << a << ", b = " << b << '.' << std::endl;

    return false;
  }

  template<typename T>
  bool is_ge(const T& a, const T& b) {
    if (a >= b) {
      return true;
    }

    set_status(FAILED);
    _report << "\t\e[31mFailed greater-equal test!\e[0m a = " << a << ", b = " << b << '.' << std::endl;

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
    _report << "\t\e[31m" << message << "\e[0m" << std::endl;

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
    _report << "\t\e[33m" << message << "\e[0m" << std::endl;

    va_end(args);
    free(message);
  }

  bool expectation_met(Expectation e) {
    switch (e) {
      case SHOULD_PASS: return _status == PASSED;
      case SHOULD_FAIL: return _status == FAILED;
      case SHOULD_BE_INCONCLUSIVE: return _status == INCONCLUSIVE;

      default:
        std::cerr << "Invalid `Expectation` " << e << std::endl;
        break;
    }
  }

  void report(const char *test_name) const {
    std::cout << test_name << "... ";

    switch (_status) {
      case PASSED:
        std::cout << "\e[32mOk!\e[0m" << std::endl;
        break;
      case INCONCLUSIVE:
        std::cout << "\e[33mInconclusive!\e[0m" << std::endl;
        break;
      case FAILED:
        std::cout << "\e[31mFailed!\e[0m" << std::endl;
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
};

template<typename T>
int run_fixture_for() {
  const std::type_info& info = typeid(T);

  int demangle_status;
  const char *name = abi::__cxa_demangle(info.name(), NULL, NULL, &demangle_status);
  if (name == nullptr || demangle_status != 0) {
    return 0;
  } else {
    std::cout << "\e[36mRunning " << name << "'s tests\e[0m:" << std::endl;
    free((void *)name);
  }

  auto tests = Fixture<T>::__tests__;
  size_t passed_tests = 0;
  Tester tester;
  for (auto&& test : tests) {
    tester.reset();

    test.fn(tester);
    tester.report(test.name);

    if (tester.expectation_met(test.expectation)) {
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

  auto color = passed_tests == num_tests ? "\e[32m" : "\e[31m";
  std::cout << color << passed_tests << '/' << num_tests << " tests passed!\e[0m" << std::endl << std::endl;
}

} // namespace test