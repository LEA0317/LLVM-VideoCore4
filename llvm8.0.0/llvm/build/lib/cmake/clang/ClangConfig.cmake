# This file allows users to call find_package(Clang) and pick up our targets.



find_package(LLVM REQUIRED CONFIG
             HINTS "/mnt/work/my_github/LLVM-VideoCORE4/llvm8.0.0/llvm/build/lib/cmake/llvm")

set(CLANG_EXPORTED_TARGETS "clangBasic;clangLex;clangParse;clangAST;clangDynamicASTMatchers;clangASTMatchers;clangCrossTU;clangSema;clangCodeGen;clangAnalysis;clangEdit;clangRewrite;clangARCMigrate;clangDriver;clangSerialization;clangRewriteFrontend;clangFrontend;clangFrontendTool;clangToolingCore;clangToolingInclusions;clangToolingRefactor;clangToolingASTDiff;clangTooling;clangIndex;clangStaticAnalyzerCore;clangStaticAnalyzerCheckers;clangStaticAnalyzerFrontend;clangFormat;clang;clang-format;clangHandleCXX;clangHandleLLVM;clang-import-test;clang-rename;clang-refactor;clangApplyReplacements;clang-apply-replacements;clangReorderFields;clang-reorder-fields;modularize;clangTidy;clangTidyAndroidModule;clangTidyAbseilModule;clangTidyBoostModule;clangTidyBugproneModule;clangTidyCERTModule;clangTidyCppCoreGuidelinesModule;clangTidyFuchsiaModule;clangTidyGoogleModule;clangTidyHICPPModule;clangTidyLLVMModule;clangTidyMiscModule;clangTidyModernizeModule;clangTidyMPIModule;clangTidyObjCModule;clangTidyPerformanceModule;clangTidyPlugin;clangTidyPortabilityModule;clangTidyReadabilityModule;clang-tidy;clangTidyUtils;clangTidyZirconModule;clangChangeNamespace;clangDoc;clangQuery;clangMove;clangDaemon;clangd;clangIncludeFixer;clangIncludeFixerPlugin;clang-include-fixer;findAllSymbols;libclang")
set(CLANG_CMAKE_DIR "/mnt/work/my_github/LLVM-VideoCORE4/llvm8.0.0/llvm/build/lib/cmake/clang")
set(CLANG_INCLUDE_DIRS "/mnt/work/my_github/LLVM-VideoCORE4/llvm8.0.0/clang/include;/mnt/work/my_github/LLVM-VideoCORE4/llvm8.0.0/llvm/build/tools/clang/include")

# Provide all our library targets to users.
include("/mnt/work/my_github/LLVM-VideoCORE4/llvm8.0.0/llvm/build/lib/cmake/clang/ClangTargets.cmake")

# By creating clang-tablegen-targets here, subprojects that depend on Clang's
# tablegen-generated headers can always depend on this target whether building
# in-tree with Clang or not.
if(NOT TARGET clang-tablegen-targets)
  add_custom_target(clang-tablegen-targets)
endif()
