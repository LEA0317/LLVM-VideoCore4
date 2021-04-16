//===-- VideoCore4TargetObjectFile.h - VideoCore4 Object Info -*- C++ ---------*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VideoCore4_VideoCore4TARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_VideoCore4_VideoCore4TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {
class VideoCore4TargetMachine;

/// This implementation is used for VideoCore4 ELF targets.
class VideoCore4ELFTargetObjectFile : public TargetLoweringObjectFileELF {
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
};

} // end namespace llvm

#endif
