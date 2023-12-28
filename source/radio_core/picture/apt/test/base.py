# Copyright (c) 2023 radio core authors
#
# SPDX-License-Identifier: MIT


import argparse
import subprocess
import tempfile
from module.reference_image_test import (
    ReferenceImageTest,
    ReferenceImageTestSuit,
)
from pathlib import Path


class PictureAPTTest(ReferenceImageTest):
    base_name: str

    def __init__(self, base_name: str):
        super().__init__()

        self.fail_value_threshold = 1.0 / 255
        self.fail_percentage_threshold = 1

        self.base_name = base_name

    def get_name(self) -> str:
        return self.base_name


class PictureAPTDecodeTestSuit(ReferenceImageTestSuit):
    """
    APT picture decoding tests
    """

    def setup(self):
        super().setup()

        parser = argparse.ArgumentParser(description="Picture APT Test")

        parser.add_argument(
            "--apt_decoder",
            type=Path,
            help="Path to the apt_decoder application",
            required=True,
        )

        parser.add_argument(
            "--test_srcdir",
            type=Path,
            help="The location of data for this test",
            required=True,
        )

        args, unknown = parser.parse_known_args()

        self.apt_decoder = args.apt_decoder
        self.test_srcdir = args.test_srcdir

        assert self.apt_decoder.exists()
        assert self.test_srcdir.exists()

    def get_reference_image_filepath(self, test: ReferenceImageTest) -> Path:
        """
        Get filepath at which reference image for the given test is accessible
        """
        return self.test_srcdir / (test.base_name + "_reference.png")

    def read_actual_output_image(self, test: PictureAPTTest):
        apt_decoder = self.apt_decoder
        test_srcdir = self.test_srcdir

        wav_file = test_srcdir / (test.base_name + ".wav")
        assert wav_file.exists()

        with tempfile.TemporaryDirectory() as tmpdirname:
            # Configure the decoder command.
            command = [
                apt_decoder,
                "--format",
                "PNG",
                wav_file,
                tmpdirname,
            ]

            # Run the command.
            #
            # The STDERR is redirected to STDOUT and the STDOUT is captured.
            # This allows to have a single buffer which can be printed for
            # troubleshooting for the case the command fails.
            #
            # Note that the exit code check is done by this suit, so that extra
            # details are printed.
            x = subprocess.run(
                command,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
            )

            # Check the command exit code.
            if x.returncode:
                print(x.stdout.decode())
                self.raise_failure("Decoder exited with non-zero code")

            # List the output directory.
            # A single decoded file is expected.
            tmppath = Path(tmpdirname)
            files = [file for file in tmppath.iterdir()]

            assert len(files) == 1

            return self.read_image_from_file(files[0])
