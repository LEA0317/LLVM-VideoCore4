//===-- VideoCore4TargetObjectFile.cpp - VideoCore4 Object Info -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4TargetObjectFile.h"
#include "VideoCore4TargetMachine.h"

using namespace llvm;

void VideoCore4ELFTargetObjectFile::Initialize(MCContext           &Ctx,
					       const TargetMachine &TM) {
  TargetLoweringObjectFileELF::Initialize(Ctx, TM);
  InitializeELF(TM.Options.UseInitArray);
}
