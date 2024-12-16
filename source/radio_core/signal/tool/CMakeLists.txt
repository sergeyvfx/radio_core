# Copyright (c) 2023 radio core authors
#
# SPDX-License-Identifier: MIT-0

if (WITH_DEBUG_TOOLS)
  # Decimator debugger.
  add_executable(debug_decimator debug_decimator.cc)
  target_link_libraries(debug_decimator
    radio_core_signal_path
    Argparse::argparse
  )
  target_set_output_directory(debug_decimator ${TOOL_EXECUTABLE_OUTPUT_DIR})

  # Interpolator debugger.
  add_executable(debug_interpolator debug_interpolator.cc)
  target_link_libraries(debug_interpolator
    radio_core_signal_path
    Argparse::argparse
  )
  target_set_output_directory(debug_interpolator ${TOOL_EXECUTABLE_OUTPUT_DIR})
endif()
