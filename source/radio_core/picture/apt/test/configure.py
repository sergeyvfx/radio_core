# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT

# Configure the path to be able to import from the base utilities which are in
# the parent folder.
#
# TODO(sergey): There must be a neater way of organizing the module, so that it
# also integrates better with IDEs.
import sys
from pathlib import Path

sys.path.insert(
    0, str(Path(__file__).resolve().parent.parent.parent.parent / "unittest" / "python")
)
