add_llvm_component_library(LLVMVideoCore4Desc
  VideoCore4InstPrinter.cpp
  VideoCore4MCAsmInfo.cpp
  VideoCore4MCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  VideoCore4Info
  Support

  ADD_TO_COMPONENT
  VideoCore4
  )
