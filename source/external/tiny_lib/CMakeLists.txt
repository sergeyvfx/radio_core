# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

add_library(external_tiny_lib INTERFACE
  include/tl_audio_wav/tl_audio_wav_reader.h
  include/tl_audio_wav/tl_audio_wav_writer.h
  include/tl_io/tl_io_file.h
)

target_include_directories(external_tiny_lib SYSTEM INTERFACE
  include
)
