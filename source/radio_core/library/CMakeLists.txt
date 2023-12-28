# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

add_library(radio_core INTERFACE)

target_link_libraries(radio_core INTERFACE
  radio_core_base
  radio_core_crypto
  radio_core_info
  radio_core_math
  radio_core_modulation_analog
  radio_core_modulation_analog_am
  radio_core_modulation_analog_fm
  radio_core_modulation_analog_nfm
  radio_core_modulation_analog_wfm
  radio_core_modulation_digital_fsk
  radio_core_picture
  radio_core_picture_sstv
  radio_core_protocol_binary_nrzs
  radio_core_protocol_datalink
  radio_core_protocol_datalink_ax25
  radio_core_protocol_datalink_hdlc
  radio_core_protocol_packet_aprs
  radio_core_signal
  radio_core_signal_path
)

if(WITH_BENCHMARKS)
  target_link_libraries(radio_core INTERFACE
    radio_core_benchmark
  )
endif()

radio_core_install(TARGETS radio_core)
