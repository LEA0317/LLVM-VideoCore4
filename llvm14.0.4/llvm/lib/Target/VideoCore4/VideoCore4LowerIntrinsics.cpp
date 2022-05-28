//===-- VideoCore4LowerIntrinsics.cpp -----------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4.h"
#include "VideoCore4Subtarget.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/LowerMemIntrinsics.h"

using namespace llvm;

#define DEBUG_TYPE "vc4-lower-intrinsics"
#define PASS_DESC  "VideoCore4 Lower Intrinsics"

namespace {

const unsigned MaxStaticSize = 32;

class VideoCore4LowerIntrinsics : public ModulePass {
public:
  static char ID;

  VideoCore4LowerIntrinsics() : ModulePass(ID) {}

  bool runOnModule(Module &M) override;
  bool expandMemIntrinsicUses(Function &F);
  StringRef getPassName() const override {
    return PASS_DESC;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<TargetTransformInfoWrapperPass>();
  }
};

}

char VideoCore4LowerIntrinsics::ID = 0;

INITIALIZE_PASS(VideoCore4LowerIntrinsics,
		DEBUG_TYPE,
		PASS_DESC,
		false,
                false)

static bool shouldExpandOperationWithSize(Value *Size) {
  ConstantInt *CI = dyn_cast<ConstantInt>(Size);
  return !CI || (CI->getZExtValue() > MaxStaticSize);
}

bool
VideoCore4LowerIntrinsics::expandMemIntrinsicUses(Function &F) {
  Intrinsic::ID ID      = F.getIntrinsicID();
  bool          Changed = false;

  for (auto I = F.user_begin(), E = F.user_end(); I != E;) {
    Instruction *Inst = cast<Instruction>(*I);
    ++I;

    switch (ID) {
    case Intrinsic::memcpy:
      {
	auto *Memcpy = cast<MemCpyInst>(Inst);
	if (shouldExpandOperationWithSize(Memcpy->getLength())) {
	  Function *ParentFunc = Memcpy->getParent()->getParent();
	  const TargetTransformInfo &TTI =
            getAnalysis<TargetTransformInfoWrapperPass>().getTTI(*ParentFunc);
	  expandMemCpyAsLoop(Memcpy, TTI);
	  Changed = true;
	  Memcpy->eraseFromParent();
	}
	break;
      }
    case Intrinsic::memmove:
      {
	auto *Memmove = cast<MemMoveInst>(Inst);
	if (shouldExpandOperationWithSize(Memmove->getLength())) {
	  expandMemMoveAsLoop(Memmove);
	  Changed = true;
	  Memmove->eraseFromParent();
	}
	break;
      }
    case Intrinsic::memset:
      {
	auto *Memset = cast<MemSetInst>(Inst);
	if (shouldExpandOperationWithSize(Memset->getLength())) {
	  expandMemSetAsLoop(Memset);
	  Changed = true;
	  Memset->eraseFromParent();
	}	
	break;
      }
    default:
      {
	break;
      }
    }
  }

  return Changed;
}

bool
VideoCore4LowerIntrinsics::runOnModule(Module &M) {
  bool Changed = false;

  for (Function &F : M) {
    if (!F.isDeclaration())
      continue;

    switch (F.getIntrinsicID()) {
    case Intrinsic::memcpy:
    case Intrinsic::memmove:
    case Intrinsic::memset:
      if (expandMemIntrinsicUses(F))
        Changed = true;
      break;
    default:
      break;
    }
  }

  return Changed;
}

ModulePass*
llvm::createVideoCore4LowerIntrinsicsPass() {
  return new VideoCore4LowerIntrinsics();
}
