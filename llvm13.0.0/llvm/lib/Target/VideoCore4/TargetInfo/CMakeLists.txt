add_llvm_component_library(LLVMVideoCore4Info
  VideoCore4TargetInfo.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  VideoCore4
  )
