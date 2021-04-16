//- VideoCore4FrameLowering.h - Define frame lowering for VideoCore4 --*- C++ -*--//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef VIDEOCORE4_FRAMEINFO_H
#define VIDEOCORE4_FRAMEINFO_H

#include "VideoCore4.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class VideoCore4Subtarget;

class VideoCore4FrameLowering : public TargetFrameLowering {
protected:
  const VideoCore4Subtarget &STI;
  void determineFrameLayout(MachineFunction& MF) const;
  
public:
  explicit VideoCore4FrameLowering(const VideoCore4Subtarget &sti)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(4), 0), STI(sti) {}
  
  static VideoCore4FrameLowering *create(const VideoCore4Subtarget &ST);
  
  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction   &MF,
		    MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction   &MF,
		    MachineBasicBlock &MBB) const override;

  StackOffset getFrameIndexReference(const MachineFunction &MF,
				     int                    FI,
				     Register              &FrameReg) const override;

  bool hasFP(const MachineFunction &MF) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction            &MF,
				MachineBasicBlock          &MBB,
				MachineBasicBlock::iterator MI) const override;

  bool
  spillCalleeSavedRegisters(MachineBasicBlock              &MBB,
                            MachineBasicBlock::iterator     MI,
                            ArrayRef<llvm::CalleeSavedInfo> CSI,
                            const TargetRegisterInfo       *TRI) const override;
  bool
  restoreCalleeSavedRegisters(MachineBasicBlock                     &MBB,
                              MachineBasicBlock::iterator            MI,
                              MutableArrayRef<llvm::CalleeSavedInfo> CSI,
                              const TargetRegisterInfo              *TRI) const override;
};

} // End llvm namespace

#endif
