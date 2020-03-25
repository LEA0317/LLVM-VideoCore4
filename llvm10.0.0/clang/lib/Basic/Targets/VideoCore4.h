//===--- VideoCore4.h - Declare VideoCore4 target feature support -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares VideoCore4 TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_VIDEOCORE_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_VIDEOCORE_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"
#include "llvm/ADT/StringSwitch.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY VideoCore4TargetInfo : public TargetInfo {
  enum class CPUKind {
    CK_Generic,
    CK_VC4,
  };
  CPUKind cpu;
  
  void setDataLayout() {
    StringRef Layout;

    Layout = "e-p:32:32:32-i8:8:8-i16:16:16-i32:32:32-f32:32:32-n32-S32";
    resetDataLayout(Layout.str());
  }

  static const Builtin::Info BuiltinInfo[];
  std::string                CPU;

public:
  VideoCore4TargetInfo(const llvm::Triple  &Triple,
		       const TargetOptions &Options)
    : TargetInfo(Triple), cpu(CPUKind::CK_Generic) {
    BigEndian    = false;
    TLSSupported = false;
    LongWidth    = LongAlign = 32;

    PointerWidth = PointerAlign = 32;
    SizeType     = TargetInfo::UnsignedInt;
    PtrDiffType  = IntPtrType = TargetInfo::SignedInt;
  }

  bool setCPU(const std::string &Name) override {
    cpu = llvm::StringSwitch<CPUKind>(Name)
      .Case("vc4", CPUKind::CK_VC4)
      .Default(CPUKind::CK_Generic);
    switch (cpu) {
    case CPUKind::CK_Generic:
      if (getTriple().getArch() != llvm::Triple::videocore) {
        return false;
      }
      return true;
    case CPUKind::CK_VC4:
      return true;
    }
    llvm_unreachable("Unhandled CPU kind");
  }

  const std::string &getCPU() const { return CPU; }

  bool
  initFeatureMap(llvm::StringMap<bool>          &Features,
		 DiagnosticsEngine              &Diags,
                 StringRef                       CPU,
                 const std::vector<std::string> &FeaturesVec) const override {
    if (CPU.empty())
      CPU = getCPU();
    else
      Features[CPU] = true;
    return TargetInfo::initFeatureMap(Features, Diags, CPU, FeaturesVec);
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder      &Builder) const override;

  ArrayRef<Builtin::Info> getTargetBuiltins() const override;

  //bool hasFeature(StringRef Feature) const override;

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  ArrayRef<const char *> getGCCRegNames() const override {
    static const char *const GCCRegNames[] = {
      // GPR
      "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
      "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
      "r16",  "r17",  "r18",  "r19",  "r20",  "r21",  "r22",  "r23",
      "r24",  "r25",  "r26",  "r27",  "r28",  "r29",  "r30",  "r31",
    };
    return llvm::makeArrayRef(GCCRegNames);
  }

  // FIX ME (konda)
  bool validateAsmConstraint(const char                *&Name,
                             TargetInfo::ConstraintInfo &Info) const override {
    switch (*Name) {
    default:
      break;
    }
    return false;
  }

  std::string convertConstraint(const char *&Constraint) const override {
    return std::string(1, *Constraint);
  }

  const char *getClobbers() const override {
    return "";
  }

  bool handleTargetFeatures(std::vector<std::string> &Features,
                            DiagnosticsEngine        &Diags) override {
    setDataLayout();

    return true;
  }

  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
    return None;
  }
};
} // namespace targets
} // namespace clang

#endif // LLVM_CLANG_LIB_BASIC_TARGETS_VIDEOCORE_H
