# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

from typing import List, Tuple


class Signal:
    """
    Signal representation
    """

    label: str

    # (x, y) values of samples.
    #
    # The assumption is that the x coordinate denotes time in seconds.
    samples: List[Tuple[float, float]]

    def __init__(self, label=""):
        self.label = label
        self.samples = []
