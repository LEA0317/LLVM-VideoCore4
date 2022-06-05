//===- VideoCore4TargetStreamer.h - VideoCore4TargetStreamer class --*- C++ -*---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VIDEOCORE4TARGETSTREAMER_H
#define LLVM_LIB_TARGET_VIDEOCORE4TARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/FormattedStream.h"

namespace llvm {
  class MCELFStreamer;
  class raw_pwrite_stream;

  class VideoCore4TargetStreamer: public MCTargetStreamer {
  public:
    explicit VideoCore4TargetStreamer(MCStreamer &S);
  };

  class VideoCore4TargetAsmStreamer: public VideoCore4TargetStreamer {
  public:
    explicit VideoCore4TargetAsmStreamer(MCStreamer            &S,
					 formatted_raw_ostream &OS);
  };

  class VideoCore4TargetELFStreamer: public VideoCore4TargetStreamer {
  public:
    VideoCore4TargetELFStreamer(MCStreamer            &S,
				const MCSubtargetInfo &STI);
    MCELFStreamer &getStreamer();
    void finish() override;
  };
  
  MCELFStreamer* createVideoCore4ELFStreamer(MCContext                      &Context,
					     std::unique_ptr<MCAsmBackend>   MAB,
					     std::unique_ptr<MCObjectWriter> OW,
					     std::unique_ptr<MCCodeEmitter>  Emitter,
					     bool                            RelaxAll);
} // namespace llvm

#endif
