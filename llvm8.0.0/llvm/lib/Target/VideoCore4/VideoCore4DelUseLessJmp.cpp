//===-- VideoCore4DelUseLessJmp.cpp --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Simple pass to fills delay slots with useful instructions.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4.h"

#include "VideoCore4TargetMachine.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"

using namespace llvm;

#define DEBUG_TYPE "del-jmp"

#define BRANCH_KIND_NUM 10

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
    getPassName() const {
      return "VideoCore4 Del Useless jmp";
    }

    bool
    runOnMachineBasicBlock(MachineBasicBlock &MBB,
  	   	 	   MachineBasicBlock &MBBN);

    bool
    runOnMachineFunction(MachineFunction &F) {
      if (skipFunction(F.getFunction())) {
	return false;
      }
  
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
		"del-jmp",
		"del jump pass",
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
  
  if (I->getOpcode()               == VideoCore4::JMP
      && I->getOperand(0).getMBB() == &MBBN) {
    MBB.erase(I);
    Changed = true;
  } else {
    unsigned BranchTakenOpcode[BRANCH_KIND_NUM] = {
      VideoCore4::JMP_COMP_EQ_P,
      VideoCore4::JMP_COMP_NE_P,
      VideoCore4::JMP_COMP_GT_P,
      VideoCore4::JMP_COMP_GE_P,
      VideoCore4::JMP_COMP_LT_P,
      VideoCore4::JMP_COMP_LE_P,
      VideoCore4::JMP_COMP_HI_P,
      VideoCore4::JMP_COMP_HS_P,
      VideoCore4::JMP_COMP_LO_P,
      VideoCore4::JMP_COMP_LS_P
    };
    unsigned BranchNotTakenOpcode[BRANCH_KIND_NUM] = {
      VideoCore4::JMP_COMP_EQ_F_P,
      VideoCore4::JMP_COMP_NE_F_P,
      VideoCore4::JMP_COMP_GT_F_P,
      VideoCore4::JMP_COMP_GE_F_P,
      VideoCore4::JMP_COMP_LT_F_P,
      VideoCore4::JMP_COMP_LE_F_P,
      VideoCore4::JMP_COMP_HI_F_P,
      VideoCore4::JMP_COMP_HS_F_P,
      VideoCore4::JMP_COMP_LO_F_P,
      VideoCore4::JMP_COMP_LS_F_P
    };

    for (int i=0; i < BRANCH_KIND_NUM; i++) {
      if (I->getOpcode()               == BranchTakenOpcode[i]
	  && I->getOperand(2).getMBB() == &MBBN) {
	MBB.erase(I);
	Changed = true;
	break;
      } else {	
	if (I->getOpcode()              == BranchNotTakenOpcode[i]) {
	  if (I->getOperand(2).getMBB() == &MBBN) {
	    MBB.erase(I);
	    Changed = true;
	  } else {
	    prevI = I;
	    prevI--;
	    if (prevI->getOpcode()              == BranchTakenOpcode[i]) {
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

