//==- VideoCore4TargetMachine.h - Define TargetMachine for VideoCore4 -*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the VideoCore4 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//


#ifndef LLVM_TARGET_VIDEOCORE4_TARGETMACHINE_H
#define LLVM_TARGET_VIDEOCORE4_TARGETMACHINE_H

#include "VideoCore4InstrInfo.h"
#include "VideoCore4ISelLowering.h"
#include "VideoCore4FrameLowering.h"
#include "VideoCore4SelectionDAGInfo.h"
#include "VideoCore4RegisterInfo.h"
#include "VideoCore4Subtarget.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

/// VideoCore4TargetMachine
///
class VideoCore4TargetMachine : public LLVMTargetMachine {
  VideoCore4Subtarget                       Subtarget;
  const DataLayout                          DL; // Calculates type size & alignment
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

public:
  VideoCore4TargetMachine(const Target              &T,
			  const Triple              &TT,
			  StringRef                  CPU,
			  StringRef                  FS,
			  const TargetOptions       &Options,
			  Optional<Reloc::Model>     RM,
			  Optional<CodeModel::Model> CM,
			  CodeGenOpt::Level          OL,
			  bool                       JIT);

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  
  TargetTransformInfo getTargetTransformInfo(const Function &F) override;

  const DataLayout          *getDataLayout() const { return &DL; }
  const VideoCore4Subtarget *getSubtargetImpl() const { return &Subtarget; }
  const VideoCore4Subtarget *getSubtargetImpl(const Function &) const override { return &Subtarget; }
  
  virtual TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
}; // VideoCore4TargetMachine.

} // end namespace llvm

#endif // LLVM_TARGET_VIDEOCORE4_TARGETMACHINE_H
