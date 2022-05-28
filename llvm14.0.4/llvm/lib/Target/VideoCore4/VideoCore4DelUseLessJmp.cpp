//===-- VideoCore4DelUseLessJmp.cpp --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#include "VideoCore4.h"
#include "VideoCore4Util.h"

#include "VideoCore4TargetMachine.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"

using namespace llvm;

#define DEBUG_TYPE "del-jmp"
#define PASS_DESC  "VideoCore4 Del Useless jmp"

namespace llvm {
  FunctionPass *createVideoCore4DelJmpPass(void);
}

namespace {
  struct VideoCore4DelJmp : public MachineFunctionPass {
    static char ID;
    VideoCore4DelJmp()
      : MachineFunctionPass(ID) {
      initializeVideoCore4DelJmpPass(*PassRegistry::getPassRegistry());
    }

    StringRef
    getPassName() const override {
      return PASS_DESC;
    }

    bool
    runOnMachineBasicBlock(MachineBasicBlock &MBB,
  	   	 	   MachineBasicBlock &MBBN);

    bool
    runOnMachineFunction(MachineFunction &F) override {
      if (skipFunction(F.getFunction())) {
	return false;
      }

      LLVM_DEBUG(dbgs() << "== VideoCore4DelUseLessJmp == ("
		 << F.getName()
		 << ")\n");
  
      bool Changed = false;
      
      MachineFunction::iterator FJ = F.begin();
      if (FJ != F.end())
	FJ++;
      if (FJ == F.end())
	return Changed;
      for (MachineFunction::iterator FI = F.begin(), FE = F.end();
	   FJ != FE; ++FI, ++FJ)
	// In STL style, F.end() is the dummy BasicBlock() like '\0' in 
	//  C string. 
	// FJ is the next BasicBlock of FI; When FI range from F.begin() to 
	//  the PreviousBasicBlock of F.end() call runOnMachineBasicBlock().
	Changed |= runOnMachineBasicBlock(*FI, *FJ);
      
      return Changed;
    }

  };
  char VideoCore4DelJmp::ID = 0;
} // end of anonymous namespace

INITIALIZE_PASS(VideoCore4DelJmp,
		DEBUG_TYPE,
		PASS_DESC,
		false,
		false)

bool
VideoCore4DelJmp::runOnMachineBasicBlock(MachineBasicBlock &MBB,
					 MachineBasicBlock &MBBN) {
  bool Changed = false;

  MachineBasicBlock::iterator I = MBB.end();
  MachineBasicBlock::iterator prevI;
  if (I != MBB.begin()) {
    I--;	// set I to the last instruction
  } else {
    return Changed;
  }
  
  if (vc4util::isUnconditionalJump(I->getOpcode())
      && I->getOperand(0).getMBB() == &MBBN) {
    MBB.erase(I);
    Changed = true;
  } else {
    for (int i=0; i < vc4util::branchKindNum; i++) {
      if (I->getOpcode()               == vc4util::BranchTakenOpcode[i]
	  && I->getOperand(2).getMBB() == &MBBN) {
	MBB.erase(I);
	Changed = true;
	break;
      } else {	
	if (I->getOpcode()              == vc4util::BranchNotTakenOpcode[i]) {
	  if (I->getOperand(2).getMBB() == &MBBN) {
	    MBB.erase(I);
	    Changed = true;
	  } else {
	    prevI = I;
	    prevI--;
	    if (prevI->getOpcode()              == vc4util::BranchTakenOpcode[i]) {
	      if (prevI->getOperand(2).getMBB() == &MBBN) {
		MBB.erase(prevI);
		Changed = true;
	      }
	    }
	  }
	}
      }
    }
  }

  return Changed;
}

FunctionPass*
llvm::createVideoCore4DelJmpPass(void) {
  return new VideoCore4DelJmp();
}

