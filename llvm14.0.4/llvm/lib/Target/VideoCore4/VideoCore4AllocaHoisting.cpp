//===-- AllocaHoisting.cpp - Hoist allocas to the entry block --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Hoist the alloca instructions in the non-entry blocks to the entry blocks.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4.h"
#include "llvm/CodeGen/StackProtector.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

#define DEBUG_TYPE "vc4-alloca-hoisting"
#define PASS_DESC  "Hoisting alloca instructions in non-entry blocks to the entry block"

namespace {
// Hoisting the alloca instructions in the non-entry blocks to the entry
// block.
class VideoCore4AllocaHoisting : public FunctionPass {
public:
  static char ID; // Pass ID
  VideoCore4AllocaHoisting() : FunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addPreserved<StackProtector>();
  }

  StringRef getPassName() const override {
    return PASS_DESC;
  }

  bool runOnFunction(Function &function) override;
};
} // namespace

bool
VideoCore4AllocaHoisting::runOnFunction(Function &F) {
  if (skipFunction(F.getFunction())) {
    return false;
  }

  LLVM_DEBUG(dbgs() << "== AllocaHoisting == ("
             << F.getName()
             << ")\n");

  bool               functionModified    = false;
  Function::iterator I                   = F.begin();
  Instruction       *firstTerminatorInst = (I++)->getTerminator();

  for (Function::iterator E = F.end(); I != E; ++I) {
    for (BasicBlock::iterator BI = I->begin(), BE = I->end(); BI != BE;) {
      AllocaInst *allocaInst = dyn_cast<AllocaInst>(BI++);
      if (allocaInst && isa<ConstantInt>(allocaInst->getArraySize())) {
        allocaInst->moveBefore(firstTerminatorInst);
        functionModified = true;
      }
    }
  }

  return functionModified;
}

char VideoCore4AllocaHoisting::ID = 0;

namespace llvm {
  void initializeVideoCore4AllocaHoistingPass(PassRegistry&);
}

INITIALIZE_PASS(VideoCore4AllocaHoisting,
                DEBUG_TYPE,
                PASS_DESC,
                false,
                false)

FunctionPass*
llvm::createVideoCore4AllocaHoistingPass(void) {
  return new VideoCore4AllocaHoisting();
}
