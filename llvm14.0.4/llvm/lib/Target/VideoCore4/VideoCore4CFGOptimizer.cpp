//===-- VideoCore4CFGOptimizer.cpp - CFG optimizations -----------------------===//
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
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#include "VideoCore4.h"
#include "VideoCore4Util.h"
#include "VideoCore4Subtarget.h"
#include "VideoCore4TargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "vc4-cfg"
#define PASS_DESC  "cfg opt pass"

namespace llvm {
  FunctionPass *createVideoCore4CFGOptPass(void);
}

namespace {
  struct VideoCore4CFGOptimizer : public MachineFunctionPass {
    static char ID;
    
    VideoCore4CFGOptimizer()
      : MachineFunctionPass(ID) {
      initializeVideoCore4CFGOptimizerPass(*PassRegistry::getPassRegistry());
    }

    bool
    runOnMachineFunction(MachineFunction &Fn) override;

    StringRef
    getPassName() const override {
      return PASS_DESC;
    }

    MachineFunctionProperties
    getRequiredProperties() const override {
      return MachineFunctionProperties().set(MachineFunctionProperties::Property::NoVRegs);
    }

  private:
    void InvertAndChangeJumpTarget(MachineInstr&, MachineBasicBlock*);
  };

  char VideoCore4CFGOptimizer::ID = 0;
}

INITIALIZE_PASS(VideoCore4CFGOptimizer,
                DEBUG_TYPE,
                PASS_DESC,
                false,
                false)

void
VideoCore4CFGOptimizer::InvertAndChangeJumpTarget(MachineInstr      &MI,
						  MachineBasicBlock *NewTarget) {
  const TargetInstrInfo *TII       = MI.getParent()->getParent()->getSubtarget().getInstrInfo();
  unsigned               NewOpcode = vc4util::reverseBranch(MI.getOpcode());

  MI.setDesc(TII->get(NewOpcode));
  MI.getOperand(0).setMBB(NewTarget);
}

bool
VideoCore4CFGOptimizer::runOnMachineFunction(MachineFunction &F) {
  if (skipFunction(F.getFunction())) {
    return false;
  }

  LLVM_DEBUG(dbgs() << "== VideoCore4CFGOptimizer == ("
	     << F.getName()
	     << ")\n");
  
  // Loop over all of the basic blocks.
  for (MachineFunction::iterator MBBb = F.begin(), MBBe = F.end();
       MBBb != MBBe; ++MBBb) {
    MachineBasicBlock *MBB = &*MBBb;

    // Traverse the basic block.
    MachineBasicBlock::iterator MII = MBB->getFirstTerminator();
    if (MII != MBB->end()) {
      MachineInstr &MI  = *MII;
      unsigned      Opc = MI.getOpcode();
      if (vc4util::isCondBranch(Opc)) {
	//
	// (Case 1) Transform the code if the following condition occurs:
	//   BB1: if (p0) jump BB3
	//   ...falls-through to BB2 ...
	//   BB2: jump BB4
	//   ...next block in layout is BB3...
	//   BB3: ...
	//
	//  Transform this to:
	//  BB1: if (!p0) jump BB4
	//  Remove BB2
	//  BB3: ...
	//
	// (Case 2) A variation occurs when BB3 contains a JMP to BB4:
	//   BB1: if (p0) jump BB3
	//   ...falls-through to BB2 ...
	//   BB2: jump BB4
	//   ...other basic blocks ...
	//   BB4:
	//   ...not a fall-thru
	//   BB3: ...
	//     jump BB4
	//
	// Transform this to:
	//   BB1: if (!p0) jump BB4
	//   Remove BB2
	//   BB3: ...
	//   BB4: ...
	//
	unsigned                         NumSuccs         = MBB->succ_size();
	MachineBasicBlock::succ_iterator SI               = MBB->succ_begin();
	MachineBasicBlock*               FirstSucc        = *SI;
	MachineBasicBlock*               SecondSucc       = *(++SI);
	MachineBasicBlock*               LayoutSucc       = nullptr;
	MachineBasicBlock*               JumpAroundTarget = nullptr;
	
	if (MBB->isLayoutSuccessor(FirstSucc)) {
	  LayoutSucc       = FirstSucc;
	  JumpAroundTarget = SecondSucc;
	} else if (MBB->isLayoutSuccessor(SecondSucc)) {
	  LayoutSucc       = SecondSucc;
	  JumpAroundTarget = FirstSucc;
	} else {
	  // Odd case...cannot handle.
	}
	
	// The target of the unconditional branch must be JumpAroundTarget.
	// TODO: If not, we should not invert the unconditional branch.
	MachineBasicBlock* CondBranchTarget = nullptr;
	if (vc4util::isRawCondBranch(MI.getOpcode())) {
	  CondBranchTarget = MI.getOperand(0).getMBB();
	}
	  
	if (!LayoutSucc || (CondBranchTarget != JumpAroundTarget)) {
	  continue;
	}
	
	if ((NumSuccs == 2) && LayoutSucc && (LayoutSucc->pred_size() == 1)) {	  
	  // Ensure that BB2 has one instruction -- an unconditional jump.
	  if ((LayoutSucc->size() == 1) &&
	      vc4util::isUnconditionalJump(LayoutSucc->front().getOpcode())) {
	    assert(JumpAroundTarget && "jump target is needed to process second basic block");
	    MachineBasicBlock* UncondTarget = LayoutSucc->front().getOperand(0).getMBB();
	    // Check if the layout successor of BB2 is BB3.
	    bool case1 = LayoutSucc->isLayoutSuccessor(JumpAroundTarget);
	    bool case2 = JumpAroundTarget->isSuccessor(UncondTarget)
	      && JumpAroundTarget->size() >= 1
	      && vc4util::isUnconditionalJump(JumpAroundTarget->back().getOpcode())
	      && JumpAroundTarget->pred_size() == 1
	      && JumpAroundTarget->succ_size() == 1;
	    
	    if (case1 || case2) {
	      InvertAndChangeJumpTarget(MI, UncondTarget);
	      MBB->replaceSuccessor(JumpAroundTarget, UncondTarget);
	      
	      // Remove the unconditional branch in LayoutSucc.
	      LayoutSucc->erase(LayoutSucc->begin());
	      LayoutSucc->replaceSuccessor(UncondTarget, JumpAroundTarget);
	      
	      // This code performs the conversion for case 2, which moves
	      // the block to the fall-thru case (BB3 in the code above).
	      if (case2 && !case1) {
		JumpAroundTarget->moveAfter(LayoutSucc);
		// only move a block if it doesn't have a fall-thru. otherwise
		// the CFG will be incorrect.
		if (!UncondTarget->canFallThrough()) {
		  UncondTarget->moveAfter(JumpAroundTarget);
		}
	      }
	      
	      //
	      // Correct live-in information. Is used by post-RA scheduler
	      // The live-in to LayoutSucc is now all values live-in to
	      // JumpAroundTarget.
	      //
	      std::vector<MachineBasicBlock::RegisterMaskPair> OrigLiveIn(LayoutSucc->livein_begin(), LayoutSucc->livein_end());
	      std::vector<MachineBasicBlock::RegisterMaskPair> NewLiveIn(JumpAroundTarget->livein_begin(), JumpAroundTarget->livein_end());
	      for (const auto &OrigLI : OrigLiveIn) {
		LayoutSucc->removeLiveIn(OrigLI.PhysReg);
	      }
	      for (const auto &NewLI : NewLiveIn) {
		LayoutSucc->addLiveIn(NewLI);
	      }
	    }
	  }
	}
      }
    }
  }
  return true;
}

FunctionPass*
llvm::createVideoCore4CFGOptPass(void) {
  return new VideoCore4CFGOptimizer();
}
