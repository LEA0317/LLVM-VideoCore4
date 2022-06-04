//===-- VideoCore4MCAsmInfo.h - VideoCore4 Asm Info ------------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the VideoCore4MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_VIDEOCORE4MCASMINFO_H
#define LLVM_LIB_TARGET_VIDEOCORE4MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class Triple;

  class VideoCore4MCAsmInfo : public MCAsmInfoELF {
    void anchor() override;
  public:
    explicit VideoCore4MCAsmInfo(const Triple &TheTriple);
  };

} // namespace llvm
#endif

