#include <iostream>

#include "test.h"

class Timer {
public:
  Timer() = default;
  Timer(const Timer&) = default;
  Timer(Timer&&) = default;

public:
  void reset() {
    _seconds = 0;
    _minutes = 0;
    _hours = 0;
  }

  void tick() {
    _seconds++;
    if (_seconds >= 60) {
      _seconds = 0;
      _minutes++;
      if (_minutes >= 60) {
        _minutes = 0;
        _hours++;
      }
    }
  }

  int seconds() const {
    return _seconds;
  }

  int minutes() const {
    return _minutes;
  }

  int hours() const {
    return _hours;
  }

private:
  int _seconds;
  int _minutes;
  int _hours;

  friend class test::Fixture<Timer>;
};

namespace test {

template<>
struct Fixture<Timer> {
  static void default_ctor(Tester& tester) {
    auto t = Timer {};
    tester.is_eq(t._seconds, 0);
    tester.is_eq(t._minutes, 0);
    tester.is_eq(t._hours, 0);
  }

  static void tick(Tester& tester) {
    auto t = Timer {};
    t._seconds = 59;
    t._minutes = 59;

    t.tick();

    tester.is_eq(t._seconds, 0);
    tester.is_eq(t._minutes, 0);
    tester.is_eq(t._hours, 1);
  }

  static void reset(Tester& tester) {
    auto t = Timer {};
    t.tick();
    t.reset();

    tester.is_eq(t._seconds, 0);
    tester.is_eq(t._minutes, 0);
    tester.is_eq(t._hours, 0);
  }

  static void seconds(Tester& tester) {
    auto t = Timer {};
    t._seconds = 11;
    int seconds = t.seconds();

    tester.is_eq(seconds, t._seconds);
  }

  static void minutes(Tester& tester) {
    auto t = Timer {};
    t._minutes = 14;
    int minutes = t.minutes();

    tester.is_eq(minutes, t._minutes);
  }

  static void hours(Tester& tester) {
    auto t = Timer {};
    t._hours = 19;
    int hours = t.hours();

    tester.is_eq(hours, t._hours);
  }

  REGISTER_TESTS {
    REGISTER_TEST(default_ctor),
    REGISTER_TEST(tick),
    REGISTER_TEST(reset),
    REGISTER_TEST(seconds),
    REGISTER_TEST(minutes),
    REGISTER_TEST(hours)
  };
};

template<>
struct Fixture<Tester> {
  static void is_true_with_true(Tester& tester) {
    tester.is_true(true);
  }

  static void is_true_with_false(Tester& tester) {
    tester.is_true(false);
  }

  static void is_false_with_true(Tester& tester) {
    tester.is_false(true);
  }

  static void is_false_with_false(Tester& tester) {
    tester.is_false(false);
  }

  static void fail(Tester& tester) {
    tester.fail();
  }

  static void fail_with_message(Tester& tester) {
    tester.fail("The variable is %d.", 42);
  }

  static void inconclusive(Tester& tester) {
    tester.inconclusive();
  }

  static void inconclusive_with_message(Tester& tester) {
    tester.inconclusive("*Is* the variable %d?", 42);
  }

  static void is_null_with_null(Tester& tester) {
    tester.is_null(nullptr);
  }

  static void is_null_with_ptr(Tester& tester) {
    int d = 5;
    tester.is_null(&d);
  }

  static void not_null_with_null(Tester& tester) {
    tester.not_null(nullptr);
  }

  static void not_null_with_ptr(Tester& tester) {
    int d = 5;
    tester.not_null(&d);
  }

  static void is_eq_with_eq(Tester& tester) {
    tester.is_eq(0, 0);
  }

  static void is_eq_with_ne(Tester& tester) {
    tester.is_eq(0, 1);
  }

  static void is_ne_with_eq(Tester& tester) {
    tester.is_ne(0, 0);
  }

  static void is_ne_with_ne(Tester& tester) {
    tester.is_ne(0, 1);
  }

  static void is_lt_with_0_1(Tester& tester) {
    tester.is_lt(0, 1);
  }

  static void is_lt_with_1_1(Tester& tester) {
    tester.is_lt(1, 1);
  }

  static void is_lt_with_1_0(Tester& tester) {
    tester.is_lt(1, 0);
  }

  static void is_le_with_0_1(Tester& tester) {
    tester.is_le(0, 1);
  }

  static void is_le_with_1_1(Tester& tester) {
    tester.is_le(1, 1);
  }

  static void is_le_with_1_0(Tester& tester) {
    tester.is_le(1, 0);
  }

  static void is_gt_with_0_1(Tester& tester) {
    tester.is_gt(0, 1);
  }

  static void is_gt_with_1_1(Tester& tester) {
    tester.is_gt(1, 1);
  }

  static void is_gt_with_1_0(Tester& tester) {
    tester.is_gt(1, 0);
  }

  static void is_ge_with_0_1(Tester& tester) {
    tester.is_ge(0, 1);
  }

  static void is_ge_with_1_1(Tester& tester) {
    tester.is_ge(1, 1);
  }

  static void is_ge_with_1_0(Tester& tester) {
    tester.is_ge(1, 0);
  }

  REGISTER_TESTS {
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_true_with_true),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_true_with_false),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_false_with_true),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_false_with_false),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_null_with_null),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_null_with_ptr),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, not_null_with_null),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, not_null_with_ptr),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_eq_with_eq),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_eq_with_ne),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_ne_with_eq),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_ne_with_ne),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_lt_with_0_1),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_lt_with_1_1),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_lt_with_1_0),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_le_with_0_1),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_le_with_1_1),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_le_with_1_0),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_gt_with_0_1),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_gt_with_1_1),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_gt_with_1_0),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, is_ge_with_0_1),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_ge_with_1_1),
    REGISTER_TEST_EXPECT(test::SHOULD_PASS, is_ge_with_1_0),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, fail),
    REGISTER_TEST_EXPECT(test::SHOULD_FAIL, fail_with_message),
    REGISTER_TEST_EXPECT(test::SHOULD_BE_INCONCLUSIVE, inconclusive),
    REGISTER_TEST_EXPECT(test::SHOULD_BE_INCONCLUSIVE, inconclusive_with_message),
  };
};

} // namespace test

int main(int argc, const char **argv) {
  test::run_tests<Timer, test::Tester>();
  return 0;
}
