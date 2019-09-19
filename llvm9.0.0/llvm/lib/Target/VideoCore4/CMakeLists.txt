set(LLVM_TARGET_DEFINITIONS VideoCore4.td)

tablegen(LLVM VideoCore4GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM VideoCore4GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM VideoCore4GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM VideoCore4GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM VideoCore4GenCallingConv.inc -gen-callingconv)
tablegen(LLVM VideoCore4GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(VideoCore4CommonTableGen)

add_llvm_target(VideoCore4CodeGen
  VideoCore4AllocaHoisting.cpp
  VideoCore4AsmPrinter.cpp
  VideoCore4BoolRetToInt.cpp
  VideoCore4CFGOptimizer.cpp
  VideoCore4DelaySlotFiller.cpp 
  VideoCore4DelUseLessJmp.cpp
  VideoCore4FrameLowering.cpp
  VideoCore4InstrInfo.cpp
  VideoCore4ISelDAGToDAG.cpp
  VideoCore4ISelLowering.cpp
  VideoCore4LowerIntrinsics.cpp
  VideoCore4MachineFunctionInfo.cpp
  VideoCore4MCInstLower.cpp
  VideoCore4RegisterInfo.cpp
  VideoCore4PseudoFixup.cpp
  VideoCore4SelectionDAGInfo.cpp
  VideoCore4Subtarget.cpp
  VideoCore4TargetMachine.cpp
  VideoCore4TargetObjectFile.cpp
)

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(InstPrinter)
