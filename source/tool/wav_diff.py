#!/usr/bin/env python3

# Copyright (c) 2023 radio core authors
#
# SPDX-License-Identifier: MIT-0

"""
Utility to compare two WAV files

NOTE: Loads the WAV file in-memory.
"""

import argparse
import numpy
import sys

from pathlib import Path
from typing import Any
from scipy.io import wavfile


class WAV:
    """
    In-memory WAV file
    """

    sample_rate: int
    samples: Any

    def __init__(self):
        self.sample_rate = 0
        self.samples = []

    @classmethod
    def from_file(cls, file_name: Path) -> "WAV":
        wav = WAV()

        wav.sample_rate, wav.samples = wavfile.read(file_name)

        return wav

    def header_info(self) -> str:
        """
        One-line information about this WAV
        """

        num_samples = len(self.samples)
        duration = num_samples / float(self.sample_rate)

        return (
            f"sample rate: {self.sample_rate}, "
            f"{self.samples.dtype}, "
            f"number of samples: {num_samples}, "
            f"duration: {duration} sec"
        )

    def get_float_samples(self):
        """
        Get samples of this WAV file as floating point values
        """

        scale = 1

        data_type = self.samples.dtype

        if data_type == numpy.int16:
            scale = 32768.0
        else:
            raise f"Unknown scale for data type {data_type}"

        return self.samples.astype(numpy.float32) / scale


def create_argument_parser() -> argparse.ArgumentParser:
    """
    Create parser of command line arguments

    The parser will be configured to recognize all command line arguments, but
    no actual parsing will happen in this function.
    """

    parser = argparse.ArgumentParser(description="Histogram plotter")

    parser.add_argument(
        "wav_file_a",
        type=Path,
        help="First WAV file for the comparison",
    )

    parser.add_argument(
        "wav_file_b",
        type=Path,
        help="Second WAV file for the comparison",
    )

    return parser


def main() -> None:
    parser = create_argument_parser()

    args = parser.parse_args()

    # Read WAV files in-memory
    wav_a = WAV.from_file(args.wav_file_a)
    wav_b = WAV.from_file(args.wav_file_b)

    # Print basic information.
    print("Information")
    print("===========")
    print(f"WAV A: {wav_a.header_info()}")
    print(f"WAV B: {wav_b.header_info()}")

    # Check the samples are comparable.
    if len(wav_a.samples) != len(wav_b.samples):
        print("The WAV files have different number of samples")
        sys.exit(1)
    if wav_a.samples.dtype != wav_b.samples.dtype:
        print("The WAV files have sample type")
        sys.exit(1)

    all_difference = wav_a.get_float_samples() - wav_b.get_float_samples()

    # TODO(sergey): Does this work for stereo WAV?
    difference = all_difference[all_difference != 0]

    num_different = len(difference)
    percent_different = (
        int(num_different / float(len(wav_a.samples)) * 10000) / 100
    )

    abs_difference = difference
    max_difference = numpy.max(abs_difference) if len(abs_difference) else 0

    mean_difference = numpy.average(max_difference)
    difference_variance = numpy.var(max_difference)

    print()
    print("Statistics")
    print("==========")

    print(
        f"Number of different samples : {num_different} ({percent_different}%)"
    )
    print(
        f"Maximum difference          : {max_difference} "
        f"(int16:{max_difference * 32768})"
    )
    print(f"Mean difference             : {mean_difference}")
    print(f"Difference variance         : {difference_variance}")


if __name__ == "__main__":
    main()
