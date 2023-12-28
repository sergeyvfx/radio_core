# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

"""
Report generator for the test results

Works together with the test runner and generates an easy-to-explore HTML
page with the test results.
"""

import argparse
import json
import os
import shutil

from pathlib import Path
from typing import Iterable, Optional
from .test import Test, TestSuit


class _TestResult:
    # Original name of the test.
    name: str

    # Boolean flag indicating whether test passed
    passed: bool

    # Generated report for this test.
    report_html: str


class _SuitResult:
    # Original name of the suit.
    name: str

    # Boolean flag indicating whether suit as a whole did pass.
    # The suit passes if all its tests did pass
    passed: bool

    tests: Iterable[_TestResult]

    def __init__(self):
        self.name = ""
        self.passed = True
        self.tests = []


class _SuitResultEncoder(json.JSONEncoder):
    """
    Encoder of the _SuitResult class for JSON serialization
    """

    def default(self, result):
        if isinstance(result, _SuitResult):
            return {
                "name": result.name,
                "passed": result.passed,
                "tests": result.tests,
            }
        elif isinstance(result, _TestResult):
            return {
                "name": result.name,
                "passed": result.passed,
                "report_html": result.report_html,
            }
        else:
            return super().default(result)


class _HTMLTemplate:
    html: str

    def __init__(self):
        self.html = ""

    @classmethod
    def read_from_file(cls, filepath: Path) -> "_HTMLTemplate":
        template = _HTMLTemplate()
        template.html = filepath.read_text()
        return template

    def write_to_file(self, filepath: Path):
        filepath.write_text(self.html)

    def substitute_variable(
        self, variable_name: str, variable_value: str
    ) -> str:
        self.html = self.html.replace("{" + variable_name + "}", variable_value)


def _as_suit_result(d: dict) -> _SuitResult:
    """
    De-serializer of a dict-type object into _SuitResult
    """

    suit_result = _SuitResult()
    suit_result.name = d["name"]
    suit_result.passed = d["passed"]

    for test in d.get("tests", []):
        test_result = _TestResult()
        test_result.name = test["name"]
        test_result.passed = test["passed"]
        test_result.report_html = test["report_html"]
        suit_result.tests.append(test_result)

    return suit_result


def _has_suit_failure(all_suit_results: Iterable[_SuitResult]) -> bool:
    """
    Check whether there is any suit which failed
    """
    for suit_result in all_suit_results:
        if not suit_result.passed:
            return True
    return False


def _name_to_filename(name: str) -> str:
    """
    Convert suit or test name to a filename-safe notation

    All the unsafe characters are replaced with underscore.
    """

    filename = ""

    for c in name:
        if c.isalnum():
            filename += c
        else:
            filename += "_"

    return filename


class Report:
    def __init__(self):
        self.report_rootdir = None
        self.suits = []

    ############################################################################
    # Global configuration.

    def is_enabled(self) -> bool:
        """
        Check whether reporting system is enabled
        """
        return self.report_rootdir is not None

    def setup(self):
        """
        Global setup of the report

        Called before any suit is setup.
        """

        parser = argparse.ArgumentParser(description="Histogram plotter")

        parser.add_argument(
            "--report_rootdir",
            type=Path,
            help="Root directory where report files will be stored",
        )

        args, unknown = parser.parse_known_args()

        self.report_rootdir = args.report_rootdir

        # When the reporting is enabled ensure the outout root directory exists.
        if self.is_enabled():
            self.report_rootdir.mkdir(exist_ok=True)

    def teardown(self):
        """
        Global teardown of the report

        Called after all suits are teareddown.
        """

        self._generate_report()

    def print_footer(self):
        report_filename = self.report_rootdir / "report.html"

        print()
        print(f"Tests report has been saved to {report_filename}")

    ############################################################################
    # Template directory.

    def _get_template_dir(self) -> Path:
        """
        Get directory with templates
        """
        script_filename = os.path.realpath(__file__)

        return Path(script_filename).parent / "template"

    ############################################################################
    # Suit directories and data access.

    def get_suit_report_relative_dir(self, suit: TestSuit) -> Path:
        """
        Path to directory with suit's report relative to the root
        """
        return _name_to_filename(suit.get_name())

    def get_suit_report_dir(self, suit: TestSuit) -> Path:
        """
        Get directory in which suit report is generated
        """

        return self.report_rootdir / self.get_suit_report_relative_dir(suit)

    def _get_suit_result_filename(self, suit: TestSuit) -> Path:
        """
        Get name of a while which contains serialized information about the suit
        """

        return self.get_suit_report_dir(suit) / "result.json"

    def _read_suit_result_from_file(self, filepath: Path) -> _SuitResult:
        json_str = filepath.read_text()
        return _as_suit_result(json.loads(json_str))

    def _read_suit_result_from_suit_directory(
        self, dirpath: Path
    ) -> Optional[_SuitResult]:
        json_path = dirpath / "result.json"
        if not json_path.exists():
            return None
        return self._read_suit_result_from_file(json_path)

    def _read_suit_result(self, suit: TestSuit) -> _SuitResult:
        result_filename = self._get_suit_result_filename(suit)
        if not result_filename.exists():
            suit_result = _SuitResult()
            suit_result.name = suit.get_name()
            return suit_result

        return self._read_suit_result_from_file(result_filename)

    def _write_suit_result(self, suit: TestSuit, result: _SuitResult):
        result_filename = self._get_suit_result_filename(suit)

        # Write to a temp file and rename, which is more likely to guarantee
        # atomic access for another process which might be running in parallel
        # (with `ctest -j` for example).
        result_tmp_filename = result_filename.with_suffix(
            result_filename.suffix + ".tmp"
        )
        result_tmp_filename.write_text(
            json.dumps(result, cls=_SuitResultEncoder)
        )

        os.rename(result_tmp_filename, result_filename)

    ############################################################################
    # Tests directories and data access.

    def get_test_data_relative_dir(self, test: Test) -> Path:
        """
        Path to directory with tests' data relative to the suit report directory
        """
        return _name_to_filename(test.get_name())

    def get_test_data_dir(self, suit: TestSuit, test: Test) -> Path:
        """
        Path to directory with tests' data.
        """
        return (
            self.report_rootdir
            / self.get_suit_report_relative_dir(suit)
            / self.get_test_data_relative_dir(test)
        )

    ############################################################################
    # Registration of suits and test results.

    def add_suit(self, suit: TestSuit):
        """
        Add suit which tests will be run during this test session
        """

        if not self.is_enabled():
            return

        # Ensure the suit report directory is empty.
        # This will make it so all stale test results are removed.
        #
        # NOTE: This assumes that no test filter is in use. If there is filter
        # applied the removal of the entire folder might not be desirable.

        suit_report_dir = self.get_suit_report_dir(suit)

        if suit_report_dir.exists():
            assert suit_report_dir.is_dir()
            shutil.rmtree(suit_report_dir)

        suit_report_dir.mkdir(exist_ok=True)

        self.suits.append(suit)

    def add_test_result(self, suit: TestSuit, test: Test, did_pass: bool):
        """
        Add test run result to the report
        """

        if not self.is_enabled():
            return

        test_result = _TestResult()
        test_result.name = test.get_name()
        test_result.passed = did_pass
        test_result.report_html = suit.report_generate_test_row_code(
            self, test, did_pass
        )

        suit_result = self._read_suit_result(suit)
        suit_result.passed = suit_result.passed and did_pass
        suit_result.tests.append(test_result)

        self._write_suit_result(suit, suit_result)

    ############################################################################
    # Entry point to report generation.

    def _generate_report(self):
        self._generate_main_report()
        self._generate_suits_reports()

    ############################################################################
    # Top-level report
    #
    # Gives overview of suits status.

    def _gather_all_suit_results(self) -> Iterable[_SuitResult]:
        all_suit_results = []
        for file in self.report_rootdir.iterdir():
            if not file.is_dir():
                continue
            suit_result = self._read_suit_result_from_suit_directory(file)
            if suit_result:
                all_suit_results.append(suit_result)
        all_suit_results.sort(key=lambda suit_result: suit_result.name)
        return all_suit_results

    def _generate_main_report(self):
        """
        Generate main "hub" of report
        """

        if not self.is_enabled():
            return

        # TODO(sergey): Make this more reliable for cases when multiple
        # tests are writing to the same report.

        template_dir = self._get_template_dir()
        html = _HTMLTemplate.read_from_file(template_dir / "report.html")

        all_suit_results = self._gather_all_suit_results()

        # Title.
        html.substitute_variable("TITLE", "Test Suits Overview")

        # Overall status.
        html.substitute_variable(
            "OVERALL-RESULT",
            self._generate_suit_overall_result_code(all_suit_results),
        )

        # Table header.
        html.substitute_variable(
            "SUIT-RESULT-HEADER",
            '<tr><th>Suit</th><th width="100">Result</th></tr>',
        )

        # Per-suit row.
        html.substitute_variable(
            "SUIT-RESULT-ROWS",
            self._generate_suit_result_rows(all_suit_results),
        )

        # Write the report to disk.
        report_filename = self.report_rootdir / "report.html"
        html.write_to_file(report_filename)

    def _generate_suit_overall_result_code(
        self, all_suit_results: Iterable[_SuitResult]
    ) -> str:
        overall_result_str = (
            "FAILED" if _has_suit_failure(all_suit_results) else "PASSED"
        )
        return (
            f'<span class="{overall_result_str.lower()}">'
            f"{overall_result_str}</span>"
        )

    def _generate_suit_result_rows(
        self, all_suit_results: Iterable[_SuitResult]
    ) -> str:
        rows = ""
        for suit_result in all_suit_results:
            rows += self._generate_suit_result_row(suit_result)
        return rows

    def _generate_suit_result_row(self, suit_result: _SuitResult) -> str:
        row_class = "passed" if suit_result.passed else "failed"
        report_dir = _name_to_filename(suit_result.name)
        suit_report_href = f"{report_dir}/report.html"
        return (
            f'<tr class="{row_class}">'
            f'<td><a href="{suit_report_href}">{suit_result.name}</a></td>'
            f'<td class="result">{row_class.upper()}</td>'
            "</tr>"
        )

    ############################################################################
    # Suit report generation.
    #
    # Gives overview of tests of a specific suit.

    def _generate_suits_reports(self):
        """
        Generate report for all suits which were run
        """

        for suit in self.suits:
            self._generate_suit_report(suit)

    def _generate_suit_report(self, suit: TestSuit):
        template_dir = self._get_template_dir()
        html = _HTMLTemplate.read_from_file(template_dir / "report.html")

        suit_result = self._read_suit_result(suit)

        # Title.
        html.substitute_variable("TITLE", suit.get_name())

        # Overall result.
        html.substitute_variable(
            "OVERALL-RESULT",
            self._generate_test_overall_result_code(suit_result),
        )

        # Table header.
        html.substitute_variable(
            "SUIT-RESULT-HEADER",
            suit.report_generate_table_header_code(),
        )

        # Table rows
        rows = []
        for test_result in suit_result.tests:
            rows.append(test_result.report_html)
        html.substitute_variable("SUIT-RESULT-ROWS", "\n".join(rows))

        # Write the report to disk.
        suit_report_dir = self.get_suit_report_dir(suit)
        html.write_to_file(suit_report_dir / "report.html")

    def _generate_test_overall_result_code(
        self, suit_result: _SuitResult
    ) -> str:
        overall_result_str = "PASSED" if suit_result.passed else "FAILED"
        return (
            f'<span class="{overall_result_str.lower()}">'
            f"{overall_result_str}</span>"
        )
