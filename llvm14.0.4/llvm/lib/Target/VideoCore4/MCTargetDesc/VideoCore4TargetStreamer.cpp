//===-- ViodeCore4TargetStreamer.cpp - VideoCore4 Target Streamer Methods -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides VideoCore4 specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4InstPrinter.h"
#include "VideoCore4MCTargetDesc.h"
#include "VideoCore4TargetStreamer.h"
#include "VideoCore4MCCodeEmitter.h"

#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/MC/MCAsmBackend.h"

#define DEBUG_TYPE "videocore4-target-streamer"

using namespace llvm;

VideoCore4TargetStreamer::VideoCore4TargetStreamer(MCStreamer &S)
  : MCTargetStreamer(S) {}

VideoCore4TargetAsmStreamer::VideoCore4TargetAsmStreamer(MCStreamer            &S,
							 formatted_raw_ostream &OS)
  : VideoCore4TargetStreamer(S) {}

VideoCore4TargetELFStreamer::VideoCore4TargetELFStreamer(MCStreamer            &S,
							 const MCSubtargetInfo &STI)
  : VideoCore4TargetStreamer(S)
{
  MCAssembler &MCA = static_cast<MCELFStreamer&>(S).getAssembler();

  unsigned EFlags = MCA.getELFHeaderEFlags();

  EFlags = 0; //(EFlags & ~ELF::EF_ATK_ISA) | ELF::EF_ATK_ISA_V2;

  MCA.setELFHeaderEFlags(EFlags);
}

MCELFStreamer&
VideoCore4TargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer&>(Streamer);
}

void
VideoCore4TargetELFStreamer::finish() {
#if 0
  MCAssembler            &MCA = getStreamer().getAssembler();
  const MCObjectFileInfo &OFI = *MCA.getContext().getObjectFileInfo();

  // .bss, .text and .data are always at least 16-byte aligned.
  MCSection &TextSection = *OFI.getTextSection();
  MCA.registerSection(TextSection);
  MCSection &DataSection = *OFI.getDataSection();
  MCA.registerSection(DataSection);
  MCSection &BSSSection = *OFI.getBSSSection();
  MCA.registerSection(BSSSection);

  TextSection.setAlignment(Align(std::max(4u, TextSection.getAlignment())));
  DataSection.setAlignment(Align(std::max(4u, DataSection.getAlignment())));
  BSSSection.setAlignment (Align(std::max(4u, BSSSection.getAlignment())));
#endif
}

class VideoCore4ELFStreamer : public MCELFStreamer {
public:
  VideoCore4ELFStreamer(MCContext                      &Context,
			std::unique_ptr<MCAsmBackend>   MAB,
			std::unique_ptr<MCObjectWriter> OW,
			std::unique_ptr<MCCodeEmitter>  Emitter)
    : MCELFStreamer(Context, std::move(MAB), std::move(OW), std::move(Emitter)) {}
    
  void emitValueToOffset(const MCExpr *Offset,
			 unsigned char Value,
			 SMLoc         Loc) override {
    FlushInstruction();
    MCELFStreamer::emitValueToOffset(Offset, Value, Loc);
  }

private:
  void FlushInstruction() {
    assert(SlotInst.size() == 0 && "Unexpected behavior");
    return;
  }
  
  SmallVector<MCInst, 8> SlotInst;
};

namespace llvm {
  MCELFStreamer*
  createVideoCore4ELFStreamer(MCContext                      &Context,
			      std::unique_ptr<MCAsmBackend>   MAB,
			      std::unique_ptr<MCObjectWriter> OW,
			      std::unique_ptr<MCCodeEmitter>  Emitter,
			      bool                            RelaxAll) {
    return new VideoCore4ELFStreamer(Context, std::move(MAB), std::move(OW), std::move(Emitter));
  }
}

