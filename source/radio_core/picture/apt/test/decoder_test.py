#!/usr/bin/env python3

# Copyright (c) 2023 radio core authors
#
# SPDX-License-Identifier: MIT

import configure

from module.test_runner import TestSuitRunner
from typing import Iterable
from base import PictureAPTTest, PictureAPTDecodeTestSuit


class PictureAPTDecoderTestSuit(PictureAPTDecodeTestSuit):
    def get_tests(self) -> Iterable[PictureAPTTest]:
        return (
            PictureAPTTest(base_name="grid_test_44100"),
            PictureAPTTest(base_name="NOAA_44100"),
        )


TestSuitRunner.main([PictureAPTDecoderTestSuit])
