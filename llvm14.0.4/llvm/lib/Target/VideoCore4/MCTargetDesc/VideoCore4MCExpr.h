//===-- VideoCore4MCExpr.h - VideoCore4 specific MC expression classes ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ARTKMCEXPR_H
#define LLVM_LIB_TARGET_ARTKMCEXPR_H

#include "llvm/MC/MCAsmLayout.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCValue.h"

namespace llvm {

class VideoCore4MCExpr : public MCTargetExpr {
public:
  enum VideoCore4ExprKind {
    CEK_None,

    CEK_GPREL,
    CEK_GOT,
    CEK_PC16,
    CEK_PC16_T368,
    CEK_GOT_CALL,
    CEK_GOT_HI16,
    CEK_GOT_LO16,

    CEK_ABS_RVHI,
    CEK_ABS_RVLO,
    CEK_PCREL_RVBR,
    CEK_PCREL_RVJMP,
    CEK_PCREL_RVCALL,
    CEK_PCREL_RVHI,
    CEK_PCREL_RVLO,

    CEK_Special,
  };

private:
  const VideoCore4ExprKind Kind;
  const MCExpr            *Expr;

  explicit VideoCore4MCExpr(VideoCore4ExprKind Kind, const MCExpr *Expr)
    : Kind(Kind), Expr(Expr) {}

public:
  static const VideoCore4MCExpr *create(VideoCore4ExprKind Kind, const MCExpr *Expr,
					MCContext &Ctx);
  static const VideoCore4MCExpr *create(const MCSymbol *Symbol, 
					VideoCore4MCExpr::VideoCore4ExprKind Kind, MCContext &Ctx);
  static const VideoCore4MCExpr *createGpOff(VideoCore4ExprKind Kind, const MCExpr *Expr,
					     MCContext &Ctx);

  /// Get the kind of this expression.
  VideoCore4ExprKind getKind() const { return Kind; }

  /// Get the child of this expression.
  const MCExpr *getSubExpr() const { return Expr; }

  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;
  bool evaluateAsRelocatableImpl(MCValue &Res, const MCAsmLayout *Layout,
                                 const MCFixup *Fixup) const override;
  void visitUsedExpr(MCStreamer &Streamer) const override;
  MCFragment *findAssociatedFragment() const override {
    return getSubExpr()->findAssociatedFragment();
  }

  void fixELFSymbolsInTLSFixups(MCAssembler &Asm) const override;

  static bool classof(const MCExpr *E) {
    return E->getKind() == MCExpr::Target;
  }

  bool isGpOff(VideoCore4ExprKind &Kind) const;
  bool isGpOff() const {
    VideoCore4ExprKind Kind;
    return isGpOff(Kind);
  }
};
} // end namespace llvm

#endif
