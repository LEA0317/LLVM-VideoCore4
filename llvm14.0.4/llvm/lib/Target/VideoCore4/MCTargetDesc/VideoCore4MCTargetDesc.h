//===-- VideoCore4MCTargetDesc.h - VideoCore4 Target Descriptions ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides VideoCore4 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef VideoCore4MCTARGETDESC_H
#define VideoCore4MCTARGETDESC_H

#include "llvm/MC/MCTargetOptions.h"

#include <memory>

namespace llvm {
  class MCAsmBackend;
  class MCSubtargetInfo;
  class Target;
  class MCCodeEmitter;
  class MCContext;
  class MCRegisterInfo;
  class MCInstrInfo;
  class MCObjectTargetWriter;

  extern Target TheVideoCore4Target;

  MCCodeEmitter *createVideoCore4MCCodeEmitter(const MCInstrInfo    &MCII,
					       const MCRegisterInfo &MRI,
					       MCContext            &Ctx);

  MCAsmBackend *createVideoCore4AsmBackend(const Target          &T,
					   const MCSubtargetInfo &STI,
					   const MCRegisterInfo  &MRI,
					   const MCTargetOptions &Option);

  std::unique_ptr<MCObjectTargetWriter> createVideoCore4ELFObjectWriter(uint8_t OSABI);
} // End llvm namespace

// Defines symbolic names for VideoCore4 registers.
// This defines a mapping from register name to register number.
#define GET_REGINFO_ENUM
#include "VideoCore4GenRegisterInfo.inc"

// Defines symbolic names for the VideoCore4 instructions.
#define GET_INSTRINFO_ENUM
#include "VideoCore4GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "VideoCore4GenSubtargetInfo.inc"

#endif
