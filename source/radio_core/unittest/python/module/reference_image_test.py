# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

import os
import numpy

from .image import Image
from .report import Report
from .test import Test, TestSuit
from pathlib import Path


class ReferenceImageTest(Test):
    """
    Base class for test which use reference image for validness check
    """

    # Threshold in difference between pixel values which is considered a
    # failure at that pixel.
    fail_value_threshold: float

    # Threshold in percentage of failed pixels over entire number of pixels
    # which makes test to fail.
    fail_percentage_threshold: float

    def __init__(self):
        self.fail_value_threshold = 1e-6
        self.fail_percentage_threshold = 0


class ReferenceImageTestSuit(TestSuit):
    """
    Base class for test suits which use reference image for validness check
    """

    def __init__(self):
        super().__init__()

        # When actual image is too different from reference don't consider it a
        # failure but, instead, update the reference file.
        # Used when test is failed upon expected changes.
        self._update_reference_on_failure = False

    def setup(self):
        super().setup()

        self._update_reference_on_failure = os.environ.get(
            "UPDATE_REFERENCE_ON_FAILURE", 0
        )

    def read_image_from_file(self, filepath: Path) -> Image:
        """
        Shortcut for reading image from file
        """

        return Image.read_from_file(filepath)

    def run_test(self, report: Report, test: ReferenceImageTest):
        """
        Entry point to run test and check its results
        """

        if report.is_enabled():
            data_dir = report.get_test_data_dir(self, test)
            data_dir.mkdir(exist_ok=True)

        actual_image = self.read_actual_output_image(test)
        if report.is_enabled():
            actual_image.save_to_file(data_dir / "actual.png")

        reference_image_filepath = self.get_reference_image_filepath(test)

        try:
            reference_image = self.read_image_from_file(
                reference_image_filepath
            )
        except FileNotFoundError as e:
            if self._update_reference_on_failure:
                self._update_reference_image(report, test, actual_image)
                return
            else:
                raise e

        if report.is_enabled():
            reference_image.save_to_file(data_dir / "reference.png")
        self._compare_image_or_fail(
            report,
            test,
            actual_image=actual_image,
            reference_image=reference_image,
        )

    def get_reference_image_filepath(self, test: ReferenceImageTest) -> Path:
        """
        Get filepath at which reference image for the given test is accessible
        """
        self.raise_failure("Not implemented")

    def read_actual_output_image(self, test: ReferenceImageTest) -> Image:
        """
        Read actual image which the given test outputs

        Usually this also involves running an external command of some sort.
        So typical implementation of this method will construct a command to be
        executed, watch for it to be run with zero exit code and read image from
        an output location of the command.
        """
        self.raise_failure("Not implemented")

    def _compare_image_or_fail(
        self,
        report: Report,
        test: ReferenceImageTest,
        actual_image: Image,
        reference_image: Image,
    ):
        """
        Test comparison logic which will pass if images are almost equal

        If the image is too different from expected reference test fill be
        marked as failed.
        """

        if actual_image.pixels.shape != reference_image.pixels.shape:
            if self._update_reference_on_failure:
                self._update_reference_image(report, test, actual_image)
                return
            else:
                self.raise_failure(
                    "Mismatch in action and reference shape "
                    f"{actual_image.pixels.shape} vs {reference_image.pixels.shape}"
                )

        difference = numpy.absolute(
            actual_image.pixels - reference_image.pixels
        )
        flatten_difference = difference.flatten()

        if report.is_enabled():
            data_dir = report.get_test_data_dir(self, test)

            # TODO(sergey): Normalize values.
            difference_image = Image(difference)

            max_value = flatten_difference.max()
            if max_value != 0:
                difference_image.pixels *= 1.0 / max_value

            difference_image.save_to_file(data_dir / "difference.png")

        num_accepted_failed_pixels = int(
            float(len(flatten_difference))
            * test.fail_percentage_threshold
            / 100
        )

        num_failed_pixels = (
            flatten_difference > test.fail_value_threshold
        ).sum()

        if num_failed_pixels > num_accepted_failed_pixels:
            if self._update_reference_on_failure:
                self._update_reference_image(report, test, actual_image)
            else:
                self.raise_failure(
                    f"Too many failed pixel values: {num_failed_pixels} "
                    f"at threshold {num_accepted_failed_pixels}"
                )

    def _update_reference_image(
        self, report: Report, test: ReferenceImageTest, actual_image: Image
    ):
        reference_image_filepath = self.get_reference_image_filepath(test)
        actual_image.save_to_file(reference_image_filepath)

        if report.is_enabled():
            data_dir = report.get_test_data_dir(self, test)
            actual_image.save_to_file(data_dir / "reference.png")

            difference_image = Image(
                numpy.ndarray(actual_image.pixels.shape, dtype=float)
            )
            difference_image.save_to_file(data_dir / "difference.png")

    ############################################################################
    # Report.

    def report_generate_table_header_code(self) -> str:
        return (
            "<tr>"
            "<th>Test</th>"
            '<th width="5%">Result</th>'
            '<th width="25%">Output</th>'
            '<th width="25%">Reference</th>'
            '<th width="25%">Difference</th>'
            "</tr>"
        )

    def report_generate_test_row_code(
        self, report: Report, test: Test, did_pass: bool
    ) -> str:
        tr_class = "passed" if did_pass else "failed"
        result = "PASSED" if did_pass else "FAILED"

        data_dir = report.get_test_data_relative_dir(test)
        actual_src = f"{data_dir}/actual.png"
        reference_src = f"{data_dir}/reference.png"
        difference_src = f"{data_dir}/difference.png"

        return (
            f'<tr class="{tr_class}">'
            f"<td>{test.get_name()}</td>"
            f"<td><b>{result}</b></td>"
            f'<td><a href="{actual_src}"><img src="{actual_src}"></a></td>'
            f'<td><a href="{reference_src}"><img src="{reference_src}"></a></td>'
            f'<td><a href="{difference_src}"><img src="{difference_src}"></a></td>'
            "<tr>"
        )
