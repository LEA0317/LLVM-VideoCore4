//===--- VideoCore4.cpp - Implement VideoCore4 target feature support -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements VideoCore4 TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4.h"
#include "Targets.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/ADT/StringSwitch.h"

using namespace clang;
using namespace clang::targets;

const Builtin::Info VideoCore4TargetInfo::BuiltinInfo[] = {
#undef LIBBUILTIN
#define BUILTIN(ID, TYPE, ATTRS)                        \
  {#ID, TYPE, ATTRS, 0, ALL_LANGUAGES, nullptr},
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER)             \
  {#ID, TYPE, ATTRS, HEADER, ALL_LANGUAGES, nullptr},
#include "clang/Basic/BuiltinsVideoCore4.def"
};

void VideoCore4TargetInfo::getTargetDefines(const LangOptions &Opts,
					    MacroBuilder      &Builder) const {
  Builder.defineMacro("__VIDEOCORE_KERNEL__");
  switch (cpu) {
  default:
    break;
  case CPUKind::CK_VC4:
    defineCPUMacros(Builder, "vc4");
    break;
  }
}

ArrayRef<Builtin::Info> VideoCore4TargetInfo::getTargetBuiltins() const {
  return llvm::makeArrayRef(BuiltinInfo,
			    clang::VideoCore4::LastTSBuiltin - Builtin::FirstTSBuiltin);
}
