# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

import time
import traceback
import sys

from .color_print import ColorPrint
from .report import Report
from .test import Test, TestSuit
from typing import Iterable


class _SuitTests:
    """
    Unrolled tests specification for a specific suit
    """

    suit: TestSuit
    tests: Iterable[Test]

    def __init__(self, suit: TestSuit, tests: Iterable[Test]):
        self.suit = suit
        self.tests = tests


class _RunStats:
    """
    Statistics of executed tests
    """

    num_run_tests: int
    num_run_suits: int
    num_passed_tests: int
    failed_tests: Iterable[str]
    run_time_ms: int

    def __init__(self):
        self.num_run_tests = 0
        self.num_run_suits = 0
        self.num_passed_tests = 0
        self.failed_tests = []
        self.run_time_ms = 0


class TestSuitRunner:
    """
    Class which takes care of running tests of provided suits
    """

    def __init__(self):
        self._stats = _RunStats()
        self._report = Report()

    # Printing helpers.
    #
    # Those are lines with common predefined prefix used for output. Takes care
    # of coloring of the output.
    #
    # TODO(sergey): COuld move some of shared logic to a more reusable functions
    # but not sure it will make code easier.

    def _print_info_h1(self, info: str):
        ColorPrint.print(
            f"{ColorPrint.GREEN}[==========]{ColorPrint.END} {info}"
        )
        ColorPrint.flush()

    def _print_info_h2(self, info: str):
        ColorPrint.print(
            f"{ColorPrint.GREEN}[----------]{ColorPrint.END} {info}"
        )
        ColorPrint.flush()

    def _print_run_start(self, info: str):
        ColorPrint.print(
            f"{ColorPrint.GREEN}[ RUN      ]{ColorPrint.END} {info}"
        )
        ColorPrint.flush()

    def _print_run_ok(self, info: str):
        ColorPrint.print(
            f"{ColorPrint.GREEN}[       OK ]{ColorPrint.END} {info}"
        )
        ColorPrint.flush()

    def _print_run_failed(self, info: str):
        ColorPrint.print(f"{ColorPrint.RED}[  FAILED  ]{ColorPrint.END} {info}")
        ColorPrint.flush()

    def _print_passed(self, info: str):
        ColorPrint.print(
            f"{ColorPrint.GREEN}[  PASSED  ]{ColorPrint.END} {info}"
        )
        ColorPrint.flush()

    def _print_failed(self, info: str):
        ColorPrint.print(f"{ColorPrint.RED}[  FAILED  ]{ColorPrint.END} {info}")
        ColorPrint.flush()

    # TODO(sergey): Find a better name for this function.
    def _test_word(self, number: int) -> str:
        """
        Get spelling of a 'test' word for the given number of tests

        Examples:
          _test_word(0) -> "tests"
          _test_word(1) -> "test"
          _test_word(10) -> "tests"
        """
        if number == 0 or number > 1:
            return "tests"
        return "test"

    def _collect_tests_to_run(
        self, suits_classes: Iterable[TestSuit]
    ) -> Iterable[_SuitTests]:
        num_suits = 0
        num_tests = 0

        all_suit_tests = []
        for suit_class in suits_classes:
            suit = suit_class()
            tests = suit.get_tests()

            if tests:
                all_suit_tests.append(_SuitTests(suit=suit, tests=tests))
                num_suits += 1
                num_tests += len(tests)

        self._print_info_h1(
            f"Running {num_tests} tests from {num_suits} test suite."
        )

        return all_suit_tests

    def _setup_global_environment(self, all_suit_tests: _SuitTests):
        self._print_info_h2("Global test environment set-up.")

        self._report.setup()

        for suit_test in all_suit_tests:
            suit_test.suit.setup()
            self._report.add_suit(suit_test.suit)

    def _teardown_global_environment(self, all_suit_tests: _SuitTests):
        self._print_info_h2("Global test environment tear-down")

        for suit_test in all_suit_tests:
            suit_test.suit.teardown()

        self._report.teardown()

    def _run_suit_test(self, suit: TestSuit, test: Test):
        """
        Run single test from a suit
        """

        full_test_name = f"{suit.get_name()}.{test.get_name()}"
        self._stats.num_run_tests += 1

        self._print_run_start(f"{full_test_name}")

        start_time = time.time()

        did_pass = False
        try:
            suit.run_test(self._report, test)
            did_pass = True
        except Exception as e:
            print(f"Failure: {e}")
            traceback.print_exc()
        run_time_ms = int((time.time() - start_time) * 1000)

        if did_pass:
            self._print_run_ok(f"{full_test_name} ({run_time_ms} ms)")
            self._stats.num_passed_tests += 1
        else:
            self._print_run_failed(f"{full_test_name} ({run_time_ms} ms)")
            self._stats.failed_tests.append(full_test_name)

        self._report.add_test_result(suit, test, did_pass)

    def _run_all_suit_tests(self, all_suit_tests: _SuitTests):
        """
        Run tests from all suits
        """

        suit = all_suit_tests.suit
        tests = all_suit_tests.tests

        num_tests = len(tests)
        suit_name = suit.get_name()

        self._print_info_h2(f"{num_tests} tests from {suit_name}")

        if not tests:
            return

        self._stats.num_run_suits += 1

        start_time = time.time()
        for test in tests:
            self._run_suit_test(suit, test)
        run_time_ms = int((time.time() - start_time) * 1000)

        self._print_info_h2(
            f"{num_tests} tests from {suit_name} ({run_time_ms} ms total)"
        )
        print()

    def _print_run_summary(self):
        """
        Print tests run summary

        Includes number of suits and tests run, total time, number of passed
        and failed tests, list of failed tests.
        """

        self._print_info_h1(
            f"{self._stats.num_run_tests} tests "
            f"from {self._stats.num_run_suits} test suite ran. "
            f"({self._stats.run_time_ms} ms total)"
        )

        num_passed_tests = self._stats.num_passed_tests
        self._print_passed(
            f"{num_passed_tests} {self._test_word(num_passed_tests)}."
        )

        num_failed_tests = len(self._stats.failed_tests)
        if num_failed_tests:
            self._print_failed(
                f"{num_passed_tests} "
                f"{self._test_word(num_passed_tests)}, listed below:"
            )

            for failed_test_name in self._stats.failed_tests:
                self._print_failed(f"{failed_test_name}")

            print()
            print(
                f" {num_failed_tests} FAILED "
                f"{self._test_word(num_failed_tests).upper()}"
            )

    def run(self, suits_classes: Iterable[TestSuit]):
        all_suit_tests = self._collect_tests_to_run(suits_classes)
        self._setup_global_environment(all_suit_tests)

        start_time = time.time()
        for suit_tests in all_suit_tests:
            self._run_all_suit_tests(suit_tests)
        self._stats.run_time_ms = int((time.time() - start_time) * 1000)

        self._teardown_global_environment(all_suit_tests)
        self._print_run_summary()

        self._report.print_footer()

        if self._stats.failed_tests:
            sys.exit(1)

    @classmethod
    def main(self, suits_classes: Iterable[TestSuit]):
        """
        Entry point for tests implementations
        """

        runner = TestSuitRunner()
        runner.run(suits_classes)
