# Copyright (c) 2021 radio shell authors
#
# SPDX-License-Identifier: MIT-0

add_library(external_stb INTERFACE
  include/stb/stb_image.h
  include/stb/stb_image_write.h
)

target_include_directories(external_stb SYSTEM INTERFACE
  include
)
