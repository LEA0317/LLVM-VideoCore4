//===-- VideoCore4AsmBackend.h - VideoCore4 Asm Backend  ------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the VideoCore4AsmBackend class.
//
//===----------------------------------------------------------------------===//
//

#ifndef LLVM_LIB_TARGET_VIDEOCORE4ASMBACKEND_H
#define LLVM_LIB_TARGET_VIDEOCORE4ASMBACKEND_H

#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCTargetOptions.h"

namespace llvm {

class MCAssembler;
struct MCFixupKindInfo;
class Target;
class MCObjectWriter;

class VideoCore4AsmBackend : public MCAsmBackend {
  Triple::OSType OSType;

public:
  VideoCore4AsmBackend(const Target &T, Triple::OSType _OSType)
    : MCAsmBackend(support::little), OSType(_OSType) {}

  std::unique_ptr<llvm::MCObjectTargetWriter> createObjectTargetWriter() const override;

  void applyFixup(const MCAssembler     &Asm,
		  const MCFixup         &Fixup,
                  const MCValue         &Target,
		  MutableArrayRef<char>  Data,
                  uint64_t               Value,
		  bool                   IsResolved,
                  const MCSubtargetInfo *STI) const override;
  
  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override;

  unsigned getNumFixupKinds() const override {
    return 0; // FIXME
  }

  /// MayNeedRelaxation - Check whether the given instruction may need
  /// relaxation.
  ///
  /// \param Inst - The instruction to test.
  bool mayNeedRelaxation(const MCInst          &Inst,
                         const MCSubtargetInfo &STI) const override {
    return false;
  }

  bool shouldForceRelocation(const MCAssembler &Asm,
                             const MCFixup     &Fixup,
                             const MCValue     &Target) override;

  /// fixupNeedsRelaxation - Target specific predicate for whether a given
  /// fixup requires the associated instruction to be relaxed.
   bool fixupNeedsRelaxation(const MCFixup             &Fixup,
			     uint64_t                   Value,
                             const MCRelaxableFragment *DF,
                             const MCAsmLayout         &Layout) const override {
    // FIXME.
    llvm_unreachable("RelaxInstruction() unimplemented");
    return false;
  }

  /// RelaxInstruction - Relax the instruction in the given fragment
  /// to the next wider instruction.
  ///
  /// \param Inst - The instruction to relax, which may be the same
  /// as the output.
  /// \param [out] Res On return, the relaxed instruction.
  void
  relaxInstruction(MCInst                &Inst,
		   const MCSubtargetInfo &STI) const override {};

  bool
  writeNopData(raw_ostream           &OS,
	       uint64_t               Count,
	       const MCSubtargetInfo *STI) const override;
}; // class VideoCore4AsmBackend

} // namespace

#endif

