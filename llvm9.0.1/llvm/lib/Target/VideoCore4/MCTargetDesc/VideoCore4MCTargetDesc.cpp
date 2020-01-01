//===-- VideoCore4MCTargetDesc.cpp - VideoCore4 Target Descriptions ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides VideoCore4 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4MCTargetDesc.h"
#include "VideoCore4MCAsmInfo.h"
#include "InstPrinter/VideoCore4InstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "VideoCore4GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "VideoCore4GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "VideoCore4GenRegisterInfo.inc"

using namespace llvm;

static std::string selectVideoCoreArchFeature(const Triple &TT, StringRef CPU) {
  std::string VideoCoreArchFeature;
  if (CPU.empty() || CPU == "generic") {
    if (TT.getArch() == Triple::videocore) {
      if (CPU == "generic") {
        VideoCoreArchFeature = "+generic";        
      } else if (CPU == "vc4") {
        VideoCoreArchFeature = "+vc4";
      }
    }
  }
  return VideoCoreArchFeature;
}

static MCInstrInfo *createVideoCore4MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitVideoCore4MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createVideoCore4MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitVideoCore4MCRegisterInfo(X,
			       VideoCore4::PC);
  return X;
}

static MCSubtargetInfo *createVideoCore4MCSubtargetInfo(const Triple &TT, StringRef CPU,
							StringRef FS) {
  std::string ArchFS = selectVideoCoreArchFeature(TT,
						  CPU);
  if (!FS.empty()) {
    if (!ArchFS.empty())
      ArchFS = ArchFS + "," + FS.str();
    else
      ArchFS = FS;
  }
  return createVideoCore4MCSubtargetInfoImpl(TT, CPU, ArchFS);
}

static MCInstPrinter*
createVideoCore4MCInstPrinter(const Triple &T,
			      unsigned SyntaxVariant,
			      const MCAsmInfo &MAI,
			      const MCInstrInfo &MII,
			      const MCRegisterInfo &MRI) {
  return new VideoCore4InstPrinter(MAI,
				   MII,
				   MRI);
}

static MCAsmInfo*
createVideoCore4MCAsmInfo(const MCRegisterInfo &MRI,
			  const Triple         &TT) {
  MCAsmInfo       *MAI  = new VideoCore4MCAsmInfo(MRI, TT);
  unsigned         SP   = MRI.getDwarfRegNum(VideoCore4::SP,
                                             true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(nullptr,
                                                         SP,
                                                         0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

extern "C" void LLVMInitializeVideoCore4TargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(TheVideoCore4Target,
			createVideoCore4MCAsmInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheVideoCore4Target,
				      createVideoCore4MCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheVideoCore4Target,
                                    createVideoCore4MCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheVideoCore4Target,
                                          createVideoCore4MCSubtargetInfo);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(TheVideoCore4Target,
                                        createVideoCore4MCInstPrinter);
}
