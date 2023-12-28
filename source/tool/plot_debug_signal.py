#!/usr/bin/env python3

# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

"""
Visualize signal written by a signal debugging tools such as the
radio_core::signal::DebugWriter C++ class.
"""

import argparse
import re
import sys

from typing import List, Dict

from module import signal, windowed_plot


class Context:
    """
    Context which holds entire signals and lines information
    """

    # Flat storage of all known signals.
    signals: List[signal.Signal]

    # Dictionary for a fast signal lookup by its label.
    _signals_lookup: Dict[str, signal.Signal]

    def __init__(self) -> None:
        self.signals = []
        self._signals_lookup = {}

    def get_labelled_signal(self, label: str) -> signal.Signal:
        """
        Grt signal for the given label

        If the signal does not exist yet a new one is created and returned.
        """

        sig = self._signals_lookup.get(label, None)
        if not sig:
            sig = signal.Signal(label=label)
            self.signals.append(sig)
            self._signals_lookup[label] = sig

        return sig


def create_argument_parser() -> argparse.ArgumentParser:
    """
    Create parser of command line arguments

    The parser will be configured to recognize all command line arguments, but
    no actual parsing will happen in this function.
    """

    parser = argparse.ArgumentParser(description="Histogram plotter")

    parser.add_argument(
        "input",
        nargs="?",
        type=argparse.FileType("r"),
        default=sys.stdin,
        help="File with values to read signals from (empty to read from STDIN)",
    )

    windowed_plot.Options.configure_argument_parser(parser)

    return parser


def read_signals_from_file(
    context: Context, input: argparse.FileType("r")
) -> None:
    """Read signals from the input opened by the argument parser"""

    regex = re.compile(
        r"l:(?P<label>\w+)\s+x:(?P<x>[\w\-\.]+)\s+y:(?P<y>[\w\-\.]+)"
    )

    for line in input.readlines():
        match = regex.match(line)

        label = match.group("label")
        x = float(match.group("x"))
        y = float(match.group("y"))

        signal = context.get_labelled_signal(label)
        signal.samples.append((x, y))


def main() -> None:
    parser = create_argument_parser()

    args = parser.parse_args()

    context = Context()
    read_signals_from_file(context, args.input)

    options = windowed_plot.Options.create_from_parser_arguments(args)
    plot = windowed_plot.Plot(options)

    for sig in context.signals:
        plot.plot_signal(sig)

    plot.show()


if __name__ == "__main__":
    main()
