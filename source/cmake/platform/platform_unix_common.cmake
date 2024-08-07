# Copyright (c) 2021 radio core authors
#
# SPDX-License-Identifier: MIT-0

# Common part of platform-specific configuration for any UNIX platform. Includes
# both UNIX and APPLE platforms.

################################################################################
# Default compilation flags.

set(DEFAULT_C_FLAGS)
set(DEFAULT_CC_FLAGS)
set(DEFAULT_CXX_FLAGS)

if(WITH_DEVELOPER_STRICT)
  add_c_flag_if_supported(DEFAULT_CC_FLAGS -Werror)
endif()

if(WITH_DEVELOPER_SANITIZER)
  set(_sanitizer_flags
    -fsanitize=address
    -fsanitize=bool
    -fsanitize=bounds
    -fsanitize=enum
    -fsanitize=float-cast-overflow
    -fsanitize=float-divide-by-zero
    -fsanitize=nonnull-attribute
    -fsanitize=returns-nonnull-attribute
    -fsanitize=signed-integer-overflow
    -fsanitize=undefined
    -fsanitize=vla-bound
  )

  add_compiler_flag(DEFAULT_C_FLAGS "${_sanitizer_flags}")
  add_compiler_flag(DEFAULT_CXX_FLAGS "${_sanitizer_flags}")

  unset(_sanitizer_flags)
endif()

# Collect flags which are to be enabled by default.
#
# For regular compilation use same warnings as for strict configuration, but
# do not escalate them to errors.

# C/C++ flags
add_c_flag_if_supported(DEFAULT_CC_FLAGS -Wall)
add_c_flag_if_supported(DEFAULT_CC_FLAGS -Wextra)

# Some of the -Wall are too strict, so we disable them.
add_c_flag_if_supported(DEFAULT_CC_FLAGS -Wno-sign-compare)
add_c_flag_if_supported(DEFAULT_CC_FLAGS -Wno-missing-field-initializers)
add_c_flag_if_supported(DEFAULT_CC_FLAGS -fno-strict-aliasing)

# Optimization flags.
#
# NOTES:
#   - `-ftree-vectorize` is typically included into `-03`, so is not explicitly
#     enabled here.
#   - `-ffast-math` causes weird slowdown in SIMD-vectorized code when using
#     intrinsics with CLang. For example, peak detection kernel becomes 2-3
#     times slower with the fast math.

# Using -march=native on Apple M2 Max CPU does not imply the vector arithmetic
# for the half-float (__ARM_FEATURE_FP16_VECTOR_ARITHMETIC), Tested with the
# clang-1403.0.22.14.1.
#
# This also makes it so code detects Neon instruction set on BeagleBone Black
# board which uses AM335x CPU.
#
# This is solved with -mcpu=native, which is also advised to be used instead of
# the -march=native:
#   https://community.arm.com/arm-community-blogs/b/tools-software-ides-blog/posts/compiler-flags-across-architectures-march-mtune-and-mcpu
if(APPLE AND CMAKE_OSX_ARCHITECTURES)
  # Do not use native architecture when cross-compiling.
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm" OR
   CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch")
  add_c_flag_if_supported(DEFAULT_CC_FLAGS -mcpu=native)
else()
  add_c_flag_if_supported(DEFAULT_CC_FLAGS -march=native)
endif()

################################################################################
# Enable extra warnings which are not covered with the Wall and the ones we want
# to be enabled for the project.
#
# This also maintains flags used to cancel affect of the strict flags.

macro(add_c_warning_flag_if_supported LANG WARNING)
  add_c_flag_if_supported(DEFAULT_${LANG}_FLAGS -W${WARNING})
  add_c_flag_if_supported(CANCEL_${LANG}_STRICT_FLAGS -Wno-${WARNING})
endmacro()

macro(add_cxx_warning_flag_if_supported LANG WARNING)
  add_cxx_flag_if_supported(DEFAULT_${LANG}_FLAGS -W${WARNING})
  add_cxx_flag_if_supported(CANCEL_${LANG}_STRICT_FLAGS -Wno-${WARNING})
endmacro()

add_c_warning_flag_if_supported(C missing-prototypes)

add_c_warning_flag_if_supported(CC missing-declarations)
add_c_warning_flag_if_supported(CC undef)
add_c_warning_flag_if_supported(CC undef-prefix)
add_c_warning_flag_if_supported(CC implicit-fallthrough)
add_c_warning_flag_if_supported(CC double-promotion)
add_c_warning_flag_if_supported(CC missing-include-dirs)
add_c_warning_flag_if_supported(CC shadow)
add_c_warning_flag_if_supported(CC vla)
add_c_warning_flag_if_supported(CC switch)
add_c_warning_flag_if_supported(CC switch-enum)
add_c_warning_flag_if_supported(CC dangling-else)
add_c_warning_flag_if_supported(CC format)
add_c_warning_flag_if_supported(CC array-parameter)

# C++-only flags.
#
# NOTE: GNU C++ passes the test but later causes
#   '-Wmissing-prototypes' is valid for C/ObjC but not for C++ [-Werror]
#
# This is probably because test program is compiled without strict flags.
# Disable the warning explicitly when building with GCC.
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_cxx_warning_flag_if_supported(CXX missing-prototypes)
endif()

################################################################################
# Collect flags which are to be removed from cflags to cancel out strict
# behavior of compiler checks.

# Nothing yet.

################################################################################
# Collect flags which are to be added to the CFLAGS and CXXFLAGS to cancel
# affect of strict flags.
#
# Only flags which are not handled as part of being enabled explicitly above.

add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-cast-function-type)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-deprecated-declarations)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-sign-compare)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-unused-but-set-variable)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-unused-const-variable)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-unused-function)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-unused-parameter)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-unused-variable)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-maybe-uninitialized)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-type-limits)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-int-in-bool-context)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-pointer-bool-conversion)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-tautological-compare)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-extra-semi-stmt)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-empty-init-stmt)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-conversion)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-narrowing)
add_c_flag_if_supported(CANCEL_CC_STRICT_FLAGS -Wno-restrict)

add_cxx_flag_if_supported(CANCEL_CXX_STRICT_FLAGS -Wno-class-memaccess)

################################################################################
# Append effective flags.

add_compiler_flag(CMAKE_C_FLAGS "${DEFAULT_C_FLAGS}")
add_compiler_flag(CMAKE_C_FLAGS "${DEFAULT_CC_FLAGS}")
add_compiler_flag(CMAKE_CXX_FLAGS "${DEFAULT_CC_FLAGS}")
add_compiler_flag(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")

unset(DEFAULT_C_FLAGS)
unset(DEFAULT_CC_FLAGS)
unset(DEFAULT_CXX_FLAGS)
