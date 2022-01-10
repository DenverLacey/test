#pragma once

#include <cxxabi.h>
#include <initializer_list> 
#include <iostream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <typeinfo>
#include <vector>

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

  bool status_meets_expectation(Expectation e) {
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

struct Evaluation {
  size_t num_tests;
  std::vector<const Test*> failed_tests;

  void report() {
    if (failed_tests.empty()) {
      std::cout << color::GREEN << num_tests << "/" << num_tests << " tests passed!" << color::RESET << std::endl;
      return;
    }

    std::cout << color::RED << failed_tests.size() << "/" << num_tests << " tests failed!" << color::RESET << std::endl;

    for (const Test* test : failed_tests) {
      std::cout << color::RED << "---> " << color::RESET << test->file << ':' << test->line << ": " << test->name << '.' << std::endl;
    }
  }
};

template<typename T>
struct __report_beginning_of_fixture_evaluation__ {
	static void call() {
		const std::type_info& info = typeid(T);

		int demangle_status;
  	const char *name = abi::__cxa_demangle(info.name(), NULL, NULL, &demangle_status);
  	if (name == nullptr || demangle_status != 0) {
  	  return;
  	} else {
  	  std::cout << color::CYAN << "Running " << name << "'s tests" << color::RESET << ':' << std::endl;
  	  free((void *)name);
  	}
	}
};

template<typename T>
struct __report_beginning_of_fixture_evaluation__<Fixture<T>> {
	static void call() {
		const std::type_info& info = typeid(T);

		int demangle_status;
	  const char *name = abi::__cxa_demangle(info.name(), NULL, NULL, &demangle_status);
	  if (name == nullptr || demangle_status != 0) {
	    return;
	  } else {
	    std::cout << color::CYAN << "Running " << name << "'s tests" << color::RESET << ':' << std::endl;
	    free((void *)name);
	  }
	}
};

template<typename F>
void __run_fixtures__(Evaluation& evaluation) {
	__report_beginning_of_fixture_evaluation__<F>::call();

	auto tests = F::__tests__;
  Tester tester;
  for (const Test& test : tests) {
    tester.reset();

    test.fn(tester);
    tester.report(test.name, test.file, test.line);

    if (!tester.status_meets_expectation(test.expectation)) {
      evaluation.failed_tests.push_back(&test);
    } 
  }

  std::cout << std::endl;
}

template<typename F, typename... Fs>
void run_fixtures() {
	std::cout << std::endl;

  Evaluation evaluation;
  evaluation.num_tests = F::__tests__.size() + (Fs::__tests__.size() + ... + 0);
	
	__run_fixtures__<F>(evaluation);

	if constexpr (sizeof...(Fs) != 0) {
		__run_fixtures__<Fs...>(evaluation);
	}

  evaluation.report();
}

template<typename T, typename... Ts>
void run_tests_for() {
  run_fixtures<Fixture<T>, Fixture<Ts>...>();
}

} // namespace test