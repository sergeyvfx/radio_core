# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

# Slow-Scan Television (SSTV) encoders, decoders, utility applications.

################################################################################
# Library.

set(PUBLIC_HEADERS
  encoder.h
  decoder.h
  line_decoder.h
  line_encoding.h
  line_sync.h
  luma.h
  message.h
  mode.h
  mode_limits.h
  mode_spec.h
  picture_decoder.h
  picture_encoder.h
  prefilter.h
  result.h
  vis.h
  vis_decoder.h
  vis_encoder.h
  vox.h
  vox_encoder.h

  mode_spec/common.h
  mode_spec/pd90.h
  mode_spec/pd120.h
  mode_spec/pd160.h
  mode_spec/pd180.h
  mode_spec/pd240.h
  mode_spec/pd290.h
)

add_library(radio_core_picture_sstv INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_picture_sstv
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(radio_core_picture_sstv INTERFACE
  radio_core_base
  radio_core_math
  radio_core_picture
  radio_core_signal
)

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/picture/sstv
)

################################################################################
# Regression tests.

function(radio_core_picture_sstv_test PRIMITIVE_NAME)
  radio_core_test(
      picture_sstv_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_picture_sstv external_tiny_lib
      ARGUMENTS --test_srcdir ${PROJECT_SOURCE_DIR}/data/test
  )
endfunction()

radio_core_picture_sstv_test(luma)
radio_core_picture_sstv_test(mode_spec)
radio_core_picture_sstv_test(vis_decoder)
radio_core_picture_sstv_test(vis_encoder)
radio_core_picture_sstv_test(result)

function(radio_core_sstv_decoder_mode_test MODE)
  if(NOT WITH_TESTS)
    return()
  endif()

  add_test(
    NAME radio_core_picture_sstv_decode_${MODE}_test
    COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/test/${MODE}.py
        --test_srcdir ${PROJECT_SOURCE_DIR}/data/test/sstv/${MODE}
        --report_rootdir=${CMAKE_BINARY_DIR}/test
        --sstv_decoder=$<TARGET_FILE:sstv_decoder>
  )
endfunction()

radio_core_sstv_decoder_mode_test(pd90)
radio_core_sstv_decoder_mode_test(pd120)
radio_core_sstv_decoder_mode_test(pd160)
radio_core_sstv_decoder_mode_test(pd180)
radio_core_sstv_decoder_mode_test(pd240)
radio_core_sstv_decoder_mode_test(pd290)

################################################################################
# Tools.

if(WITH_TOOLS)
  add_subdirectory(tool)
endif()
