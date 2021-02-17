//===-- VideoCore4AsmPrinter.h - Print machine code to an VideoCore4 .s file ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// VideoCore4 Assembly printer class.
//
//===----------------------------------------------------------------------===//

#ifndef VIDEOCORE4ASMPRINTER_H
#define VIDEOCORE4ASMPRINTER_H

#include "VideoCore4.h"
#include "VideoCore4TargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
  class VideoCore4AsmPrinter : public AsmPrinter {
    const VideoCore4Subtarget *Subtarget;
    
  public:
    explicit VideoCore4AsmPrinter(TargetMachine              &TM,
				  std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {
      Subtarget = static_cast<VideoCore4TargetMachine &>(TM).getSubtargetImpl();
    }
    
    virtual StringRef getPassName() const override {
      return "VideoCore4 Assembly Printer";
    }
    
    bool isBlockOnlyReachableByFallthrough(const MachineBasicBlock *MBB) const override;

    void emitInstruction(const MachineInstr *MI) override;

    void printOperand(const MachineInstr *MI,
		      unsigned            OpNo,
		      raw_ostream        &O);
    /*bool PrintAsmOperand(const MachineInstr *MI,
			 unsigned            OpNo,
			 const char         *ExtraCode,
			 raw_ostream        &OS);
    bool PrintAsmMemoryOperand(const MachineInstr *MI,
			       unsigned            OpNo,
			       const char         *ExtraCode,
			       raw_ostream        &OS);*/

    static const char *getRegisterName(unsigned RegNo);
  };

} // end of llvm namespace

#endif
