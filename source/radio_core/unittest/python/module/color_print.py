# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

import os
import sys


def _ansi_colors_supported():
    if sys.platform == "win32" and "ANSICON" not in os.environ:
        return False

    # NOTE: isatty is not always implemented
    return hasattr(sys.stdout, "isatty") and sys.stdout.isatty()


class _ANSIColors:
    BLACK = "\033[0;30m"
    RED = "\033[0;31m"
    GREEN = "\033[0;32m"
    BROWN = "\033[0;33m"
    BLUE = "\033[0;34m"
    PURPLE = "\033[0;35m"
    CYAN = "\033[0;36m"
    LIGHT_GRAY = "\033[0;37m"
    DARK_GRAY = "\033[1;30m"
    LIGHT_RED = "\033[1;31m"
    LIGHT_GREEN = "\033[1;32m"
    YELLOW = "\033[1;33m"
    LIGHT_BLUE = "\033[1;34m"
    LIGHT_PURPLE = "\033[1;35m"
    LIGHT_CYAN = "\033[1;36m"
    LIGHT_WHITE = "\033[1;37m"
    BOLD = "\033[1m"
    FAINT = "\033[2m"
    ITALIC = "\033[3m"
    UNDERLINE = "\033[4m"
    BLINK = "\033[5m"
    NEGATIVE = "\033[7m"
    CROSSED = "\033[9m"
    END = "\033[0m"


class _NULLColors:
    BLACK = ""
    RED = ""
    GREEN = ""
    BROWN = ""
    BLUE = ""
    PURPLE = ""
    CYAN = ""
    LIGHT_GRAY = ""
    DARK_GRAY = ""
    LIGHT_RED = ""
    LIGHT_GREEN = ""
    YELLOW = ""
    LIGHT_BLUE = ""
    LIGHT_PURPLE = ""
    LIGHT_CYAN = ""
    LIGHT_WHITE = ""
    BOLD = ""
    FAINT = ""
    ITALIC = ""
    UNDERLINE = ""
    BLINK = ""
    NEGATIVE = ""
    CROSSED = ""
    END = ""


if _ansi_colors_supported():
    _BaseClass = _ANSIColors
else:
    _BaseClass = _NULLColors


class ColorPrint(_BaseClass):
    @classmethod
    def print(cls, *args, **kwargs):
        print(*args, *kwargs)

    @classmethod
    def flush(cls):
        sys.stdout.flush()
