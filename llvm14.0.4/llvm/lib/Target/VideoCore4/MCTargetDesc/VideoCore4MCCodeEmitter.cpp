//===-- VideoCore4MCCodeEmitter.cpp - Convert VideoCore4 Code to Machine Code ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the VideoCore4MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//
//

#include "VideoCore4MCCodeEmitter.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/EndianStream.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#define DEBUG_TYPE "mccodeemitter"

#define GET_INSTRMAP_INFO
#include "VideoCore4GenInstrInfo.inc"
#undef GET_INSTRMAP_INFO

namespace llvm {
  MCCodeEmitter *createVideoCore4MCCodeEmitter(const MCInstrInfo    &MCII,
					       const MCRegisterInfo &MRI,
					       MCContext            &Ctx) {
    return new VideoCore4MCCodeEmitter(MCII, Ctx, false);
  }
} // End of namespace llvm

void
VideoCore4MCCodeEmitter::EmitByte(unsigned char C,
				  raw_ostream  &OS) const {
  OS << (char)C;
}

void
VideoCore4MCCodeEmitter::EmitInstruction(APInt        Val,
					 unsigned     Size,
					 raw_ostream &OS) const {
  switch (Size) {
  default:
    report_fatal_error("invalid instruction length");
  case 2: {
    for (unsigned i = 0; i < Size; ++i) {
      unsigned Shift = i * 8;
      APInt    tmp   = Val;
      tmp.lshrInPlace(Shift);
      tmp &= 0xff;
      EmitByte(std::stol(toString(tmp, 10, true)), OS);
    }
    break;
  }
  case 4: {
    APInt tmp = Val;
    tmp.lshrInPlace(16);
    EmitInstruction(Val, 2, OS);
    EmitInstruction(tmp, 2, OS);
    break;
  }
  case 6: {
    break;
  }
  case 10: {
    break;
  }
  }
}

/// encodeInstruction - Emit the instruction.
/// Size the instruction
void
VideoCore4MCCodeEmitter::encodeInstruction(const MCInst             &MI,
					   raw_ostream              &OS,
					   SmallVectorImpl<MCFixup> &Fixups,
					   const MCSubtargetInfo    &STI) const {
  const MCInstrDesc &Desc = MCII.get(MI.getOpcode());
  int                Size = Desc.getSize();
  APInt              Binary(Size*8, 0);
  APInt              Scratch(Size*8, 0);

#if 0
  getBinaryCodeForInstr(MI,
			Fixups,
			Binary,
			Scratch,
			STI);

  // this switch statement is redundant. but may be useful later
  switch (Size) {
  default:
    report_fatal_error("invalid instruction length");
  case 2: {
    EmitInstruction(Binary,
		    Size,
		    OS);
    break;
  }
  case 4: {
    EmitInstruction(Binary,
		    Size,
		    OS);
    break;
  }
  case 6: {
    break;
  }
  case 10: {
    break;
  }
  }
#endif
}

//#include "VideoCore4GenMCCodeEmitter.inc"
