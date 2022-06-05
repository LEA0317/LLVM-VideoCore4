//===-- VideoCore4FixupKinds.h - VideoCore4 Specific Fixup Entries ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VIDEOCORE4FIXUPKINDS_H
#define LLVM_LIB_TARGET_VIDEOCORE4FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace VideoCore4 {
  // Although most of the current fixup types reflect a unique relocation
  // one can have multiple fixup types for a given relocation and thus need
  // to be uniquely named.
  //
  // This table *must* be in the save order of
  // MCFixupKindInfo Infos[VideoCore4::NumTargetFixupKinds]
  // in VideoCore4AsmBackend.cpp.
  enum Fixups {
    fixup_VideoCore4_32 = FirstTargetFixupKind,
    // Marker
    LastTargetFixupKind,
    NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
  };
} // namespace VideoCore4
} // namespace llvm

#endif // LLVM_VIDEOCORE4FIXUPKINDS_H
