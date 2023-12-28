# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

add_library(radio_core_benchmark INTERFACE
  base_app.h
)

target_link_libraries(radio_core_benchmark INTERFACE
  Argparse::argparse
)
