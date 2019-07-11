#include "VideoCore4.h"
#include "VideoCore4Subtarget.h"
#include "VideoCore4Util.h"

#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#include <iostream>

using namespace llvm;

#define DEBUG_TYPE "vc4-delayslot-filler"

namespace llvm {
  FunctionPass *createVideoCore4DelaySlotFillerPass(void);
}

namespace {
  struct VideoCore4DelaySlotFiller : public MachineFunctionPass {
    static char ID;

    VideoCore4DelaySlotFiller()
      : MachineFunctionPass(ID) {
      initializeVideoCore4DelaySlotFillerPass(*PassRegistry::getPassRegistry());
    }

    bool runOnMachineFunction(MachineFunction &Fn) override;

    StringRef getPassName() const override {
      return "delay Slot fill opt pass";
    }

  private:
    bool DelaySlotFiller(MachineBasicBlock &MBB);
  };

  char VideoCore4DelaySlotFiller::ID = 0;
}

INITIALIZE_PASS(VideoCore4DelaySlotFiller,
                "vc4-delayslot-filler",
                "vc4 delayslot fill pass",
                false,
                false)

bool
VideoCore4DelaySlotFiller::runOnMachineFunction(MachineFunction &Fn) {
  if (skipFunction(Fn.getFunction())) {
    return false;
  }

  LLVM_DEBUG(dbgs() << ("== VideoCore4DelaySlotFiller ==\n"));

  bool isModified = false;

  for (MachineFunction::iterator FI = Fn.begin(), FIE = Fn.end(); FI != FIE; FI++) {
    if (DelaySlotFiller(*FI)) {
      isModified = true;
    }
  }

  return isModified;
}

// check branch and fill delayslot (Todo: improve this)
bool
VideoCore4DelaySlotFiller::DelaySlotFiller(MachineBasicBlock &MBB) {
  bool                        isChanged = false;
  MachineBasicBlock::iterator MBBI;
  const auto                 &Subtarget = MBB.getParent()->getSubtarget();
  const TargetInstrInfo      *TII       = Subtarget.getInstrInfo();
  
  for (MBBI = MBB.getLastNonDebugInstr();; MBBI--) {
    while (!vc4util::isEffectiveInst(MBBI->getOpcode())) {
      MBBI--;
    }
    if (MBBI == MBB.getFirstNonDebugInstr()) { break; }

    // must fill delay slot
    if (vc4util::isBranch(MBBI->getOpcode())
	|| vc4util::isReturn(MBBI->getOpcode())
	|| vc4util::isCall(MBBI->getOpcode())) {
      MachineBasicBlock::iterator I  = MBBI;
      MachineInstr               *MI = &(*I);
      I++;
      DebugLoc  dl = MI->getDebugLoc();

      // fill delay slot
      int                         delayslotInstNum  = 3;
      MachineBasicBlock::iterator fillCandidateMBBI = MBBI;
      MachineBasicBlock::iterator stopMBBI          = MBBI;
      
      fillCandidateMBBI--;
      for (;;) {
	// if branch inst, give up
	if (vc4util::isBranch(fillCandidateMBBI->getOpcode())
	    || vc4util::isReturn(fillCandidateMBBI->getOpcode())
	    || vc4util::isCall(fillCandidateMBBI->getOpcode())) {
	  break;
	} else {
	  MachineBasicBlock::iterator maySchedBoundaryMBBI = fillCandidateMBBI;
	  maySchedBoundaryMBBI++;
	  for (;;) {
	    // has dependency (cannot insert)
	    if (vc4util::HasDataDepForDelaySlot(&(*fillCandidateMBBI), &(*maySchedBoundaryMBBI))    != UINT_MAX
		|| vc4util::HasDataDepForDelaySlot(&(*maySchedBoundaryMBBI), &(*fillCandidateMBBI)) != UINT_MAX) {
	      break;
	    }

	    // fill dalay slot
	    if (maySchedBoundaryMBBI == stopMBBI) {
	      MachineInstr *miResched = &(*fillCandidateMBBI);
	      MBB.remove(miResched);
	      MBB.insert(I, miResched);
	      isChanged = true;
	      delayslotInstNum--;
	      stopMBBI++;

	      if (fillCandidateMBBI == MBB.getFirstNonDebugInstr()) { break; }

	      for (int i=0; i<3-delayslotInstNum; i++) {
		fillCandidateMBBI--;
	      }
	      break;
	    }
	    if (maySchedBoundaryMBBI == MBB.getLastNonDebugInstr()) { break; }
	    
	    maySchedBoundaryMBBI++;
	    while (!vc4util::isEffectiveInst(maySchedBoundaryMBBI->getOpcode())) { maySchedBoundaryMBBI++; }
	    if (MBBI == MBB.getFirstNonDebugInstr()) { break; }
	  }
	}
	if (delayslotInstNum == 0) { break; }
	if (fillCandidateMBBI == MBB.getFirstNonDebugInstr()) { break; }
	fillCandidateMBBI--;
	while (!vc4util::isEffectiveInst(fillCandidateMBBI->getOpcode())) { fillCandidateMBBI--; }
	if (MBBI == MBB.getFirstNonDebugInstr()) { break; }
      }
      
      // insert remain nop
      while (delayslotInstNum > 0) {
	BuildMI(MBB, I, dl, TII->get(VideoCore4::NOP));
	delayslotInstNum--;
	isChanged = true;
      }
      if (MBBI == MBB.getFirstNonDebugInstr()) { break; }
    }
    if (MBBI == MBB.getFirstNonDebugInstr()) { break; }
  }  
  return isChanged;
}

FunctionPass*
llvm::createVideoCore4DelaySlotFillerPass(void) {
  return new VideoCore4DelaySlotFiller();
}
