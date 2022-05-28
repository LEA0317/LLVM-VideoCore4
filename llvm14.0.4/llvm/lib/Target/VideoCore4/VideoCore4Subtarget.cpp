//===-- VideoCore4Subtarget.cpp - VideoCore4 Subtarget Information -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the VideoCore4 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4.h"
#include "VideoCore4ISelLowering.h"
#include "VideoCore4RegisterInfo.h"
#include "VideoCore4Subtarget.h"
#include "VideoCore4FrameLowering.h"
#include "VideoCore4TargetMachine.h"

#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "vc4-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "VideoCore4GenSubtargetInfo.inc"

using namespace llvm;

VideoCore4Subtarget::VideoCore4Subtarget(const Triple                  &TT,
					 StringRef                      CPU,
					 StringRef                      FS,
					 const VideoCore4TargetMachine &_TM) :
  VideoCore4GenSubtargetInfo(TT, CPU, CPU, FS), TM(_TM), TSInfo(),
  InstrInfo(VideoCore4InstrInfo::create(initializeSubtargetDependencies(CPU,
									FS,
									TM))),
  FrameLowering(VideoCore4FrameLowering::create(*this)),
  TLInfo(VideoCore4TargetLowering::create(TM)) {
}

VideoCore4Subtarget&
VideoCore4Subtarget::initializeSubtargetDependencies(StringRef            CPU,
						     StringRef            FS,
						     const TargetMachine &TM) {
  if (CPU.empty()) {
    CPU = "generic";
  } else {
    CPU = "vc4";
  }

  InstrItins = getInstrItineraryForCPU(CPU);
  // Parse features string.
  ParseSubtargetFeatures(CPU,
			 CPU,
                         FS);

  return *this;
}
