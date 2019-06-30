#ifndef __HEADER_VIDEOCORE4_UTIL__
#define __HEADER_VIDEOCORE4_UTIL__

#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#define BRANCH_KIND_NUM 10


inline unsigned
reverseCmovConditon(unsigned Opcode) {
  switch (Opcode) {
  case llvm::VideoCore4::CMOV_EQ_RR_P:
    return llvm::VideoCore4::CMOV_NE_RR_P;
  case llvm::VideoCore4::CMOV_NE_RR_P:
    return llvm::VideoCore4::CMOV_EQ_RR_P;
  case llvm::VideoCore4::CMOV_GT_RR_P:
    return llvm::VideoCore4::CMOV_LE_RR_P;
  case llvm::VideoCore4::CMOV_GE_RR_P:
    return llvm::VideoCore4::CMOV_LT_RR_P;
  case llvm::VideoCore4::CMOV_LT_RR_P:
    return llvm::VideoCore4::CMOV_GE_RR_P;
  case llvm::VideoCore4::CMOV_LE_RR_P:
    return llvm::VideoCore4::CMOV_GT_RR_P;
  case llvm::VideoCore4::CMOV_HI_RR_P:
    return llvm::VideoCore4::CMOV_LS_RR_P;
  case llvm::VideoCore4::CMOV_HS_RR_P:
    return llvm::VideoCore4::CMOV_LO_RR_P;
  case llvm::VideoCore4::CMOV_LO_RR_P:
    return llvm::VideoCore4::CMOV_HS_RR_P;
  case llvm::VideoCore4::CMOV_LS_RR_P:
    return llvm::VideoCore4::CMOV_HI_RR_P;
  default:
    llvm_unreachable("cannot handle this conditional mov");
  }

  // error
  return UINT_MAX;
}


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

inline bool
IsUnconditionalJump(int Opc) {
  return (Opc    == llvm::VideoCore4::JMP
	  || Opc == llvm::VideoCore4::JMP_R);
}

inline unsigned
reverseBranchCondition(llvm::MachineInstr *mi) {
  unsigned reverseOpc = UINT_MAX;
  unsigned opc        = mi->getOpcode();

  for (int i=0; i<BRANCH_KIND_NUM; i++) {
    if (opc == BranchTakenOpcode[i]) {
      reverseOpc = BranchNotTakenOpcode[i];
      break;
    } else if (opc == BranchNotTakenOpcode[i]) {
      reverseOpc  = BranchTakenOpcode[i];
      break;
    }
  }

  if (reverseOpc == UINT_MAX) {
    mi->dump();
    llvm_unreachable("cannot handle this branch");
  }

  return reverseOpc;
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
