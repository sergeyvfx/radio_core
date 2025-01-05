# Copyright (c) 2021 radio shell authors
#
# SPDX-License-Identifier: MIT-0

add_library(external_pffft
  pffft.c
  pffft.h
)

# Sielnce the "building with simd disabled" warning on CPUs which are known to
# not be supported by PFFFT vectorization.
if(WITH_PROCESSOR_FAMILY_STM32)
  target_compile_definitions(external_pffft PRIVATE PFFFT_SIMD_DISABLE)
endif()

if(MSVC)
  target_compile_definitions(external_pffft PRIVATE _USE_MATH_DEFINES)
endif()

target_include_directories(external_pffft
 PUBLIC
  .
)
