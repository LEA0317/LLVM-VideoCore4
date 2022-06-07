//===-- VideoCore4AsmBackend.cpp - VideoCore4 Asm Backend  ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the VideoCore4AsmBackend class.
//
//===----------------------------------------------------------------------===//
//

#include "VideoCore4AsmBackend.h"
#include "VideoCore4MCTargetDesc.h"
#include "VideoCore4FixupKinds.h"

#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>

using namespace llvm;

// Prepare value for the target space for it
#if 0
static unsigned
adjustFixupValue(const MCFixup &Fixup,
		 uint64_t       Value,
		 MCContext     *Ctx = nullptr) {
  unsigned long Kind = Fixup.getKind();

  switch (Kind) {
  default:
    {
      break;
    }
  }
  return Value;
}
#endif

std::unique_ptr<llvm::MCObjectTargetWriter>
VideoCore4AsmBackend::createObjectTargetWriter() const {
  return createVideoCore4ELFObjectWriter(MCELFObjectTargetWriter::getOSABI(OSType));
}

/// ApplyFixup - Apply the \p Value for given \p Fixup into the provided
/// data fragment, at the offset specified by the fixup and following the
/// fixup kind as appropriate.
void
VideoCore4AsmBackend::applyFixup(const MCAssembler     &Asm,
				 const MCFixup         &Fixup,
				 const MCValue         &Target,
				 MutableArrayRef<char>  Data,
				 uint64_t               Value,
				 bool                   IsResolved,
				 const MCSubtargetInfo *STI) const {
#if 0
  MCFixupKind Kind = Fixup.getKind();
#endif

  if (!Value) {
    return; // Doesn't change encoding.
  }
}

const MCFixupKindInfo&
VideoCore4AsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  if (Kind < FirstTargetFixupKind) {
    return MCAsmBackend::getFixupKindInfo(Kind);
  }

  const static MCFixupKindInfo Infos[VideoCore4::NumTargetFixupKinds] =
    {
      // This table *must* be in same the order of fixup_* kinds in
      // VideoCore4FixupKinds.h.
      //
      // name                        offset  bits  flags
      { "fixup_VideoCore4_32",            0,    0,     0 }, // FIXME
    };
#if 0
  assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
         "Invalid kind!");
  return Infos[Kind - FirstTargetFixupKind];
#else
  return Infos[0];
#endif
}

/// WriteNopData - Write an (optimal) nop sequence of Count bytes
/// to the given output. If the target cannot generate such a sequence,
/// it should return an error.
///
/// \return - True on success.
bool
VideoCore4AsmBackend::writeNopData(raw_ostream &OS, uint64_t Count, const MCSubtargetInfo *STI) const {
  return true;
}

// MCAsmBackend
MCAsmBackend*
llvm::createVideoCore4AsmBackend(const Target          &T,
				 const MCSubtargetInfo &STI,
				 const MCRegisterInfo  &MRI,
				 const MCTargetOptions &Option) {
  return new VideoCore4AsmBackend(T, STI.getTargetTriple().getOS());
}

// If linker relaxation is enabled, or the relax option had previously been
// enabled, always emit relocations even if the fixup can be resolved. This is
// necessary for correctness as offsets may change during relaxation.
bool VideoCore4AsmBackend::shouldForceRelocation(const MCAssembler &Asm,
						 const MCFixup     &Fixup,
						 const MCValue     &Target) {
  if (Fixup.getKind() >= FirstLiteralRelocationKind)
    return true;
  switch (Fixup.getTargetKind()) {
  default:
    break;
  }

  return false /*ForceRelocs*/;
}
