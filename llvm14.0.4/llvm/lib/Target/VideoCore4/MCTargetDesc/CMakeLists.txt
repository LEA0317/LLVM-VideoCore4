add_llvm_component_library(LLVMVideoCore4Desc
  VideoCore4InstPrinter.cpp
  VideoCore4MCAsmInfo.cpp
  VideoCore4AsmBackend.cpp
  VideoCore4MCExpr.cpp
  VideoCore4MCTargetDesc.cpp
  VideoCore4MCCodeEmitter.cpp
  VideoCore4ELFObjectWriter.cpp
  VideoCore4TargetStreamer.cpp

  LINK_COMPONENTS
  MC
  VideoCore4Info
  Support

  ADD_TO_COMPONENT
  VideoCore4
  )
