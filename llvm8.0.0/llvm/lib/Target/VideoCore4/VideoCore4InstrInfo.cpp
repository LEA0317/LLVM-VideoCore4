//===-- VideoCore4InstrInfo.cpp - VideoCore4 Instruction Information -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the VideoCore4 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4InstrInfo.h"
#include "VideoCore4.h"
#include "VideoCore4MachineFunctionInfo.h"
#include "VideoCore4TargetMachine.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#define GET_INSTRINFO_CTOR_DTOR
#include "VideoCore4GenInstrInfo.inc"

using namespace llvm;

#define BRANCH_KIND_NUM 10
static unsigned BranchTakenOpcode[BRANCH_KIND_NUM] = {
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
static unsigned BranchNotTakenOpcode[BRANCH_KIND_NUM] = {
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

static bool isCondTrueBranch(unsigned opc) {
  for (int i=0; i<BRANCH_KIND_NUM; i++) {
    if (BranchTakenOpcode[i] == opc) return true;
  }
  return false;
}
static bool isCondFalseBranch(unsigned opc) {
  for (int i=0; i<BRANCH_KIND_NUM; i++) {
    if (BranchNotTakenOpcode[i] == opc) return true;
  }
  return false;
}
static bool isCondBranch(unsigned opc) {
  if (isCondTrueBranch(opc) || isCondFalseBranch(opc)) return true;
  return false;
}

namespace CC {
  const int DUMMY = 0;
}
static void
parseCondBranch(MachineInstr                    *LastInst,
                MachineBasicBlock              *&Target,
                SmallVectorImpl<MachineOperand> &Cond) {
  Cond.push_back(MachineOperand::CreateImm(CC::DUMMY));
  Target = LastInst->getOperand(2).getMBB();
}


VideoCore4InstrInfo::VideoCore4InstrInfo(const VideoCore4Subtarget &STI)
  : VideoCore4GenInstrInfo(VideoCore4::ADJCALLSTACKDOWN, VideoCore4::ADJCALLSTACKUP),
    RI(),
    Subtarget(STI) {}

VideoCore4InstrInfo*
VideoCore4InstrInfo::create(VideoCore4Subtarget &STI) {
  return new llvm::VideoCore4InstrInfo(STI);
}


void
VideoCore4InstrInfo::copyPhysReg(MachineBasicBlock          &MBB,
				 MachineBasicBlock::iterator I,
				 const DebugLoc             &DL,
				 unsigned                    DestReg,
				 unsigned                    SrcReg,
				 bool                        KillSrc) const {
  if (VideoCore4::FR32RegClass.contains(DestReg, SrcReg)) {
    BuildMI(MBB, I, DL, get(VideoCore4::MOV_F), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  } else if (VideoCore4::GR32RegClass.contains(DestReg, SrcReg)) {
    BuildMI(MBB, I, DL, get(VideoCore4::MOV_R), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }
  llvm_unreachable("Cannot emit physreg copy instruction");
}

void
VideoCore4InstrInfo::storeRegToStackSlot(MachineBasicBlock          &MBB,
					 MachineBasicBlock::iterator I,
					 unsigned                    SrcReg,
					 bool                        isKill,
					 int                         FI,
					 const TargetRegisterClass  *RC,
					 const TargetRegisterInfo   *TRI) const {
  
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  MachineFunction        &MF  = *MBB.getParent();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineMemOperand      *MMO = MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(MF,
											  FI),
							MachineMemOperand::MOStore,
							MFI.getObjectSize(FI),
							MFI.getObjectAlignment(FI));

  BuildMI(MBB, I, DL, get(VideoCore4::MEM32_ST_LI))
    .addReg(SrcReg, getKillRegState(isKill))
    .addFrameIndex(FI)
    .addImm(0)
    .addMemOperand(MMO);
}

void
VideoCore4InstrInfo::loadRegFromStackSlot(MachineBasicBlock          &MBB,
					  MachineBasicBlock::iterator I,
					  unsigned                    DestReg,
					  int                         FI,
					  const TargetRegisterClass  *RC,
					  const TargetRegisterInfo   *TRI) const {
  
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();
  
  MachineFunction        &MF  = *MBB.getParent();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineMemOperand      *MMO = MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(MF,
											  FI),
							MachineMemOperand::MOStore,
							MFI.getObjectSize(FI),
							MFI.getObjectAlignment(FI));
  
  BuildMI(MBB, I, DL, get(VideoCore4::MEM32_LD_LI), DestReg)
    .addFrameIndex(FI)
    .addImm(0)
    .addMemOperand(MMO);
}

void
VideoCore4InstrInfo::adjustStackPtr(int64_t                     amount,
				    MachineBasicBlock          &MBB,
				    MachineBasicBlock::iterator I) const {
  DebugLoc DL = I != MBB.end() ? I->getDebugLoc() : DebugLoc();
  
  if (amount < 0) {
    BuildMI(MBB, I, DL, get(VideoCore4::SUB_F_RI), VideoCore4::SP)
      .addReg(VideoCore4::SP)
      .addImm(-amount);
  } else if (amount > 0) {
    BuildMI(MBB, I, DL, get(VideoCore4::ADD_F_RI), VideoCore4::SP)
      .addReg(VideoCore4::SP)
      .addImm(amount);
  } else {
    /* Do nothing if we're adjusting the stack by zero */
  }
}

bool
VideoCore4InstrInfo::AnalyzeBranch(MachineBasicBlock               &MBB,
				   MachineBasicBlock              *&TBB,
				   MachineBasicBlock              *&FBB,
				   SmallVectorImpl<MachineOperand> &Cond,
				   bool AllowModify) const {
  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  if (I == MBB.end()) {
    return false;
  }

  if (!isUnpredicatedTerminator(*I)) {
    return false;
  }

  // Get the last instruction in the block.
  MachineInstr               *LastInst = &*I;
  MachineBasicBlock::iterator LastMBBI = I;
  unsigned                    LastOpc  = LastInst->getOpcode();

  // If there is only one terminator instruction, process it.
  if (I == MBB.begin() || !isUnpredicatedTerminator(*--I)) {
    if (LastOpc == VideoCore4::JMP) {
      TBB = LastInst->getOperand(0).getMBB();
      return false;
    }
    if (isCondTrueBranch(LastOpc)) {
      // Block ends with fall-through condbranch.
      parseCondBranch(LastInst, TBB, Cond);
      return false;
    }
    return true; // Can't handle indirect branch.
  }
  
  // Get the instruction before it if it is a terminator.
  MachineInstr *SecondLastInst = &*I;
  unsigned      SecondLastOpc  = SecondLastInst->getOpcode();

  // If AllowModify is true and the block ends with two or more unconditional
  // branches, delete all but the first unconditional branch.
  if (AllowModify && LastOpc == VideoCore4::JMP) {
    while (SecondLastOpc == VideoCore4::JMP) {
      LastInst->eraseFromParent();
      LastInst = SecondLastInst;
      LastOpc  = LastInst->getOpcode();
      if (I == MBB.begin() || !isUnpredicatedTerminator(*--I)) {
        // Return now the only terminator is an unconditional branch.
        TBB = LastInst->getOperand(0).getMBB();
        return false;
      } else {
        SecondLastInst = &*I;
        SecondLastOpc  = SecondLastInst->getOpcode();
      }
    }
  }

  // If the block ends with a B and a Bcc, handle it.
  if (isCondBranch(SecondLastOpc) && LastOpc == VideoCore4::JMP) {
    for (int i=0; i<BRANCH_KIND_NUM; i++) {
      if (SecondLastOpc == BranchTakenOpcode[i]) {
	// Transform the code
	//
	// L2:
	//    bt L1
	//    ba L2
	// L1:
	//    ..
	//
	// into
	//
	// L2:
	//   bf L2                                                                                                                                                                                         
	// L1: 
	//   ...
	//
	MachineBasicBlock *TargetBB = SecondLastInst->getOperand(2).getMBB();
	if (AllowModify
	    && LastMBBI != MBB.end()
	    && MBB.isLayoutSuccessor(TargetBB)) {
	  MachineBasicBlock *BNcondMBB = LastInst->getOperand(0).getMBB();
	  
	  BuildMI(&MBB, MBB.findDebugLoc(SecondLastInst), get(BranchNotTakenOpcode[i]))
	    .addReg(SecondLastInst->getOperand(0).getReg())
	    .addReg(SecondLastInst->getOperand(1).getReg())
	    .addMBB(BNcondMBB);

	  LastMBBI->eraseFromParent();

	  TBB = FBB = nullptr;
	  return true;
	}
      }
    }
    TBB = FBB = nullptr;
    return true;
  }

  // If the block ends with two unconditional branches, handle it.  The second 
  // one is not executed.
  if (SecondLastOpc == VideoCore4::JMP && LastOpc == VideoCore4::JMP) {
    TBB = SecondLastInst->getOperand(0).getMBB();
    return false;
  }

  // Otherwise, can't handle this.   
  return true;
}

unsigned
VideoCore4InstrInfo::InsertBranch(MachineBasicBlock       &MBB,
				  MachineBasicBlock       *TBB,
				  MachineBasicBlock       *FBB,
				  ArrayRef<MachineOperand> Cond,
				  DebugLoc                 DL) const {
  llvm_unreachable("InsertBranch");
}

unsigned
VideoCore4InstrInfo::RemoveBranch(MachineBasicBlock &MBB) const {
  llvm_unreachable("RemoveBranch");
}

