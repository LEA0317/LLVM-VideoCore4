#ifndef __HEADER_VIDEOCORE4_UTIL__
#define __HEADER_VIDEOCORE4_UTIL__

#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#define BRANCH_KIND_NUM 10
static unsigned BranchTakenOpcode[BRANCH_KIND_NUM] = {
  llvm::VideoCore4::JMP_COMP_EQ_P,
  llvm::VideoCore4::JMP_COMP_NE_P,
  llvm::VideoCore4::JMP_COMP_GT_P,
  llvm::VideoCore4::JMP_COMP_GE_P,
  llvm::VideoCore4::JMP_COMP_LT_P,
  llvm::VideoCore4::JMP_COMP_LE_P,
  llvm::VideoCore4::JMP_COMP_HI_P,
  llvm::VideoCore4::JMP_COMP_HS_P,
  llvm::VideoCore4::JMP_COMP_LO_P,
  llvm::VideoCore4::JMP_COMP_LS_P
};
static unsigned BranchNotTakenOpcode[BRANCH_KIND_NUM] = {
  llvm::VideoCore4::JMP_COMP_EQ_F_P,
  llvm::VideoCore4::JMP_COMP_NE_F_P,
  llvm::VideoCore4::JMP_COMP_GT_F_P,
  llvm::VideoCore4::JMP_COMP_GE_F_P,
  llvm::VideoCore4::JMP_COMP_LT_F_P,
  llvm::VideoCore4::JMP_COMP_LE_F_P,
  llvm::VideoCore4::JMP_COMP_HI_F_P,
  llvm::VideoCore4::JMP_COMP_HS_F_P,
  llvm::VideoCore4::JMP_COMP_LO_F_P,
  llvm::VideoCore4::JMP_COMP_LS_F_P
};

inline bool isCondTrueBranch(unsigned opc) {
  for (int i=0; i<BRANCH_KIND_NUM; i++) {
    if (BranchTakenOpcode[i] == opc) return true;
  }
  return false;
}

inline bool isCondFalseBranch(unsigned opc) {
  for (int i=0; i<BRANCH_KIND_NUM; i++) {
    if (BranchNotTakenOpcode[i] == opc) return true;
  }
  return false;
}

inline bool isCondBranch(unsigned opc) {
  if (isCondTrueBranch(opc) || isCondFalseBranch(opc)) return true;
  return false;
}

namespace CC {
  const int DUMMY = 0;
}

inline void
parseCondBranch(llvm::MachineInstr                          *LastInst,
                llvm::MachineBasicBlock                    *&Target,
                llvm::SmallVectorImpl<llvm::MachineOperand> &Cond) {
  Cond.push_back(llvm::MachineOperand::CreateImm(CC::DUMMY));
  Target = LastInst->getOperand(2).getMBB();
}

#endif
