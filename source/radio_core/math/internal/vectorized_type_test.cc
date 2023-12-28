// Copyright (c) 2023 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/unittest/test.h"

#include <iostream>

#include "radio_core/math/vectorized_float_type.h"

#include "radio_core/math/complex4.h"
#include "radio_core/math/complex8.h"
#include "radio_core/math/float4.h"
#include "radio_core/math/float8.h"
#include "radio_core/math/half4.h"
#include "radio_core/math/half8.h"
#include "radio_core/math/half_complex4.h"
#include "radio_core/math/half_complex8.h"
#include "radio_core/math/uint4.h"
#include "radio_core/math/uint8.h"
#include "radio_core/math/ushort4.h"
#include "radio_core/math/ushort8.h"

namespace radio_core {

// Not a real test, but more of a debug tool to be able to manually ensure which
// vectorized implementation is used for the types.
TEST(VectorizedType, TypeInfo) {
  using std::cout;
  using std::endl;

  cout << "Float4       : " << Float4::TypeInfo::GetName() << endl;
  cout << "Float8       : " << Float8::TypeInfo::GetName() << endl;
#if RADIO_CORE_HAVE_HALF
  cout << "Half4        : " << Half4::TypeInfo::GetName() << endl;
  cout << "Half8        : " << Half8::TypeInfo::GetName() << endl;
#else
  cout << "Half4        : <Not supported> " << endl;
  cout << "Half8        : <Not supported> " << endl;
#endif
  cout << "UInt4        : " << UInt4::TypeInfo::GetName() << endl;
  cout << "UInt8        : " << UInt8::TypeInfo::GetName() << endl;
  cout << "UShort4      : " << UShort4::TypeInfo::GetName() << endl;
  cout << "UShort8      : " << UShort8::TypeInfo::GetName() << endl;
  cout << "Complex4     : " << Complex4::TypeInfo::GetName() << endl;
  cout << "Complex8     : " << Complex8::TypeInfo::GetName() << endl;
#if RADIO_CORE_HAVE_HALF
  cout << "HalfComplex4 : " << HalfComplex4::TypeInfo::GetName() << endl;
  cout << "HalfComplex8 : " << HalfComplex8::TypeInfo::GetName() << endl;
#else
  cout << "HalfComplex4 : <Not supported>" << endl;
  cout << "HalfComplex8 : <Not supported>" << endl;
#endif
}

}  // namespace radio_core
