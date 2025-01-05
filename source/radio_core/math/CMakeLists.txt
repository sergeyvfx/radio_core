# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  average.h
  base_complex.h
  bessel.h
  bitwise.h
  color.h
  color_gradient.h
  colorspace.h
  complex.h
  complex2.h
  complex3.h
  complex4.h
  complex8.h
  dft.h
  distribution.h
  fft.h
  fft_api.h
  fft_api_pffft.h
  float2.h
  float3.h
  float4.h
  float8.h
  half2.h
  half3.h
  half4.h
  half8.h
  half_complex.h
  half_complex2.h
  half_complex3.h
  half_complex4.h
  half_complex8.h
  math.h
  num_digits.h
  quantize.h
  resample.h
  time.h
  uint2.h
  uint3.h
  uint4.h
  uint8.h
  ushort2.h
  ushort3.h
  ushort4.h
  ushort8.h

  vectorized_complex_type.h
  vectorized_float_type.h
  vectorized_int_type.h

  internal/complex4_neon.h
  internal/complex4_x86.h

  internal/complex8_complex4x2.h

  internal/float4_neon.h
  internal/float4_x86.h

  internal/float8_float4x2.h

  internal/half4_neon.h

  internal/half8_half4x2.h
  internal/half8_neon.h

  internal/half_complex4_neon.h

  internal/half_complex8_half_complex4x2.h
  internal/half_complex8_neon.h

  internal/uint4_neon.h
  internal/uint4_x86.h

  internal/ushort4_neon.h

  internal/uint8_uint4x2.h

  internal/ushort8_ushort4x2.h
  internal/ushort8_neon.h

  internal/math_neon.h
  internal/math_x86.h

  internal/half_math.h
  internal/half_bitwise.h

  internal/vectorized_type.h
  internal/vectorized_complex_scalar.h
  internal/vectorized_float_scalar.h
  internal/vectorized_int_scalar.h

  kernel/abs.h
  kernel/dot.h
  kernel/dot_flip.h
  kernel/fast_abs.h
  kernel/fast_arg.h
  kernel/fast_int_pow.h
  kernel/norm.h
  kernel/horizontal_max.h
  kernel/horizontal_sum.h
  kernel/peak_detector.h
  kernel/power_spectral_density.h

  kernel/internal/kernel_common.h
  kernel/internal/abs_vectorized.h
  kernel/internal/abs_neon.h
  kernel/internal/dot_vectorized.h
  kernel/internal/dot_neon.h
  kernel/internal/dot_flip_vectorized.h
  kernel/internal/fast_abs_vectorized.h
  kernel/internal/fast_abs_neon.h
  kernel/internal/fast_arg_vectorized.h
  kernel/internal/fast_arg_neon.h
  kernel/internal/horizontal_max_vectorized.h
  kernel/internal/horizontal_max_neon.h
  kernel/internal/horizontal_sum_vectorized.h
  kernel/internal/horizontal_sum_neon.h
  kernel/internal/norm_vectorized.h
  kernel/internal/norm_neon.h
  kernel/internal/peak_detector_vectorized.h
  kernel/internal/power_spectral_density_vectorized.h
  kernel/internal/rotator_vectorized.h

  unittest/complex_matchers.h
  unittest/vectorized_matchers.h
)

add_library(radio_core_math INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_math PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/math
)

################################################################################
# Regression tests.

add_library(radio_core_math_test_data INTERFACE
  internal/fft_test_data.h
)

function(radio_core_math_test PRIMITIVE_NAME)
  radio_core_test(
      math_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_math radio_core_math_test_data external_pffft)
endfunction()

function(radio_core_math_unittest_test PRIMITIVE_NAME)
  radio_core_test(
      math_${PRIMITIVE_NAME} unittest/internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_math radio_core_math_test_data external_pffft)
endfunction()

radio_core_test(math internal/math_test.cc LIBRARIES radio_core_math)

radio_core_math_test(average)
radio_core_math_test(bessel)
radio_core_math_test(bitwise)
radio_core_math_test(color)
radio_core_math_test(color_gradient)
radio_core_math_test(colorspace)
radio_core_math_test(complex)
radio_core_math_test(complex2)
radio_core_math_test(complex3)
radio_core_math_test(complex4)
radio_core_math_test(complex8)
radio_core_math_test(dft)
radio_core_math_test(distribution)
radio_core_math_test(fft)
radio_core_math_test(fft_api_pffft)
radio_core_math_test(float2)
radio_core_math_test(float3)
radio_core_math_test(float4)
radio_core_math_test(float8)
radio_core_math_test(half_math)
radio_core_math_test(half_bitwise)
radio_core_math_test(half_complex)
radio_core_math_test(half_complex2)
radio_core_math_test(half_complex3)
radio_core_math_test(half_complex4)
radio_core_math_test(half_complex8)
radio_core_math_test(half2)
radio_core_math_test(half3)
radio_core_math_test(half4)
radio_core_math_test(half8)
radio_core_math_test(num_digits)
radio_core_math_test(quantize)
radio_core_math_test(resample)
radio_core_math_test(uint2)
radio_core_math_test(uint3)
radio_core_math_test(uint4)
radio_core_math_test(uint8)
radio_core_math_test(ushort2)
radio_core_math_test(ushort3)
radio_core_math_test(ushort4)
radio_core_math_test(ushort8)
radio_core_math_test(vectorized_type)

radio_core_math_unittest_test(complex_matchers)
radio_core_math_unittest_test(vectorized_matchers)

################################################################################
# Benhcmarks.

function(radio_core_math_benchmark PRIMITIVE_NAME)
  radio_core_benchmark(
          math_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_benchmark.cc
          LIBRARIES radio_core_math
  )
endfunction()

radio_core_math_benchmark(dft_goertzel)

################################################################################
# Sub-directories.

add_subdirectory(kernel)
