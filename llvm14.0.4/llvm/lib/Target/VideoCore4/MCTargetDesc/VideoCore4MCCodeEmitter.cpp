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
#include "VideoCore4MCExpr.h"
#include "VideoCore4FixupKinds.h"

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

#define GET_INSTRINFO_ENUM
#include "VideoCore4GenInstrInfo.inc"

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
    break;
    //report_fatal_error("invalid instruction length");
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

#if 1 // debug
  MI.dump();
#endif

  getBinaryCodeForInstr(MI,
			Fixups,
			Binary,
			Scratch,
			STI);

  // this switch statement is redundant. but may be useful later
  switch (Size) {
  default:
    break;
    //report_fatal_error("invalid instruction length");
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
}

void
VideoCore4MCCodeEmitter::getMachineOpValue(const MCInst             &MI,
					   const MCOperand          &MO,
					   APInt                    &op,
					   SmallVectorImpl<MCFixup> &Fixups,
					   const MCSubtargetInfo    &STI) const {
#if 0 // konda
  MI.dump();
#endif

  if (MO.isReg()) {
    MCRegister Reg   = MO.getReg();
    unsigned   RegNo = Ctx.getRegisterInfo()->getEncodingValue(Reg);
    op = APInt(64*8, RegNo);
    return;
  } else if (MO.isImm()) {
    op = APInt(64*8, static_cast<unsigned>(MO.getImm()));
    return;
  } else if (MO.isSFPImm()) {
    op = APInt(64*8, static_cast<unsigned>(APFloat(bit_cast<float>(MO.getSFPImm())).bitcastToAPInt().getHiBits(32).getLimitedValue()));
    return;
  }
  // MO must be an Expr.

  assert(MO.isExpr());
  op = APInt(64*8, getExprOpValue(MO.getExpr(),
                                  Fixups,
                                  STI));
  return;
}

unsigned
VideoCore4MCCodeEmitter::getExprOpValue(const MCExpr             *Expr,
					SmallVectorImpl<MCFixup> &Fixups,
					const MCSubtargetInfo    &STI) const {
#if 0 // konda
  Expr->dump();
#endif
  MCExpr::ExprKind Kind = Expr->getKind();
  if (Kind == MCExpr::Constant) {
    return cast<MCConstantExpr>(Expr)->getValue();
  }

  if (Kind == MCExpr::Binary) {
    unsigned Res = getExprOpValue(cast<MCBinaryExpr>(Expr)->getLHS(), Fixups, STI);
    Res += getExprOpValue(cast<MCBinaryExpr>(Expr)->getRHS(), Fixups, STI);
    return Res;
  }

  if (Kind == MCExpr::Target) {
    const VideoCore4MCExpr *VideoCore4Expr = cast<VideoCore4MCExpr>(Expr);

    VideoCore4::Fixups FixupKind = VideoCore4::Fixups(0);
    switch (VideoCore4Expr->getKind()) {
    default:
      break;
    } // switch
    Fixups.push_back(MCFixup::create(0,
				     Expr,
				     MCFixupKind(FixupKind)));
    return 0;
  }

  if (Kind == MCExpr::SymbolRef) {
    VideoCore4::Fixups FixupKind = VideoCore4::Fixups(0);

    switch(cast<MCSymbolRefExpr>(Expr)->getKind()) {
    default:
      {
	llvm_unreachable("Unknown fixup kind!");
	break;
      }
    case MCSymbolRefExpr::VK_None:
      {
	FixupKind = VideoCore4::fixup_VideoCore4_32;
	break;
      }
    }
    Fixups.push_back(MCFixup::create(0,
				     Expr,
				     MCFixupKind(FixupKind)));
    return 0;
  }
    
  // All of the information is in the fixup.
  return 0;
}

#include "VideoCore4GenMCCodeEmitter.inc"
