//===-- VideoCore4ELFObjectWriter.cpp - VideoCore4 ELF Writer -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4MCTargetDesc.h"

#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/MCObjectWriter.h"

#include <list>
#include <memory>

using namespace llvm;

namespace {
  class VideoCore4ELFObjectWriter : public MCELFObjectTargetWriter {
  public:
    VideoCore4ELFObjectWriter(uint8_t OSABI);

    ~VideoCore4ELFObjectWriter() override;

    unsigned getRelocType(MCContext     &Ctx,
			  const MCValue &Target,
			  const MCFixup &Fixup,
			  bool           IsPCRel) const override;
    bool needsRelocateWithSymbol(const MCSymbol &Sym,
                                 unsigned        Type) const override;
  };
}

VideoCore4ELFObjectWriter::VideoCore4ELFObjectWriter(uint8_t OSABI)
  : MCELFObjectTargetWriter(/*_is64Bit=false*/ false,
			    OSABI,
			    ELF::EM_VIDEOCORE,
                            /*HasRelocationAddend*/ false) {}

VideoCore4ELFObjectWriter::~VideoCore4ELFObjectWriter() {}

unsigned
VideoCore4ELFObjectWriter::getRelocType(MCContext     &Ctx,
					const MCValue &Target,
					const MCFixup &Fixup,
					bool           IsPCRel) const {
  // determine the type of the relocation
  unsigned Type = (unsigned)0; // FIXME ELF::R_ATK_NONE;
  unsigned Kind = (unsigned)Fixup.getKind();

  switch (Kind) {
  default:
    {
      //llvm_unreachable("invalid fixup kind!");
      break;
    }
  }

  return Type;
}

bool
VideoCore4ELFObjectWriter::needsRelocateWithSymbol(const MCSymbol &Sym,
						   unsigned        Type) const {
  // FIXME: This is extremelly conservative. This really needs to use a
  // whitelist with a clear explanation for why each realocation needs to
  // point to the symbol, not to the section.
  switch (Type) {
  default:
    {
      return false;
    }
  }
}

namespace llvm {
std::unique_ptr<MCObjectTargetWriter>
createVideoCore4ELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<VideoCore4ELFObjectWriter>(OSABI);
}
}
