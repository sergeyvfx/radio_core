#!/usr/bin/env python3

# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT


import configure

from module.test_runner import TestSuitRunner
from typing import Iterable
from base import PictureSSTVTest, PictureSSTVDecodeTestSuit


class PictureSSTVDecodePD290TestSuit(PictureSSTVDecodeTestSuit):
    def get_tests(self) -> Iterable[PictureSSTVTest]:
        return (
            PictureSSTVTest(
                base_name="SMPTE_Color_Bars_vox_11025",
                mode="PD120",  # Make sure mode from VIS was used
            ),
        )


TestSuitRunner.main([PictureSSTVDecodePD290TestSuit])
