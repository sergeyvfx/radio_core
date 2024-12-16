#!/usr/bin/env python3

# Copyright (c) 2024 radio core authors
#
# SPDX-License-Identifier: MIT-0

"""
Visualization of 2-channel WAV files FFT.

Expects to have WAV file with 2 channels: first channel is considered
to be in-hase signal (I) and the second channel is considered to be
quadrature signal (Q).

Example:
  ./plot_fft.py iq_samples.wav --size 65536

TODO(sergey): Implement intensity grading
Inspiration:  https://teaandtechtime.com/python-intensity-graded-fft-plots/
"""

import argparse
import matplotlib.animation
import matplotlib.artist
import matplotlib.collections
import matplotlib.lines
import matplotlib.pyplot as plt
import numpy as np
import struct
import wave
from pathlib import Path
from scipy import fft  # type: ignore
from typing import Iterable, Callable, Any


def create_argument_parser() -> argparse.ArgumentParser:
    """
    Create parser of command line arguments

    The parser will be configured to recognize all command line arguments, but
    no actual parsing will happen in this function.
    """

    parser = argparse.ArgumentParser(description="Constellation plotter")

    parser.add_argument(
        "wav_file",
        type=Path,
        help="Path to a wav file with IQ samples to plot",
    )

    parser.add_argument(
        "--size",
        type=int,
        help="Size of the FFT: the number of bins in the transform",
        default=65536,
    )

    parser.add_argument(
        "--transform",
        type=str,
        help="Transformation of the input samples before calculating FFT",
        choices=["none", "x**2", "x**4"],
        default="none",
    )

    return parser


# TODO(sergey): Find a way to make type more specific?
def decode_frames(wav_file: wave.Wave_read, frames: bytes) -> np.ndarray:
    """
    Decode frames read from WAV from binary form to a collection of tuples

    Returns an array with N frame-samples, each frame-sample contains 2 floating
    point values for I and Q values of that sample.
    """

    num_channels = wav_file.getnchannels()
    assert num_channels == 2

    # Calculate the number of frames from the raw bytes.
    # The sample_width is the size of a single sample, so every sample frame
    # consists of num_channels*sample_width bytes.
    sample_width = wav_file.getsampwidth()
    num_frames = len(frames) // num_channels // sample_width

    if sample_width == 1:
        # int8
        data = struct.unpack("<" + str(num_frames * 2) + "c", frames)
        flat_samples = np.array(data, dtype=np.float32) / 255
    elif sample_width == 2:
        # int16
        data = struct.unpack("<" + str(num_frames * 2) + "h", frames)
        flat_samples = np.array(data, dtype=np.float32) / 32767
    else:
        raise Exception(f"Unknown sample width {sample_width}")

    return np.c_[flat_samples[0::2], flat_samples[1::2]]


class PlotState:
    """
    Configuration and the current state of the plot
    """

    # Matplotlib's plot which is used for visualization.
    plot: matplotlib.lines.Line2D

    # The size of the FFT.
    size: int

    # Window which is applied on the samples to solve aliasing problem.
    window: np.ndarray

    # Frequencies in Hz which correspond to FFT bins.
    frequencies: np.ndarray

    # A function which transform samples according to the command line
    # configuration.
    transform: Callable[[Any], Any] = lambda x: x

    def __init__(
        self,
        wav_file: wave.Wave_read,
        plot: matplotlib.lines.Line2D,
        size: int,
        transform: str = "none",
    ) -> None:
        self.wav_file = wav_file
        self.plot = plot
        self.size = size

        sample_rate = wav_file.getframerate()
        self.frequencies = np.arange(
            sample_rate / -2.0, sample_rate / 2.0, sample_rate / self.size
        )

        self.window = np.hanning(self.size)

        if transform == "none":
            self.transform = lambda x: x
        elif transform == "x**2":
            self.transform = lambda x: x**2
        elif transform == "x**4":
            self.transform = lambda x: x**4

    def reset(self):
        """
        Reset the state of the plot

        Clear the current samples and rewind the file.
        """

        self.wav_file.rewind()


def update_plot(
    frame: int, plot_state: PlotState
) -> Iterable[matplotlib.artist.Artist]:
    """
    Function that updates plot, it is plot's animation callback

    It reads next samples from the file and plots them.
    """

    # Rewind the file on the first display frame.
    if frame == 0:
        plot_state.reset()

    # Read frames form the file and decode them into IQ samples.
    # Sample is an array of (I, Q).
    new_frames = plot_state.wav_file.readframes(plot_state.size)
    new_samples = decode_frames(plot_state.wav_file, new_frames)

    if len(new_samples) != plot_state.size:
        return (plot_state.plot,)

    iq_samples = new_samples[:, 0] + new_samples[:, 1] * 1j

    iq_samples = plot_state.transform(iq_samples)

    fft_linear = (
        fft.fftshift(fft.fft(iq_samples * plot_state.window, n=plot_state.size))
        / plot_state.size
    )
    fft_db = 10 * np.log10(np.abs(fft_linear))

    plot_state.plot.set_data(plot_state.frequencies, fft_db)

    return (plot_state.plot,)


def main() -> None:
    parser = create_argument_parser()

    args = parser.parse_args()
    wav_file = wave.open(str(args.wav_file), "rb")

    sample_rate = wav_file.getframerate()
    duration_sec = wav_file.getnframes() / wav_file.getframerate()

    # Print basic information.
    print("File information")
    print("================")
    print()
    print(f"Number of channels              : {wav_file.getnchannels()}")
    print(f"Sample width                    : {wav_file.getsampwidth()} bytes")
    print(f"Frame rate (sampling frequency) : {sample_rate} Hz")
    print(f"Number of frames                : {wav_file.getnframes()}")
    print(f"Compression type                : {wav_file.getcomptype()}")
    print(f"Duration                        : {duration_sec} sec")

    if wav_file.getnchannels() != 2:
        raise Exception("Can only visualize files with 2 channels")

    fig, ax_fft = plt.subplots(1, figsize=(12, 7))

    ax_fft.set_title("FFT")
    ax_fft.grid(True)
    ax_fft.set_xlim(-sample_rate / 2, sample_rate / 2)
    ax_fft.set_ylim(-120, 0)
    ax_fft.set_ylabel("Power (dB)")
    ax_fft.set_xlabel("Frequency (Hz)")
    (fft_plot,) = ax_fft.plot([], [], "r")

    plot_state = PlotState(
        wav_file=wav_file,
        plot=fft_plot,
        size=args.size,
        transform=args.transform,
    )

    fps = 24
    anim = matplotlib.animation.FuncAnimation(
        fig=fig,
        func=update_plot,
        frames=wav_file.getnframes() // args.size,
        interval=1.0 / fps * 1000,
        fargs=(plot_state,),
    )

    plt.show()


if __name__ == "__main__":
    main()
