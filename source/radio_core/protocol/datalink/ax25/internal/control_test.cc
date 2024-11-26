// Copyright (c) 2021 radio core authors
//
// SPDX-License-Identifier: MIT

#include "radio_core/protocol/datalink/ax25/control.h"

#include "radio_core/unittest/test.h"

namespace radio_core::protocol::datalink::ax25 {

TEST(Control, GetControlFormat) {
  EXPECT_EQ(GetControlFormat(0b01000000), ControlFormat::kInformation);
  EXPECT_EQ(GetControlFormat(0b01000001), ControlFormat::kSupervisory);
  EXPECT_EQ(GetControlFormat(0b01000011), ControlFormat::kUnnumbered);
}

TEST(Control, GetSupervisoryFrameControl) {
  EXPECT_EQ(GetSupervisoryFrameControl(0b00000000),
            SupervisoryFrameControl::kUnknown);

  EXPECT_EQ(GetSupervisoryFrameControl(0b11110001),
            SupervisoryFrameControl::kReceiveReady);
  EXPECT_EQ(GetSupervisoryFrameControl(0b11110101),
            SupervisoryFrameControl::kReceiveNotReady);
  EXPECT_EQ(GetSupervisoryFrameControl(0b11111001),
            SupervisoryFrameControl::kReject);
  EXPECT_EQ(GetSupervisoryFrameControl(0b11111101),
            SupervisoryFrameControl::kSelectiveReject);
}

TEST(Control, IsFinalFrameControl) {
  // Information.
  EXPECT_TRUE(IsFinalFrameControl(0b01010000));
  EXPECT_FALSE(IsFinalFrameControl(0b01000000));
  // Supervisory.
  EXPECT_TRUE(IsFinalFrameControl(0b01010001));
  EXPECT_FALSE(IsFinalFrameControl(0b01000001));
  // Unnumbered.
  EXPECT_TRUE(IsFinalFrameControl(0b01010011));
  EXPECT_FALSE(IsFinalFrameControl(0b01000011));
}

TEST(Control, IsPollFrameControl) {
  // Information.
  EXPECT_FALSE(IsPollFrameControl(0b01010000));
  EXPECT_TRUE(IsPollFrameControl(0b01000000));
  // Supervisory.
  EXPECT_FALSE(IsPollFrameControl(0b01010001));
  EXPECT_TRUE(IsPollFrameControl(0b01000001));
  // Unnumbered.
  EXPECT_FALSE(IsPollFrameControl(0b01010011));
  EXPECT_TRUE(IsPollFrameControl(0b01000011));
}

TEST(Control, FrameControlUsesPID) {
  EXPECT_FALSE(FrameControlUsesPID(0b00000001));  // S frame.

  EXPECT_TRUE(FrameControlUsesPID(0b00000000));  // I frame.
  EXPECT_TRUE(FrameControlUsesPID(0b00000011));  // U frame.
}

TEST(Control, FrameControlUsesInfo) {
  EXPECT_FALSE(FrameControlUsesInfo(0b00000001));  // S frame.

  EXPECT_TRUE(FrameControlUsesInfo(0b00000000));  // I frame.
  EXPECT_TRUE(FrameControlUsesInfo(0b00000011));  // U frame.
}

}  // namespace radio_core::protocol::datalink::ax25
