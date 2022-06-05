//===-- VideoCore4MCCodeEmitter.h - Convert VideoCore4 Code to Machine Code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the VideoCore4MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//
//

#ifndef LLVM_LIB_TARGET_VIDEOCORE4MCCODEEMITTER_H
#define LLVM_LIB_TARGET_VIDEOCORE4MCCODEEMITTER_H

#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/ADT/APInt.h"

using namespace llvm;

namespace llvm {
  class MCContext;
  class MCExpr;
  class MCInst;
  class MCInstrInfo;
  class MCFixup;
  class MCOperand;
  class MCSubtargetInfo;
  class raw_ostream;

class VideoCore4MCCodeEmitter : public MCCodeEmitter {
  VideoCore4MCCodeEmitter(const VideoCore4MCCodeEmitter &) = delete;
  void operator=(const VideoCore4MCCodeEmitter &) = delete;
  const MCInstrInfo &MCII;
  MCContext &Ctx;

public:
  VideoCore4MCCodeEmitter(const MCInstrInfo &mcii, MCContext &Ctx_, bool IsLittle)
    : MCII(mcii), Ctx(Ctx_) {}

  ~VideoCore4MCCodeEmitter() override {}

  const MCInstrInfo &getMCII() const { return MCII; };
    
  void EmitByte(unsigned char C,
		raw_ostream  &OS) const;

  void EmitInstruction(APInt        Val,
                       unsigned     Size,
		       raw_ostream &OS) const;

  void encodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  void getBinaryCodeForInstr(const MCInst             &MI,
			     SmallVectorImpl<MCFixup> &Fixups,
			     APInt                    &Inst,
			     APInt                    &Scratch,
			     const MCSubtargetInfo    &STI) const;

  unsigned getExprOpValue(const MCExpr             *Expr,
			  SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo    &STI) const;

  void getMachineOpValue(const MCInst             &MI,
                         const MCOperand          &MO,
                         APInt                    &op,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo    &STI) const;
}; // class VideoCore4MCCodeEmitter
} // namespace llvm.

#endif

