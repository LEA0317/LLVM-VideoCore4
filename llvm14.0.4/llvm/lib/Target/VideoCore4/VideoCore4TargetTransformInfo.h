//===- VideoCore4TargetTransformInfo.h - VideoCore4 specific TTI --------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// \file
// This file contains a TargetTransformInfo::Concept conforming object specific
// to the VideoCore4 target machine. It uses the target's detailed information to
// provide more precise answers to certain TTI queries, while letting the
// target independent and default TTI implementations handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VideoCore4_TARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_VideoCore4_TARGETTRANSFORMINFO_H

#include "VideoCore4.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"

namespace llvm {

class VideoCore4Subtarget;
class VideoCore4TargetLowering;
class VideoCore4TargetMachine;

class VideoCore4TTIImpl : public BasicTTIImplBase<VideoCore4TTIImpl> {
  using  BaseT = BasicTTIImplBase<VideoCore4TTIImpl>;
  friend BaseT;

  const VideoCore4Subtarget      *ST;
  const VideoCore4TargetLowering *TLI;

  const VideoCore4Subtarget      *getST()  const { return ST; }
  const VideoCore4TargetLowering *getTLI() const { return TLI; }

public:
  explicit VideoCore4TTIImpl(const VideoCore4TargetMachine *TM, const Function &F)
    : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl()),
      TLI(ST->getTargetLowering()) {}

  // Provide value semantics. MSVC requires that we spell all of these out.
  VideoCore4TTIImpl(const VideoCore4TTIImpl &Arg)
    : BaseT(static_cast<const BaseT &>(Arg)), ST(Arg.ST), TLI(Arg.TLI) {}
  VideoCore4TTIImpl(VideoCore4TTIImpl &&Arg)
    : BaseT(std::move(static_cast<BaseT &>(Arg))), ST(std::move(Arg.ST)),
      TLI(std::move(Arg.TLI)) {}
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_VideoCore4_ARCTARGETTRANSFORMINFO_H
