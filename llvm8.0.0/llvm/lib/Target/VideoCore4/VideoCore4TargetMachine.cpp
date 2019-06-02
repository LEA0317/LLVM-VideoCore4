//===-- VideoCore4TargetMachine.cpp - Define TargetMachine for VideoCore4 -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Top-level implementation for the VideoCore4 target.
//
//===----------------------------------------------------------------------===//

#include "VideoCore4TargetMachine.h"
#include "VideoCore4.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

using namespace llvm;

extern "C" void LLVMInitializeVideoCore4Target() {
  // Register the target.
  RegisterTargetMachine<VideoCore4TargetMachine> X(TheVideoCore4Target);
}

static Reloc::Model
getEffectiveRelocModel(bool                   isJIT,
                       Optional<Reloc::Model> RM) {
  if (!RM.hasValue() || isJIT)
    return Reloc::Static;
  return *RM;
}

static CodeModel::Model
getVideoCore4EffectiveCodeModel(Optional<CodeModel::Model> CM) {
  if (CM)
    return *CM;
  return CodeModel::Small;
}

static std::string
computeDataLayout(const Triple        &TT,
                  StringRef            CPU,
                  const TargetOptions &Options,
                  bool                 isLittle) {
  std::string Ret = "";

  if (isLittle)
    Ret += "e";
  else
    Ret += "E";

  Ret += "-m:m";
  Ret += "-p:32:32";
  Ret += "-i8:8:32-i16:16:32";
  Ret += "-n32-S32";

  return Ret;
}

VideoCore4TargetMachine::VideoCore4TargetMachine(const Target &T,
						 const Triple &TT,
						 StringRef CPU,
						 StringRef FS,
						 const TargetOptions &Options,
						 Optional<Reloc::Model> RM,
						 Optional<CodeModel::Model> CM,
						 CodeGenOpt::Level OL,
						 bool JIT)
  : LLVMTargetMachine(T, computeDataLayout(TT,
					   CPU,
					   Options,
					   true/*isLittle*/),
		      TT, CPU, FS, Options,
		      getEffectiveRelocModel(JIT, RM),
		      getVideoCore4EffectiveCodeModel(CM), OL),
    Subtarget(TT, CPU, FS),
    // FIXME: Check TargetData string.
    DL("e-p:32:32:32-i8:8:8-i16:16:16-i32:32:32-f32:32:32-n32-S32"),
    InstrInfo(*this), TLInfo(*this), TSInfo(*this),
    FrameLowering(Subtarget) {
	initAsmInfo();
    }

namespace {
/// VideoCore4 Code Generator Pass Configuration Options.
class VideoCore4PassConfig : public TargetPassConfig {
public:
  VideoCore4PassConfig(VideoCore4TargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  VideoCore4TargetMachine &getVideoCore4TargetMachine() const {
    return getTM<VideoCore4TargetMachine>();
  }

  bool addILPOpts()                override;
  bool addInstSelector()           override;
  void addIRPasses()               override;
  void addMachineSSAOptimization() override;
  void addPreEmitPass()            override;
  bool addPreISel()                override;
  void addPreRegAlloc()            override;
  void addPreSched2()              override;
};
} // namespace

TargetPassConfig *VideoCore4TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new VideoCore4PassConfig(*this, PM);
}

bool
VideoCore4PassConfig::addILPOpts() {
  addPass(&MachineCombinerID);
  addPass(&EarlyIfConverterID);
  return true;
}

void
VideoCore4PassConfig::addMachineSSAOptimization() {
  addPass(&OptimizePHIsID);
  addPass(&StackColoringID);
  addPass(&LocalStackSlotAllocationID);
  addPass(&DeadMachineInstructionElimID);
  addPass(&MachineLICMID);
  addPass(&MachineCSEID);
  addPass(&MachineSinkingID);
  addPass(&PeepholeOptimizerID);

  TargetPassConfig::addMachineSSAOptimization();
}

bool
VideoCore4PassConfig::addPreISel() {
  addPass(createGlobalMergePass(TM, 4095, false));
  return false;
}

void
VideoCore4PassConfig::addIRPasses() {
  addPass(createAtomicExpandPass());
  addPass(createCFGSimplificationPass());
  addPass(createLoopDataPrefetchPass());
  addPass(createSROAPass());
  addPass(createSeparateConstOffsetFromGEPPass());
  addPass(createStraightLineStrengthReducePass());
  addPass(createGVNPass());
  addPass(createNaryReassociatePass());
  addPass(createEarlyCSEPass());
  addPass(createSROAPass());

  TargetPassConfig::addIRPasses();

  addPass(createInterleavedAccessPass());
  addPass(createSeparateConstOffsetFromGEPPass());
  addPass(createEarlyCSEPass());
  addPass(createLICMPass());
}

void
VideoCore4PassConfig::addPreSched2() {
  addPass(&IfConverterID);
}

bool
VideoCore4PassConfig::addInstSelector() {
  addPass(createVideoCore4ISelDag(getVideoCore4TargetMachine(), getOptLevel()));
  return false;
}

void
VideoCore4PassConfig::addPreRegAlloc() {
  addPass(&PeepholeOptimizerID);
  addPass(&LiveRangeShrinkID);
  return;
}

void
VideoCore4PassConfig::addPreEmitPass() {
  return;
}
