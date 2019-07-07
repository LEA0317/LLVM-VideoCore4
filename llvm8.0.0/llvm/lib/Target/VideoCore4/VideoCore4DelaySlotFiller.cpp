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

// check branch and fill dalayslot (Todo: improve this)
bool
VideoCore4DelaySlotFiller::DelaySlotFiller(MachineBasicBlock &MBB) {
  MachineBasicBlock::iterator MBBI;
  const auto                 &Subtarget = MBB.getParent()->getSubtarget();
  const TargetInstrInfo      *TII       = Subtarget.getInstrInfo();

  MBB.dump();
  for (MBBI = MBB.getLastNonDebugInstr();; MBBI--) {
    MBBI->dump();
    if (MBBI == MBB.begin()) { break; }

    if (isBranch(MBBI->getOpcode())
	|| isCall(MBBI->getOpcode())) {
      MachineBasicBlock::iterator I = MBBI;
      MachineInstr *MI = &(*I);
      I++;
      DebugLoc dl = MI->getDebugLoc();
      BuildMI(MBB, I, dl, TII->get(VideoCore4::NOP));
      BuildMI(MBB, I, dl, TII->get(VideoCore4::NOP));
      BuildMI(MBB, I, dl, TII->get(VideoCore4::NOP));
    }
  }  
  return false;
}

FunctionPass*
llvm::createVideoCore4DelaySlotFillerPass(void) {
  return new VideoCore4DelaySlotFiller();
}
