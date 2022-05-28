#include "VideoCore4.h"
#include "VideoCore4Subtarget.h"
#include "VideoCore4Util.h"

#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#include <iostream>

using namespace llvm;

#define DEBUG_TYPE "vc4-delayslot-filler"
#define PASS_DESC  "delay Slot fill opt pass"

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
      return PASS_DESC;
    }

  private:
    bool DelaySlotFiller(MachineBasicBlock &MBB);
  };

  char VideoCore4DelaySlotFiller::ID = 0;
}

INITIALIZE_PASS(VideoCore4DelaySlotFiller,
                DEBUG_TYPE,
                PASS_DESC,
                false,
                false)

bool
VideoCore4DelaySlotFiller::runOnMachineFunction(MachineFunction &F) {
  if (skipFunction(F.getFunction())) {
    return false;
  }

  LLVM_DEBUG(dbgs() << "== VideoCore4DelaySlotFiller == ("
             << F.getName()
             << ")\n");

  bool isModified = false;

  for (MachineFunction::iterator FI = F.begin(), FIE = F.end(); FI != FIE; FI++) {
    if (DelaySlotFiller(*FI)) {
      isModified = true;
    }
  }

  return isModified;
}

// check branch and fill delayslot
bool
VideoCore4DelaySlotFiller::DelaySlotFiller(MachineBasicBlock &MBB) {
  bool                        isChanged = false;
  MachineBasicBlock::iterator MBBI;
  const auto                 &Subtarget = MBB.getParent()->getSubtarget();
  const TargetInstrInfo      *TII       = Subtarget.getInstrInfo();

  // empty MBB
  if (MBB.size() == 0) return false;

  for (MBBI = MBB.getLastNonDebugInstr();; MBBI--) {
    while (!vc4util::isEffectiveMBBI(MBBI)) {
      if (MBBI == MBB.getFirstNonDebugInstr()) {
        break;
      }
      MBBI--;
    }
    if (MBBI == MBB.getFirstNonDebugInstr()) {
      if (vc4util::isBranchMBBI(MBBI)
	  || vc4util::isReturnMBBI(MBBI)
	  || vc4util::isCallMBBI(MBBI)) {
	MachineBasicBlock::iterator I  = MBBI;
	MachineInstr               *MI = &(*I);
	I++;
	DebugLoc dl = MI->getDebugLoc();
	for (int i=0; i<vc4util::numDelayslot; i++) {
	  BuildMI(MBB, I, dl, TII->get(VideoCore4::NOP));
	}
	isChanged = true;
	break;
      }
    }

    // must fill delay slot
    if (vc4util::isBranchMBBI(MBBI)
	|| vc4util::isReturnMBBI(MBBI)
	|| vc4util::isCallMBBI(MBBI)) {
      MachineBasicBlock::iterator I  = MBBI;
      MachineInstr               *MI = &(*I);
      I++;
      DebugLoc dl = MI->getDebugLoc();

      // fill delay slot
      int                         delayslotInstNum  = vc4util::numDelayslot;
      MachineBasicBlock::iterator fillCandidateMBBI = MBBI;
      MachineBasicBlock::iterator stopMBBI          = MBBI;
      
      fillCandidateMBBI--;
      for (;;) {
	// if branch inst, give up
	if (vc4util::isBranchMBBI(fillCandidateMBBI)
	    || vc4util::isReturnMBBI(fillCandidateMBBI)
	    || vc4util::isCallMBBI(fillCandidateMBBI)) {
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

	    // should not move cmp instr
	    if (vc4util::isCmpMBBI(fillCandidateMBBI)) {
	      break;
	    }

	    // should not move not effective instr
	    if (!vc4util::isEffectiveMBBI(fillCandidateMBBI)) {
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

	      for (int i=0; i<vc4util::numDelayslot-delayslotInstNum; i++) {
		fillCandidateMBBI--;
	      }
	      break;
	    }
	    if (maySchedBoundaryMBBI == MBB.getLastNonDebugInstr()) { break; }
	    
	    maySchedBoundaryMBBI++;
	    while (!vc4util::isEffectiveMBBI(maySchedBoundaryMBBI)) { maySchedBoundaryMBBI++; }
	    if (MBBI == MBB.getFirstNonDebugInstr()) { break; }
	  }
	}
	if (delayslotInstNum == 0) { break; }
	if (fillCandidateMBBI == MBB.getFirstNonDebugInstr()) { break; }
	fillCandidateMBBI--;
	while (!vc4util::isEffectiveMBBI(fillCandidateMBBI)) {
	  if (fillCandidateMBBI == MBB.getFirstNonDebugInstr()) break;
	  fillCandidateMBBI--;
	}
	if (fillCandidateMBBI == MBB.getFirstNonDebugInstr()) break;
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
