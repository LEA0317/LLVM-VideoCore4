//===-- VideoCore4FrameLowering.cpp - VideoCore4 Frame Information ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the VideoCore4 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4Subtarget.h"
#include "VideoCore4FrameLowering.h"
#include "VideoCore4InstrInfo.h"
#include "VideoCore4MachineFunctionInfo.h"
#include "VideoCore4ISelLowering.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

VideoCore4FrameLowering*
VideoCore4FrameLowering::create(const VideoCore4Subtarget &ST) {
  return new llvm::VideoCore4FrameLowering(ST);
}

/* Our stack frame is going to look like this:
 *
 *       HI
 *     .......   arguments passed on the stack
 *   ----------- function call boundary
 *     SSSSSSS   saved register block
 *     LLLLLLL   locals block
 *     AAAAAAA   outgoing arguments passed on the stack
 *   ----------- function call boundary
 *     .......   next function saved register block
 *       LO
 *
 * We want to use push and pop instructions for handing the
 * S block. This means that the function prologue and
 * epilogue have to be done in two stages: a push for the S
 * block and then a stack adjustment for the LA blocks.
 */

void
VideoCore4FrameLowering::determineFrameLayout(MachineFunction& MF) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  
  unsigned alignment = getStackAlignment();
  unsigned SLsize    = MFI.getStackSize();
  unsigned Asize     = MFI.getMaxCallFrameSize();

  if (MFI.hasVarSizedObjects())
    Asize = alignTo(Asize, alignment);
  
  MFI.setMaxCallFrameSize(Asize);
  unsigned SLAsize = SLsize + Asize;
  
  SLAsize = alignTo(SLAsize, alignment);
  MFI.setStackSize(SLAsize);
}

bool
VideoCore4FrameLowering::hasFP(const MachineFunction &MF) const {
  return false;
}

void
VideoCore4FrameLowering::emitPrologue(MachineFunction   &MF,
				      MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  
  determineFrameLayout(MF);

  const VideoCore4InstrInfo &TII = *static_cast<const VideoCore4InstrInfo*>(MF.getTarget().getMCInstrInfo());

  MachineBasicBlock::iterator MI = MBB.begin();
  DebugLoc                    dl = MI != MBB.end() ? MI->getDebugLoc() : DebugLoc();
  
  int stacksize = (int) MFI.getStackSize();
  TII.adjustStackPtr(-stacksize, MBB, MI);
}

bool
VideoCore4FrameLowering::spillCalleeSavedRegisters(MachineBasicBlock              &MBB,
						   MachineBasicBlock::iterator     MI,
						   ArrayRef<llvm::CalleeSavedInfo> CSI,
						   const TargetRegisterInfo       *TRI) const {
  return false;
}

bool
VideoCore4FrameLowering::restoreCalleeSavedRegisters(MachineBasicBlock                     &MBB,
						     MachineBasicBlock::iterator            MI,
						     MutableArrayRef<llvm::CalleeSavedInfo> CSI,
						     const TargetRegisterInfo              *TRI) const {
  return false;
}

void
VideoCore4FrameLowering::emitEpilogue(MachineFunction   &MF,
				      MachineBasicBlock &MBB) const {
  MachineFrameInfo           &MFI  = MF.getFrameInfo();  
  const VideoCore4InstrInfo  &TII  = *static_cast<const VideoCore4InstrInfo*>(MF.getTarget().getMCInstrInfo());
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();

  int stacksize = (int) MFI.getStackSize();
  TII.adjustStackPtr(stacksize, MBB, MBBI);
}

StackOffset
VideoCore4FrameLowering::getFrameIndexReference(const MachineFunction &MF,
						int                    FI,
						Register              &FrameReg) const {
  return StackOffset::getFixed(MF.getFrameInfo().getObjectOffset(FI));
}

MachineBasicBlock::iterator
VideoCore4FrameLowering::eliminateCallFramePseudoInstr(MachineFunction            &MF,
						       MachineBasicBlock          &MBB,
						       MachineBasicBlock::iterator I) const {
  const VideoCore4InstrInfo &TII = *static_cast<const VideoCore4InstrInfo*>(MF.getTarget().getMCInstrInfo());
  
  if (!hasReservedCallFrame(MF)) {
    if (I->getOpcode() == VideoCore4::ADJCALLSTACKDOWN) {
      int64_t Amount = I->getOperand(0).getImm();
      Amount = -Amount;
      TII.adjustStackPtr(Amount,
			 MBB,
			 I);
    } else if (I->getOpcode() == VideoCore4::ADJCALLSTACKUP) {
      int64_t Amount = I->getOperand(0).getImm();
      TII.adjustStackPtr(Amount,
			 MBB,
			 I);
    } else {
      llvm_unreachable("Cannot handle this call frame pseudo instruction");
    }
  }
  return MBB.erase(I);
}
