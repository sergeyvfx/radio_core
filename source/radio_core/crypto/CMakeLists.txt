# Copyright (c) 2022 radio core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  crc-16-ccitt.h
  md5.h
)

add_library(radio_core_crypto INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET radio_core_crypto PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

radio_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/radio_core/crypto
)

function(radio_core_crypto_test PRIMITIVE_NAME)
  radio_core_test(
      crypto_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES radio_core_crypto)
endfunction()

radio_core_crypto_test(crc-16-ccitt)
radio_core_crypto_test(md5)
