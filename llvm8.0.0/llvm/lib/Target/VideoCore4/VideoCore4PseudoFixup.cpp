//===-- VideoCore4PseudoFixup.cpp - VideoCore4 pseudo fixer ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Expand FpMOVD/FpABSD/FpNEGD instructions into their single-precision pieces.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "vc4-pseudo-fixup"
#include "VideoCore4.h"
#include "VideoCore4Subtarget.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/Debug.h"
#include "VideoCore4RegisterInfo.h"

#include <cstdlib>
#include <iostream>

#define JUMP_COMP_CC(opcode) \
        unsigned           Reg1 = MI->getOperand(0).getReg(); \
        unsigned           Reg2 = MI->getOperand(1).getReg(); \
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB(); \
							      \
	MBB.erase(MI);					      \
							      \
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))      \
	  .addReg(Reg1)					      \
	  .addReg(Reg2);				      \
	BuildMI(MBB, I, dl, TII->get(opcode))  \
	  .addMBB(BB);

#define	SELECT_CC(opcode_t, opcode_f) \
        unsigned Reg1 = MI->getOperand(0).getReg(); \
        unsigned Reg2 = MI->getOperand(1).getReg(); \
        unsigned Reg3 = MI->getOperand(2).getReg(); \
        unsigned Reg4 = MI->getOperand(3).getReg(); \
        unsigned Reg5 = MI->getOperand(4).getReg(); \
						    \
        MBB.erase(MI);				    \
						    \
	if (Reg1 == Reg3) {			    \
	  BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))	   \
	    .addReg(Reg4)					   \
	    .addReg(Reg5);					   \
	  BuildMI(MBB, I, dl, TII->get(opcode_t))		   \
	    .addReg(Reg1)					   \
	    .addReg(Reg2);					   \
	} else if (Reg1 == Reg2) {				   \
	  BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))	   \
	    .addReg(Reg4)					   \
	    .addReg(Reg5);					   \
	  BuildMI(MBB, I, dl, TII->get(opcode_f))		   \
	    .addReg(Reg1)					   \
	    .addReg(Reg3);					   \
	} else {						   \
	  BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))	   \
	    .addReg(Reg4)					   \
	    .addReg(Reg5);					   \
	  BuildMI(MBB, I, dl, TII->get(opcode_t))		   \
	    .addReg(Reg1)					   \
	    .addReg(Reg2);					   \
 	  BuildMI(MBB, I, dl, TII->get(opcode_f))		   \
	    .addReg(Reg1)					   \
	    .addReg(Reg3);					   \
	}


using namespace llvm;

namespace llvm {
  FunctionPass *createVideoCore4PseudoFixupPass(void);
}

namespace {
  struct VideoCore4PseudoFixup : public MachineFunctionPass {
    explicit VideoCore4PseudoFixup() 
        : MachineFunctionPass(ID) {
      initializeVideoCore4PseudoFixupPass(*PassRegistry::getPassRegistry());
    }

    StringRef getPassName() const override {
      return "VideoCore4 pesudo fixer";
    }

    bool runOnMachineBasicBlock(MachineBasicBlock &MBB);
    bool runOnMachineFunction(MachineFunction &F) override;

    static char ID;
  };
  char VideoCore4PseudoFixup::ID = 0;
}

INITIALIZE_PASS(VideoCore4PseudoFixup,
		"vc4-pseudo-fixup",
		"VideoCore4 pseudo fixer",
		false,
		false)

FunctionPass*
llvm::createVideoCore4PseudoFixupPass() {
  return new VideoCore4PseudoFixup();
}

bool
VideoCore4PseudoFixup::runOnMachineBasicBlock(MachineBasicBlock &MBB) {
  bool Changed = false;

  const auto            &Subtarget = MBB.getParent()->getSubtarget();
  const TargetInstrInfo *TII       = Subtarget.getInstrInfo();

  for (MachineBasicBlock::iterator I = MBB.begin(); I != MBB.end();) {
    MachineInstr *MI = &(*I);
    I++;
    DebugLoc dl     = MI->getDebugLoc();
    unsigned Opcode = MI->getOpcode();
    switch (Opcode) {
    case VideoCore4::JMP_COMP_EQ_P:
      {
	JUMP_COMP_CC(VideoCore4::JMP_CC_EQ);
	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_NE_P:
      {
	JUMP_COMP_CC(VideoCore4::JMP_CC_NE);
	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_GT_P:
      {
	JUMP_COMP_CC(VideoCore4::JMP_CC_GT);
	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_GE_P:
      {
	JUMP_COMP_CC(VideoCore4::JMP_CC_GE);
	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_LT_P:
      {
	JUMP_COMP_CC(VideoCore4::JMP_CC_LT);
	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_LE_P:
      {
	JUMP_COMP_CC(VideoCore4::JMP_CC_LE);
	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_HI_P:
      {
	JUMP_COMP_CC(VideoCore4::JMP_CC_HI);
	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_HS_P:
      {
	JUMP_COMP_CC(VideoCore4::JMP_CC_HS);
	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_LO_P:
      {
	JUMP_COMP_CC(VideoCore4::JMP_CC_LO);
	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_LS_P:
      {
	JUMP_COMP_CC(VideoCore4::JMP_CC_LS);
	Changed = true;
	break;
      }
    case VideoCore4::SELECT_EQ_P:
      {
	SELECT_CC(VideoCore4::CMOV_EQ_P, VideoCore4::CMOV_NE_P);
	Changed = true;
	break;
      }
    case VideoCore4::SELECT_NE_P:
      {
	SELECT_CC(VideoCore4::CMOV_NE_P, VideoCore4::CMOV_EQ_P);
	Changed = true;
	break;
      }

    case VideoCore4::SELECT_GT_P:
      {
	SELECT_CC(VideoCore4::CMOV_GT_P, VideoCore4::CMOV_LE_P);
	Changed = true;
	break;
      }
    case VideoCore4::SELECT_GE_P:
      {
	SELECT_CC(VideoCore4::CMOV_GE_P, VideoCore4::CMOV_LT_P);
	Changed = true;
	break;
      }
    case VideoCore4::SELECT_LT_P:
      {
	SELECT_CC(VideoCore4::CMOV_LT_P, VideoCore4::CMOV_GE_P);
	Changed = true;
	break;
      }
    case VideoCore4::SELECT_LE_P:
      {
	SELECT_CC(VideoCore4::CMOV_LE_P, VideoCore4::CMOV_GT_P);
	Changed = true;
	break;
      }
    case VideoCore4::SELECT_HI_P:
      {
	SELECT_CC(VideoCore4::CMOV_HI_P, VideoCore4::CMOV_LS_P);
	Changed = true;
	break;
      }
    case VideoCore4::SELECT_HS_P:
      {
	SELECT_CC(VideoCore4::CMOV_HS_P, VideoCore4::CMOV_LO_P);
	Changed = true;
	break;
      }
    case VideoCore4::SELECT_LO_P:
      {
	SELECT_CC(VideoCore4::CMOV_LO_P, VideoCore4::CMOV_HS_P);
	Changed = true;
	break;
      }
    case VideoCore4::SELECT_LS_P:
      {
	SELECT_CC(VideoCore4::CMOV_LS_P, VideoCore4::CMOV_HI_P);
	Changed = true;
	break;
      }
    case VideoCore4::FSELECT_EQ_P:
      {
	SELECT_CC(VideoCore4::CMOV_EQ_P, VideoCore4::CMOV_NE_P);
	Changed = true;
	break;
      }
    case VideoCore4::FSELECT_NE_P:
      {
	SELECT_CC(VideoCore4::CMOV_NE_P, VideoCore4::CMOV_EQ_P);
	Changed = true;
	break;
      }

    case VideoCore4::FSELECT_GT_P:
      {
	SELECT_CC(VideoCore4::CMOV_GT_P, VideoCore4::CMOV_LE_P);
	Changed = true;
	break;
      }
    case VideoCore4::FSELECT_GE_P:
      {
	SELECT_CC(VideoCore4::CMOV_GE_P, VideoCore4::CMOV_LT_P);
	Changed = true;
	break;
      }
    case VideoCore4::FSELECT_LT_P:
      {
	SELECT_CC(VideoCore4::CMOV_LT_P, VideoCore4::CMOV_GE_P);
	Changed = true;
	break;
      }
    case VideoCore4::FSELECT_LE_P:
      {
	SELECT_CC(VideoCore4::CMOV_LE_P, VideoCore4::CMOV_GT_P);
	Changed = true;
	break;
      }
    case VideoCore4::FSELECT_HI_P:
      {
	SELECT_CC(VideoCore4::CMOV_HI_P, VideoCore4::CMOV_LS_P);
	Changed = true;
	break;
      }
    case VideoCore4::FSELECT_HS_P:
      {
	SELECT_CC(VideoCore4::CMOV_HS_P, VideoCore4::CMOV_LO_P);
	Changed = true;
	break;
      }
    case VideoCore4::FSELECT_LO_P:
      {
	SELECT_CC(VideoCore4::CMOV_LO_P, VideoCore4::CMOV_HS_P);
	Changed = true;
	break;
      }
    case VideoCore4::FSELECT_LS_P:
      {
	SELECT_CC(VideoCore4::CMOV_LS_P, VideoCore4::CMOV_HI_P);
	Changed = true;
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
VideoCore4PseudoFixup::runOnMachineFunction(MachineFunction &F) {
  if (skipFunction(F.getFunction())) {
    return false;
  }
  
  LLVM_DEBUG(dbgs() << ("== VideoCore4PseudoFixup ==\n"));
  
  bool Changed = false;
  
  for (MachineFunction::iterator FI = F.begin(), FE = F.end();
       FI != FE; ++FI) {
    Changed |= runOnMachineBasicBlock(*FI);
  }
  
  return Changed;
}
