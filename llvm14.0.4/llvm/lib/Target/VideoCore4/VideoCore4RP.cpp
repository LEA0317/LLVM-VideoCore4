//===-- VideoCore4RP.cpp - RP optimizations -----------------------===//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Target/TargetMachine.h"

#include "VideoCore4.h"
#include "VideoCore4Util.h"
#include "VideoCore4Subtarget.h"
#include "VideoCore4TargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "vc4-rp"
#define PASS_DESC  "rp opt pass"

namespace llvm {
  FunctionPass *createVideoCore4RPPass(void);
}

namespace {
  struct VideoCore4RP : public MachineFunctionPass {
    static char ID;
    
    VideoCore4RP()
      : MachineFunctionPass(ID) {
      initializeVideoCore4RPPass(*PassRegistry::getPassRegistry());
    }

    bool runOnMachineFunction(MachineFunction &F) override;
    bool runOnMachineBasicBlock(MachineBasicBlock &MBB);

    StringRef getPassName() const override {
      return PASS_DESC;
    }
  };

  char VideoCore4RP::ID = 0;
}

INITIALIZE_PASS(VideoCore4RP,
                DEBUG_TYPE,
                PASS_DESC,
                false,
                false)

bool
VideoCore4RP::runOnMachineFunction(MachineFunction &F) {
  bool Changed = false;

  if (skipFunction(F.getFunction())) {
    return false;
  }

  LLVM_DEBUG(dbgs() << "== VideoCore4RP == ("
	     << F.getName()
	     << ")\n");

  for (MachineFunction::iterator FI = F.begin(), FE = F.end();
       FI != FE; ++FI) {
    Changed |= runOnMachineBasicBlock(*FI);
  }

  return Changed;
}
 
bool
VideoCore4RP::runOnMachineBasicBlock(MachineBasicBlock &MBB) {
  bool Changed = false;

  for (MachineBasicBlock::iterator I = MBB.getFirstNonDebugInstr(); I != MBB.end();) {
    MachineInstr *MI = &(*I);
    I++;
    DebugLoc dl     = MI->getDebugLoc();
    unsigned Opcode = MI->getOpcode();

    // register propagation
    auto RP = [&MBB, MI, I, dl](Register def,
				Register use) {
      bool                        isModified = false;
      bool                        isBlocked  = false;
      MachineBasicBlock::iterator MBBI       = I;
      MachineBasicBlock::iterator mbbi;

      for (;;) {
	isBlocked = false;
	if (MBBI == MBB.end()) break;

	// schedule boundary
	if (vc4util::isSchedulingBoundary(MBBI->getOpcode())) break;

	// check defs
	for (const auto &MO : MBBI->defs()) {
	  if (!MO.isReg()) continue;
	  if (MO.getReg() == def) { isBlocked = true; }
	  if (MO.getReg() == use) {
#if 0 // ToDo
	    if (isBlocked == true) {
	      isBlocked = false;
	    } else {
	      isBlocked = true;
	    }
#endif
	  }
	}

	for (auto &MO : MBBI->uses()) {
	  if (!MO.isReg()) continue;
	  if (MO.getReg() == def) {
	    MO.setReg(use);
	    mbbi       = MBBI;
	    isModified = true;
	  }
	}
	if (isBlocked) break;
	MBBI++;
      }

      if (!vc4util::isLiveOutReg(&MBB, def) && isModified) {
	if (mbbi == MBB.getLastNonDebugInstr()) {
	  MBB.erase(MI);
	  return true;
	}
	for (;;) {
	  for (const auto &mo_use : mbbi->uses()) {
	    if (!mo_use.isReg()) continue;

	    if (mo_use.getReg() == def) {
	      // cannot optimize
	      return isModified;
	    }
	  }
	  for (const auto &mo_def : mbbi->defs()) {
	    if (!mo_def.isReg()) continue;

	    if (mo_def.getReg() == def) {
	      MBB.erase(MI);
	      return true;
	    }
	  }
	  // schedule boundary
	  if (vc4util::isSchedulingBoundary(mbbi->getOpcode())) break;
	  mbbi++;
	  if (mbbi == MBB.getLastNonDebugInstr()) {
	    MBB.erase(MI);
	    return true;
	  }
	}
      }
      return isModified;
    };

    switch (Opcode) {
    case VideoCore4::MOV_F:
    case VideoCore4::MOV_R:
      {
	Register def = MI->getOperand(0).getReg();
	Register use = MI->getOperand(1).getReg();
	Changed |= RP(def, use);
	break;
      }
    }
  }

  return Changed;
}

FunctionPass*
llvm::createVideoCore4RPPass(void) {
  return new VideoCore4RP();
}
