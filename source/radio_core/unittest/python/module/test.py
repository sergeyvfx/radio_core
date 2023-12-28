# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0


class TestFailureException(Exception):
    pass


class Test:
    """
    Base class for all test definitions

    The test contains data needed bu a suit to know what it needs to run and the
    actual run logic is implemented on the suit level.
    """

    def get_name(self) -> str:
        """
        Name of the suit which will appear in the run logs
        """

        return type(self).__name__


class TestSuit:
    """
    Base class for all test suits
    """

    def get_name(self) -> str:
        """
        Name of the suit which will appear in the run logs
        """

        return type(self).__name__

    ############################################################################
    # Global configuration.

    def setup(self):
        """
        Setup the suit for execution of its tests

        Run one per tests run before any tests of this suit are run.
        Allows to prepare a common environment for all tests which belong to
        this suit.
        """

        pass

    def teardown(self):
        """
        Teardown the suit after all tests are run
        """

        pass

    ############################################################################
    # Test execution.

    # TODO(sergey): How to get Report type without causing circular dependency?
    def run_test(self, report, test: Test):
        """
        Run given test fixture

        In the case of a test failure a TestFailureException exception is risen
        with details in its message

        At this point the report is used to access storage directory of the test
        for the case the test needs to copy some data over for the report.
        """

        raise TestFailureException("Not implemented")

    def raise_failure(self, details: str):
        """
        Raise a test failure

        Used from cases where non-exception-based check detected a test failure.
        """

        raise TestFailureException(details)

    ############################################################################
    # Report.

    def report_generate_table_header_code(self) -> str:
        """
        Generate table header code for report generator
        """

        assert "Not implemented"

    # TODO(sergey): How to get Report type without causing circular dependency?
    def report_generate_test_row_code(
        self, report, test: Test, did_pass: bool
    ) -> str:
        """
        Generate code for test report which goes into table row

        The row (tr) code is to be included.
        """

        assert "Not implemented"
