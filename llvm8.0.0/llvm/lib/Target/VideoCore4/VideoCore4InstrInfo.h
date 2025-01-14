//==-- VideoCore4InstrInfo.h - VideoCore4 Instruction Information ---*- C++ -*-===//
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

#ifndef LLVM_TARGET_VIDEOCORE4INSTRINFO_H
#define LLVM_TARGET_VIDEOCORE4INSTRINFO_H

#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "VideoCore4RegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "VideoCore4GenInstrInfo.inc"

namespace llvm {

class VideoCore4TargetMachine;
class VideoCore4Subtarget;
struct VideoCore4RegisterInfo;
  
class VideoCore4InstrInfo : public VideoCore4GenInstrInfo {
  const VideoCore4RegisterInfo RI;
protected:
  const VideoCore4Subtarget &Subtarget;
public:
  explicit VideoCore4InstrInfo(const VideoCore4Subtarget &STI);

  static VideoCore4InstrInfo *create(VideoCore4Subtarget &STI);
  
  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  virtual const VideoCore4RegisterInfo& getRegisterInfo() const { return RI; }

  void
  copyPhysReg(MachineBasicBlock          &MBB,
	      MachineBasicBlock::iterator I,
	      const DebugLoc             &DL,
	      unsigned                    DestReg,
	      unsigned                    SrcReg,
	      bool                        KillSrc) const override;

  virtual void storeRegToStackSlot(MachineBasicBlock          &MBB,
                                   MachineBasicBlock::iterator MBBI,
                                   unsigned                    SrcReg,
				   bool                        isKill,
				   int                         FrameIndex,
                                   const TargetRegisterClass  *RC,
                                   const TargetRegisterInfo   *TRI) const;

  virtual void loadRegFromStackSlot(MachineBasicBlock          &MBB,
				    MachineBasicBlock::iterator MBBI,
				    unsigned                    DestReg,
				    int                         FrameIndex,
				    const TargetRegisterClass  *RC,
				    const TargetRegisterInfo   *TRI) const;

  virtual bool AnalyzeBranch(MachineBasicBlock               &MBB,
			     MachineBasicBlock              *&TBB,
			     MachineBasicBlock              *&FBB,
			     SmallVectorImpl<MachineOperand> &Cond,
			     bool                             AllowModify = false) const;

  virtual unsigned InsertBranch(MachineBasicBlock        &MBB,
				MachineBasicBlock        *TBB,
                                 MachineBasicBlock       *FBB,
                                 ArrayRef<MachineOperand> Cond,
                                 DebugLoc                 DL) const;

  virtual unsigned RemoveBranch(MachineBasicBlock &MBB) const;

  void adjustStackPtr(int64_t                     amount,
		      MachineBasicBlock          &MBB,
		      MachineBasicBlock::iterator I) const;
};

}

#endif
