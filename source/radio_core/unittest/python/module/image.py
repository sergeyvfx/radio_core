# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

"""
Higher level representation of image pixels.

Gives access to image configuration (dimensions, number of channels) and takes
care of basic and common operations like file IO.
"""

import PIL.Image
import numpy
from pathlib import Path


class Image:
    pixels: numpy.ndarray

    def __init__(self, pixels=numpy.ndarray((0, 0, 0))):
        self.pixels = pixels

    @classmethod
    def read_from_file(cls, filepath: Path) -> "Image":
        pil_image = PIL.Image.open(filepath)
        pixels = numpy.array(pil_image)

        if pixels.dtype == numpy.uint8:
            pixels = numpy.asarray(pixels, dtype=numpy.float32) / 255

        return Image(
            pixels=pixels,
        )

    def width(self):
        return self.pixels.shape[1]

    def height(self):
        return self.pixels.shape[0]

    def num_channels(self):
        return self.pixels.shape[2]

    def save_to_file(self, filepath: Path):
        # NOTE: Use PIL for saving because we need to preserve number of
        # channels and `matplotlib.pyplot.imsave` always saves as RGBA.
        pil_image = PIL.Image.fromarray((self.pixels * 255).astype(numpy.uint8))
        pil_image.save(filepath)
