//===-- VideoCore4MCAsmInfo.cpp - VideoCore4 Asm Properties ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the VideoCore4MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4MCAsmInfo.h"
#include "llvm/ADT/Triple.h"

using namespace llvm;

void VideoCore4MCAsmInfo::anchor() {}

VideoCore4MCAsmInfo::VideoCore4MCAsmInfo(const Triple &TheTriple) {
  IsLittleEndian = false; // the default of IsLittleEndian is false

  AlignmentIsInBytes      = false;
  Data16bitsDirective     = "\t.short\t";
  Data32bitsDirective     = "\t.long\t";
  
  PrivateGlobalPrefix     = "$";
  PrivateLabelPrefix      = "";
  CommentString           = "#";
  ZeroDirective           = "\t.space\t";
  GPRel32Directive        = "\t.gpword\t";
  WeakRefDirective        = "\t.weak\t";
  UseAssignmentForEHBegin = true;

  SupportsDebugInformation = true;
  ExceptionsType           = ExceptionHandling::None; //ExceptionHandling::DwarfCFI;
  DwarfRegNumForCFI        = false;
}

