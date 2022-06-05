//===-- VideoCore4MCExpr.cpp - VideoCore4 specific MC expression classes --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4.h"

#include "VideoCore4MCExpr.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/BinaryFormat/ELF.h"

#include <iostream>

using namespace llvm;

#define DEBUG_TYPE "videocore4-mcexpr"

const VideoCore4MCExpr*
VideoCore4MCExpr::create(VideoCore4MCExpr::VideoCore4ExprKind Kind,
		         const MCExpr                        *Expr,
		         MCContext                           &Ctx) {
  return new (Ctx) VideoCore4MCExpr(Kind, Expr);
}

const VideoCore4MCExpr*
VideoCore4MCExpr::create(const MCSymbol                      *Symbol,
    	                 VideoCore4MCExpr::VideoCore4ExprKind Kind,
	                 MCContext                           &Ctx) {
  const MCSymbolRefExpr *MCSym =
    MCSymbolRefExpr::create(Symbol, MCSymbolRefExpr::VK_None, Ctx);
  return new (Ctx) VideoCore4MCExpr(Kind, MCSym);
}

const VideoCore4MCExpr*
VideoCore4MCExpr::createGpOff(VideoCore4MCExpr::VideoCore4ExprKind Kind,
	   	              const MCExpr                        *Expr,
		              MCContext                           &Ctx) {
  return create(Kind, create(CEK_None, create(CEK_GPREL, Expr, Ctx), Ctx), Ctx);
}

void
VideoCore4MCExpr::printImpl(raw_ostream     &OS,
		            const MCAsmInfo *MAI) const {
  int64_t AbsVal;

  switch (getKind()) {
  default:
    break;
  case CEK_None:
  case CEK_Special:
    llvm_unreachable("CEK_None and CEK_Special are invalid");
    break;
  }

  OS << '(';
  if (Expr->evaluateAsAbsolute(AbsVal)) {
    OS << AbsVal;
  } else {
    Expr->print(OS, MAI, true);
  }
  OS << ')';
}

bool
VideoCore4MCExpr::evaluateAsRelocatableImpl(MCValue           &Res,
					    const MCAsmLayout *Layout,
					    const MCFixup     *Fixup) const {
  // Explicitly drop the layout and assembler to prevent any symbolic folding in
  // the expression handling.  This is required to preserve symbolic difference
  // expressions to emit the paired relocations.
  if (!getSubExpr()->evaluateAsRelocatable(Res, nullptr, nullptr))
    return false;

  Res = MCValue::get(Res.getSymA(),
		     Res.getSymB(),
		     Res.getConstant(),
		     getKind());
  // Custom fixup types are not valid with symbol difference expressions.
  return Res.getSymB() ? getKind() == CEK_None : true;
}

void VideoCore4MCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}

void VideoCore4MCExpr::fixELFSymbolsInTLSFixups(MCAssembler &Asm) const {
  switch (getKind()) {
  case CEK_None:
  case CEK_Special:
    llvm_unreachable("CEK_None and CEK_Special are invalid");
    break;
  default:  // ensure all cases are captured despite doing nothing
    break;
  }
}

bool
VideoCore4MCExpr::isGpOff(VideoCore4ExprKind &Kind) const {
  if (const VideoCore4MCExpr *S1 = dyn_cast<const VideoCore4MCExpr>(getSubExpr())) {
    if (const VideoCore4MCExpr *S2 = dyn_cast<const VideoCore4MCExpr>(S1->getSubExpr())) {
      if (S1->getKind() == CEK_None && S2->getKind() == CEK_GPREL) {
        Kind = getKind();
        return true;
      }
    }
  }
  return false;
}
