//==-- VideoCore4.h - Top-level interface for representation -*- C++ -*----===-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM VideoCore4 backend.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_VIDEOCORE4_H
#define LLVM_TARGET_VIDEOCORE4_H

#include "MCTargetDesc/VideoCore4MCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class VideoCore4TargetMachine;
  class VideoCore4AsmPrinter;
  class FunctionPass;
  class formatted_raw_ostream;

  FunctionPass*
  createVideoCore4ISelDag(VideoCore4TargetMachine &TM,
			  CodeGenOpt::Level        OptLevel);
  FunctionPass *createVideoCore4PseudoFixupPass(void);
  FunctionPass *createVideoCore4DelJmpPass(void);
  FunctionPass *createVideoCore4CFGOptPass(void);
  FunctionPass *createVideoCore4DelaySlotFillerPass(void);

  void initializeVideoCore4DelaySlotFillerPass(PassRegistry&);
  void initializeVideoCore4PseudoFixupPass(PassRegistry&);
  void initializeVideoCore4DelJmpPass(PassRegistry&);
  void initializeVideoCore4CFGOptimizerPass(PassRegistry&);
} // end namespace llvm;

#endif
