#!/usr/bin/env python3

# Copyright (c) 2024 radio core authors
#
# SPDX-License-Identifier: MIT-0

"""
Visualization of 2-channel WAV files as constellation diagram.

Expects to have WAV file with 2 channels: first channel is considered
to be in-hase signal (I) and the second channel is considered to be
quadrature signal (Q).

Example:
  ./plot_constellation.py iq_samples.wav --size 4096 --block-size 512 --scale 0.03
"""

import argparse
import matplotlib.animation
import matplotlib.collections
import matplotlib.artist
import matplotlib.pyplot as plt
import numpy as np
import struct
import wave
from pathlib import Path
from scipy import stats  # type: ignore
from typing import Iterable

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
        "--scale",
        type=float,
        help="Scale of the input signal",
        default=1.0,
    )

    parser.add_argument(
        "--size",
        type=int,
        help="The maximum number of samples displayed in the constellation",
        default=2048,
    )

    parser.add_argument(
        "--block-size",
        type=int,
        help="The number of frame-samples read on every plot redraw",
        default=2048,
    )

    return parser


class PlotState:
    """
    Configuration and the current state of the plot
    """

    # Reader of data from a WAV file.
    wav_reader: WavReader

    # Matplotlib's axes and plot which is used for visualization.
    axes: matplotlib.axes.Axes
    plot: matplotlib.collections.PathCollection

    # Matplotlib's Text object which is used to display text in a box.
    text_plot: matplotlib.text.Text

    # The number of new sample-frames read from file on every redraw.
    block_size: int

    # The maximum number of samples displayed in the plot.
    constellation_size: int

    # Currently displayed samples.
    # Stored as an array of (I, Q) elements.
    samples: np.ndarray

    def __init__(
        self,
        wav_reader: WavReader,
        axes: matplotlib.axes.Axes,
        plot: matplotlib.collections.PathCollection,
        text_plot: matplotlib.text.Text,
        block_size: int,
        constellation_size: int,
    ) -> None:
        self.wav_reader = wav_reader
        self.axes = axes
        self.plot = plot
        self.text_plot = text_plot
        self.block_size = block_size
        self.constellation_size = constellation_size
        self.samples = np.array([], dtype=np.float32)

    def reset(self):
        """
        Reset the state of the plot

        Clear the current samples and rewind the file.
        """

        self.wav_reader.rewind()
        self.samples = np.array([], dtype=np.float32)

    def push_samples(self, new_samples: np.ndarray) -> None:
        """
        Push new samples to the plot

        Maintains the maximum number of samples so they don't exceed the
        constellation_size.

        The new samples are expected to be an array of (I, Q) elements.
        """

        if len(self.samples) == 0:
            self.samples = new_samples
        else:
            self.samples = np.concatenate((self.samples, new_samples))

        num_samples_to_pop = len(self.samples) - self.constellation_size
        if num_samples_to_pop > 0:
            self.samples = self.samples[num_samples_to_pop:]


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
    new_samples = plot_state.wav_reader.read_num_samples(plot_state.block_size)

    plot_state.push_samples(new_samples)

    # Convert samples to a stack: [[I1, I2, ...], [Q1, Q2, ...]].
    samples_stack = np.vstack(
        [plot_state.samples[:, 0], plot_state.samples[:, 1]]
    )

    # Calculate density of the IQ samples.
    #
    # Add dither to the input to work-around cases when the dataset to avoid
    # it having highly dependent features, and hereby also avoid gaussian_kde
    # throwing an exception in such cases.
    num_samples = len(plot_state.samples)
    dither_x = np.random.normal(size=num_samples) * 1e-5
    dither_y = np.random.normal(size=num_samples) * 1e-5
    kde = stats.gaussian_kde(
        [samples_stack[0] + dither_x, samples_stack[1] + dither_y]
    )
    density = kde(samples_stack)

    # Sort samples by density, so they are displayed more nicely.
    idx = density.argsort()
    samples, density = plot_state.samples[idx], density[idx]

    # Normalize the density.
    # The matplotlib seems to track the min/max, even across the frames,
    # which might lead to different render results if no normalization is
    # done here depending on the block size.
    density -= density[0]
    density /= density[-1]

    plot_state.plot.set_offsets(samples)
    plot_state.plot.set_array(density)

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

    constellation_scale = args.scale

    fig, ax_const = plt.subplots(1, figsize=(7, 7))

    ax_const.set_title("Constellation")
    ax_const.grid(True)
    ax_const.set_xlim(-constellation_scale, constellation_scale)
    ax_const.set_ylim(-constellation_scale, constellation_scale)
    ax_const.set_xlabel("I")
    ax_const.set_ylabel("Q")
    scatter_plot = ax_const.scatter([], [], c=[], cmap="viridis", alpha=0.7)

    text_plot = ax_const.axes.text(
        0.05,
        0.95,
        f"",
        transform=ax_const.axes.transAxes,
        fontsize=10,
        verticalalignment="top",
        bbox=dict(boxstyle="round", facecolor="lavender", alpha=0.7),
    )

    plot_state = PlotState(
        wav_reader=wav_reader,
        axes=ax_const,
        plot=scatter_plot,
        text_plot=text_plot,
        block_size=args.block_size,
        constellation_size=args.size,
    )

    fps = 24
    anim = matplotlib.animation.FuncAnimation(
        fig=fig,
        func=update_plot,
        frames=wav_reader.get_num_samples() // plot_state.block_size,
        interval=1.0 / fps * 1000,
        fargs=(plot_state,),
    )

    plt.show()


if __name__ == "__main__":
    main()
