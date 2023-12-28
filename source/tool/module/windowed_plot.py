# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

"""
Generic reusable class which plots Signal in a way that allows to view a
part of the signal (similar to zoom in and pan), but in an easy way.
"""

import argparse
import numpy
from matplotlib import pyplot, widgets
from . import signal


class Options:
    """Options for the windowed graph plotter"""

    window_size: float
    window_position: float

    def __init__(self):
        self.window_size = 100
        self.window_position = 0
        self.ymin = 1
        self.ymax = 1

    @classmethod
    def configure_argument_parser(cls, parser: argparse.ArgumentParser) -> None:
        """Add arguments related to the view window specification"""

        parser.add_argument(
            "--plot-window-size",
            type=float,
            help="Initial plot window size (milliseconds)",
            default=100,
        )

        parser.add_argument(
            "--plot-window-position",
            type=float,
            help="Initial position of the window in the signal (milliseconds)",
            default=0,
        )

        parser.add_argument("--plot-y-range", type=float, nargs=2)

    @classmethod
    def create_from_parser_arguments(cls, args) -> "Options":
        """
        Create options from the given parsed arguments

        The arguments are to be created as a parse result of a parser which
        includes arguments from configure_argument_parser().
        """

        options = Options()
        options.window_size = args.plot_window_size
        options.window_position = args.plot_window_position
        if args.plot_y_range:
            options.ymin = args.plot_y_range[0]
            options.ymax = args.plot_y_range[1]
        return options


class Plot:
    """
    Wrapper around the matplotlib to simplify plotting signals
    """

    _position_slider: widgets.Slider
    _window_size_slider: widgets.Slider

    def __init__(self, options=Options()) -> None:
        self._fig, self._ax = pyplot.subplots()
        self._ax.set_xlabel("Time [ms]")
        pyplot.grid()

        if options.ymin < options.ymax:
            self._ax.axis(
                [
                    0,
                    1,
                    options.ymin,
                    options.ymax,
                ]
            )

        self._ax.callbacks.connect(
            "xlim_changed", lambda x: self._update_sliders()
        )
        self._ax.callbacks.connect(
            "ylim_changed", lambda x: self._update_sliders()
        )

        self._max_time_in_ms = 0

        # Special flag to synchronize update callbacks, avoiding recursive
        # updates.
        self._ignore_sliders_update = False

        self._initial_window_size = options.window_size
        self._initial_window_position = options.window_position

        self._num_signals = 0

    def plot_signal(self, sig: signal.Signal) -> None:
        """Plot the given signal"""

        if not sig.samples:
            # Empty signal: nothing to do, so early output.
            return

        self._ignore_sliders_update = True

        # Unpack tuples (x,y) to flat lists of x and y values, and convert the
        # x values to milliseconds.
        x, y = zip(*sig.samples)
        x_ms = numpy.array(x) * 1000

        pyplot.plot(x_ms, y, label=sig.label)

        self._max_time_in_ms = max(self._max_time_in_ms, x_ms[-1])

        self._num_signals += 1

        self._ignore_sliders_update = False

    def show(self) -> None:
        """Show the current state of plots"""

        if self._num_signals > 1:
            pyplot.legend(loc="upper right")

        # Adjust the main plot to make room for the slider.
        pyplot.subplots_adjust(bottom=0.25)

        self._window_size_slider = widgets.Slider(
            ax=pyplot.axes([0.25, 0.1, 0.65, 0.03]),
            label="Window size [ms]",
            valmin=1,
            valmax=5000,
            valinit=self._initial_window_size,
        )
        self._window_size_slider.on_changed(lambda val: self._update())

        self._position_slider = widgets.Slider(
            ax=pyplot.axes([0.25, 0.05, 0.65, 0.03]),
            label="Start time [ms]",
            valmin=0.0,
            valmax=self._max_time_in_ms,
            valinit=self._initial_window_position,
        )
        self._position_slider.on_changed(lambda val: self._update())

        self._update()

        pyplot.show()

    def _update_sliders(self):
        if self._ignore_sliders_update:
            return

        self._ignore_sliders_update = True

        current_axis = self._ax.axis()
        self._position_slider.set_val(current_axis[0])
        self._window_size_slider.set_val(current_axis[1] - current_axis[0])

        self._ignore_sliders_update = False

    def _update(self):
        if self._ignore_sliders_update:
            return
        self._ignore_sliders_update = True

        window_size = self._window_size_slider.val

        self._position_slider.valmax = self._max_time_in_ms - window_size
        self._position_slider.ax.set_xlim(
            self._position_slider.valmin, self._position_slider.valmax
        )

        self._position_slider.set_val(
            max(
                self._position_slider.valmin,
                min(self._position_slider.valmax, self._position_slider.val),
            )
        )

        position = self._position_slider.val

        current_axis = self._ax.axis()
        self._ax.axis(
            [
                position,
                position + window_size,
                current_axis[2],
                current_axis[3],
            ]
        )

        self._fig.canvas.draw_idle()

        self._ignore_sliders_update = False
