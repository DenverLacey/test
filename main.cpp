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

FIXTURE(Timer) {
    TESTS {
        TEST("default constructor", [](Tester& tester) {
            auto t = Timer {};
            tester.is_eq(t._seconds, 0);
            tester.is_eq(t._minutes, 0);
            tester.is_eq(t._hours, 0);
        }),

        TEST("tick", [](Tester& tester) {
            auto t = Timer {};
            t._seconds = 59;
            t._minutes = 59;

            t.tick();

            tester.is_eq(t._seconds, 0);
            tester.is_eq(t._minutes, 0);
            tester.is_eq(t._hours, 1);
        }),

        TEST("reset", [](Tester& tester) {
            auto t = Timer {};
            t.tick();
            t.reset();

            tester.is_eq(t._seconds, 0);
            tester.is_eq(t._minutes, 0);
            tester.is_eq(t._hours, 0);
        }),

        TEST("seconds", [](Tester& tester) {
            auto t = Timer {};
            t._seconds = 11;
            int seconds = t.seconds();

            tester.is_eq(seconds, t._seconds);
        }),

        TEST("minutes", [](Tester& tester) {
            auto t = Timer {};
            t._minutes = 14;
            int minutes = t.minutes();

            tester.is_eq(minutes, t._minutes);
        }),

        TEST("hours", [](Tester& tester) {
            auto t = Timer {};
            t._hours = 19;
            int hours = t.hours();

            tester.is_eq(hours, t._hours);
        })
    };
};

FIXTURE(Tester) {
    TESTS {
        TEST("is_true with true", [](Tester& tester) {
            tester.is_true(true);
        }),

        TEST_EXPECT("is_true with false", SHOULD_FAIL, [](Tester& tester) {
            tester.is_true(false);
        }),

        TEST_EXPECT("is_false with true", SHOULD_FAIL, [](Tester& tester) {
            tester.is_false(true);
        }),

        TEST("is_false with false", [](Tester& tester) {
            tester.is_false(false);
        }),

        TEST_EXPECT("fail", SHOULD_FAIL, [](Tester& tester) {
            tester.fail();
        }),

        TEST_EXPECT("fail with message", SHOULD_FAIL, [](Tester& tester) {
            tester.fail("The variable is %d.", 42);
        }),

        TEST_EXPECT("inconclusive", SHOULD_BE_INCONCLUSIVE, [](Tester& tester) {
            tester.inconclusive();
        }),

        TEST_EXPECT("inconclusive with message", SHOULD_BE_INCONCLUSIVE, [](Tester& tester) {
            tester.inconclusive("*Is* the variable %d?", 42);
        }),

        TEST("is_null with null", [](Tester& tester) {
            tester.is_null(nullptr);
        }),

        TEST_EXPECT("is_null with ptr", SHOULD_FAIL, [](Tester& tester) {
            int d = 5;
            tester.is_null(&d);
        }),

        TEST_EXPECT("not_null with null", SHOULD_FAIL, [](Tester& tester) {
            tester.not_null(nullptr);
        }),

        TEST("not_null with ptr", [](Tester& tester) {
            int d = 5;
            tester.not_null(&d);
        }),

        TEST("is_eq with eq", [](Tester& tester) {
            tester.is_eq(0, 0);
        }),

        TEST_EXPECT("is_eq with ne", SHOULD_FAIL, [](Tester& tester) {
            tester.is_eq(0, 1);
        }),

        TEST_EXPECT("is_ne with eq", SHOULD_FAIL, [](Tester& tester) {
            tester.is_ne(0, 0);
        }),

        TEST("is_ne with ne", [](Tester& tester) {
            tester.is_ne(0, 1);
        }),

        TEST("is_lt with (0, 1)", [](Tester& tester) {
            tester.is_lt(0, 1);
        }),

        TEST_EXPECT("is_lt with (1, 1)", SHOULD_FAIL, [](Tester& tester) {
            tester.is_lt(1, 1);
        }),

        TEST_EXPECT("is_lt with (1, 0)", SHOULD_FAIL, [](Tester& tester) {
            tester.is_lt(1, 0);
        }),

        TEST("is_le with (0, 1)", [](Tester& tester) {
            tester.is_le(0, 1);
        }),

        TEST("is_le with (1, 1)", [](Tester& tester) {
            tester.is_le(1, 1);
        }),

        TEST_EXPECT("is_le with (1, 0)", SHOULD_FAIL, [](Tester& tester) {
            tester.is_le(1, 0);
        }),

        TEST_EXPECT("is_gt with (0, 1)", SHOULD_FAIL, [](Tester& tester) {
            tester.is_gt(0, 1);
        }),

        TEST_EXPECT("is_gt with (1, 1)", SHOULD_FAIL, [](Tester& tester) {
            tester.is_gt(1, 1);
        }),

        TEST("is_gt with (1, 0)", [](Tester& tester) {
            tester.is_gt(1, 0);
        }),

        TEST_EXPECT("is_ge with (0, 1)", SHOULD_FAIL, [](Tester& tester) {
            tester.is_ge(0, 1);
        }),

        TEST("is_ge with (1, 1)", [](Tester& tester) {
            tester.is_ge(1, 1);
        }),

        TEST("is_ge with (1, 0)", [](Tester& tester) {
            tester.is_ge(1, 0);
        }),
    };
};

} // namespace test

int main(int argc, const char **argv) {
    test::run_tests_for<Timer, test::Tester>();
    return 0;
}
