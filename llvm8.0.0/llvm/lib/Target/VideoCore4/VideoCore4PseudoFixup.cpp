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
        unsigned           Reg1 = MI->getOperand(0).getReg();
        unsigned           Reg2 = MI->getOperand(1).getReg();
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB();
	
        MBB.erase(MI);
	
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))
	  .addReg(Reg1)
	  .addReg(Reg2);
	BuildMI(MBB, I, dl, TII->get(VideoCore4::JMP_CC_EQ))
	  .addMBB(BB);

	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_NE_P:
      {
        unsigned           Reg1 = MI->getOperand(0).getReg();
        unsigned           Reg2 = MI->getOperand(1).getReg();
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB();
	
        MBB.erase(MI);
	
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))
	  .addReg(Reg1)
	  .addReg(Reg2);
	BuildMI(MBB, I, dl, TII->get(VideoCore4::JMP_CC_NE))
	  .addMBB(BB);

	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_GT_P:
      {
        unsigned           Reg1 = MI->getOperand(0).getReg();
        unsigned           Reg2 = MI->getOperand(1).getReg();
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB();
	
        MBB.erase(MI);
	
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))
	  .addReg(Reg1)
	  .addReg(Reg2);
	BuildMI(MBB, I, dl, TII->get(VideoCore4::JMP_CC_GT))
	  .addMBB(BB);

	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_GE_P:
      {
        unsigned           Reg1 = MI->getOperand(0).getReg();
        unsigned           Reg2 = MI->getOperand(1).getReg();
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB();
	
        MBB.erase(MI);
	
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))
	  .addReg(Reg1)
	  .addReg(Reg2);
	BuildMI(MBB, I, dl, TII->get(VideoCore4::JMP_CC_GE))
	  .addMBB(BB);

	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_LT_P:
      {
        unsigned           Reg1 = MI->getOperand(0).getReg();
        unsigned           Reg2 = MI->getOperand(1).getReg();
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB();
	
        MBB.erase(MI);
	
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))
	  .addReg(Reg1)
	  .addReg(Reg2);
	BuildMI(MBB, I, dl, TII->get(VideoCore4::JMP_CC_LT))
	  .addMBB(BB);

	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_LE_P:
      {
        unsigned           Reg1 = MI->getOperand(0).getReg();
        unsigned           Reg2 = MI->getOperand(1).getReg();
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB();
	
        MBB.erase(MI);
	
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))
	  .addReg(Reg1)
	  .addReg(Reg2);
	BuildMI(MBB, I, dl, TII->get(VideoCore4::JMP_CC_LE))
	  .addMBB(BB);

	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_HI_P:
      {
        unsigned           Reg1 = MI->getOperand(0).getReg();
        unsigned           Reg2 = MI->getOperand(1).getReg();
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB();
	
        MBB.erase(MI);
	
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))
	  .addReg(Reg1)
	  .addReg(Reg2);
	BuildMI(MBB, I, dl, TII->get(VideoCore4::JMP_CC_HI))
	  .addMBB(BB);

	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_HS_P:
      {
        unsigned           Reg1 = MI->getOperand(0).getReg();
        unsigned           Reg2 = MI->getOperand(1).getReg();
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB();
	
        MBB.erase(MI);
	
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))
	  .addReg(Reg1)
	  .addReg(Reg2);
	BuildMI(MBB, I, dl, TII->get(VideoCore4::JMP_CC_HS))
	  .addMBB(BB);

	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_LO_P:
      {
        unsigned           Reg1 = MI->getOperand(0).getReg();
        unsigned           Reg2 = MI->getOperand(1).getReg();
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB();
	
        MBB.erase(MI);
	
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))
	  .addReg(Reg1)
	  .addReg(Reg2);
	BuildMI(MBB, I, dl, TII->get(VideoCore4::JMP_CC_LO))
	  .addMBB(BB);

	Changed = true;
	break;
      }
    case VideoCore4::JMP_COMP_LS_P:
      {
        unsigned           Reg1 = MI->getOperand(0).getReg();
        unsigned           Reg2 = MI->getOperand(1).getReg();
        MachineBasicBlock *BB   = MI->getOperand(2).getMBB();
	
        MBB.erase(MI);
	
	BuildMI(MBB, I, dl, TII->get(VideoCore4::CMP_P))
	  .addReg(Reg1)
	  .addReg(Reg2);
	BuildMI(MBB, I, dl, TII->get(VideoCore4::JMP_CC_LS))
	  .addMBB(BB);

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
