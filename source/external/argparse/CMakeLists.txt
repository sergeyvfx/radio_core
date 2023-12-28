# Copyright (c) 2021 radio shell authors
#
# SPDX-License-Identifier: MIT-0

add_library(external_argparse INTERFACE
  include/argparse/argparse.hpp
)

target_include_directories(external_argparse SYSTEM
 INTERFACE
  include
)

add_library(Argparse::argparse INTERFACE IMPORTED GLOBAL)
set_target_properties(Argparse::argparse PROPERTIES
  INTERFACE_LINK_LIBRARIES external_argparse
)
