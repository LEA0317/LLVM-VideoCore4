//===-- VideoCore4TargetInfo.cpp - VideoCore4 Target Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

Target llvm::TheVideoCore4Target;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeVideoCore4TargetInfo() {
  RegisterTarget<Triple::videocore, /*HasJIT=*/true> X(TheVideoCore4Target,
                                                       "videocore",
                                                       "VideoCore4 LittleEndian",
                                                       "VideoCore4");
}
