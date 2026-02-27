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
from pathlib import Path
from scipy import fft  # type: ignore
from typing import Iterable, Callable, Any

from module.wav_reader import WavReader


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
        choices=["none", "x**2", "x**4", "x**8"],
        default="none",
    )

    return parser


class PlotState:
    """
    Configuration and the current state of the plot
    """

    # Reader of data from a WAV file.
    wav_reader: WavReader

    # Matplotlib's plot which is used for visualization.
    axes: matplotlib.axes.Axes
    plot: matplotlib.lines.Line2D

    # Matplotlib's Text object which is used to display text in a box.
    text_plot: matplotlib.text.Text

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
        wav_reader: WavReader,
        plot: matplotlib.lines.Line2D,
        text_plot: matplotlib.text.Text,
        size: int,
        transform: str = "none",
    ) -> None:
        self.wav_reader = wav_reader
        self.plot = plot
        self.text_plot = text_plot
        self.size = size

        sample_rate = wav_reader.get_sample_rate()
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
        elif transform == "x**8":
            self.transform = lambda x: x**8

    def reset(self):
        """
        Reset the state of the plot

        Clear the current samples and rewind the file.
        """

        self.wav_reader.rewind()


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
    new_samples = plot_state.wav_reader.read_num_samples(plot_state.size)

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

    plot_state.text_plot.set_text(f"Frame: {frame}")

    return (plot_state.plot,)


def main() -> None:
    parser = create_argument_parser()

    args = parser.parse_args()
    wav_reader = WavReader(args.wav_file)

    # Print basic information.
    print("File information")
    print("================")
    print()
    print(f"Number of channels  : {wav_reader.get_num_channels()}")
    print(f"Sample rate         : {wav_reader.get_sample_rate()} Hz")
    print(f"Sample type         : {wav_reader.get_sample_type()}")
    print(f"Duration            : {wav_reader.get_duration_sec()} sec")

    if wav_reader.get_num_channels() != 2:
        raise Exception("Can only visualize files with 2 channels")

    fig, ax_fft = plt.subplots(1, figsize=(12, 7))

    sample_rate = wav_reader.get_sample_rate()
    ax_fft.set_title("FFT")
    ax_fft.grid(True)
    ax_fft.set_xlim(-sample_rate / 2, sample_rate / 2)
    ax_fft.set_ylim(-120, 0)
    ax_fft.set_ylabel("Power (dB)")
    ax_fft.set_xlabel("Frequency (Hz)")
    (fft_plot,) = ax_fft.plot([], [], "r")

    text_plot = ax_fft.text(
        0.05,
        0.95,
        f"",
        transform=ax_fft.transAxes,
        fontsize=10,
        verticalalignment="top",
        bbox=dict(boxstyle="round", facecolor="lavender", alpha=0.7),
    )

    plot_state = PlotState(
        wav_reader=wav_reader,
        plot=fft_plot,
        text_plot=text_plot,
        size=args.size,
        transform=args.transform,
    )

    fps = 24
    anim = matplotlib.animation.FuncAnimation(
        fig=fig,
        func=update_plot,
        frames=wav_reader.get_num_samples() // args.size,
        interval=1.0 / fps * 1000,
        fargs=(plot_state,),
    )

    plt.show()


if __name__ == "__main__":
    main()
