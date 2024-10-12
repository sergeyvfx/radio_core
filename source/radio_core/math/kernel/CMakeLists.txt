# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Regression tests.

function(radio_core_math_kernel_test PRIMITIVE_NAME)
  radio_core_test(
      math_kernel_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_math)
endfunction()

radio_core_math_kernel_test(abs)
radio_core_math_kernel_test(dot)
radio_core_math_kernel_test(dot_flip)
radio_core_math_kernel_test(fast_abs)
radio_core_math_kernel_test(fast_arg)
radio_core_math_kernel_test(horizontal_max)
radio_core_math_kernel_test(norm)
radio_core_math_kernel_test(peak_detector)
radio_core_math_kernel_test(power_spectral_density)
radio_core_math_kernel_test(rotator)

################################################################################
# Benhcmarks.

function(radio_core_math_kernel_benchmark PRIMITIVE_NAME)
  radio_core_benchmark(
      math_kernel_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_benchmark.cc
      LIBRARIES radio_core_math
  )
endfunction()

radio_core_math_kernel_benchmark(abs)
radio_core_math_kernel_benchmark(dot)
radio_core_math_kernel_benchmark(dot_flip)
radio_core_math_kernel_benchmark(fast_abs)
radio_core_math_kernel_benchmark(fast_arg)
radio_core_math_kernel_benchmark(horizontal_max)
radio_core_math_kernel_benchmark(norm)
radio_core_math_kernel_benchmark(peak_detector)
radio_core_math_kernel_benchmark(power_spectral_density)
radio_core_math_kernel_benchmark(rotator)
