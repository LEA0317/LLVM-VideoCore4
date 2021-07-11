//=- VideoCore4RegisterInfo.h - VideoCore4 Register Information Impl -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the VideoCore4 implementation of the MRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_VIDEOCORE4REGISTERINFO_H
#define LLVM_TARGET_VIDEOCORE4REGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "VideoCore4GenRegisterInfo.inc"

namespace llvm {

class VideoCore4TargetMachine;
class VideoCore4FrameLowering;

struct VideoCore4RegisterInfo : public VideoCore4GenRegisterInfo {
public:
  VideoCore4RegisterInfo();

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF = 0) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;
  const TargetRegisterClass* getPointerRegClass(const MachineFunction &MF,
						unsigned               Kind = 0) const override;

  void eliminateFrameIndex(MachineBasicBlock::iterator II,
                           int                         SPAdj,
			   unsigned                    FIOperandNum,
			   RegScavenger               *RS = NULL) const override;

  unsigned
  getRegPressureLimit(const TargetRegisterClass *RC,
                      MachineFunction           &MF) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;
};

} // end namespace llvm

#endif // LLVM_TARGET_VIDEOCORE4REGISTERINFO_H
