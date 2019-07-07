#ifndef __HEADER_VIDEOCORE4_UTIL__
#define __HEADER_VIDEOCORE4_UTIL__

#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#define BRANCH_KIND_NUM 22

inline unsigned
reverseBranch(unsigned opcode) {
  switch (opcode) {
  case llvm::VideoCore4::JMP_CC_EQ:
    return llvm::VideoCore4::JMP_CC_NE;
  case llvm::VideoCore4::JMP_CC_NE:
    return llvm::VideoCore4::JMP_CC_EQ;
  case llvm::VideoCore4::JMP_CC_GT:
    return llvm::VideoCore4::JMP_CC_LE;
  case llvm::VideoCore4::JMP_CC_GE:
    return llvm::VideoCore4::JMP_CC_LT;
  case llvm::VideoCore4::JMP_CC_LT:
    return llvm::VideoCore4::JMP_CC_GE;
  case llvm::VideoCore4::JMP_CC_LE:
    return llvm::VideoCore4::JMP_CC_GT;
  case llvm::VideoCore4::JMP_CC_HI:
    return llvm::VideoCore4::JMP_CC_LS;
  case llvm::VideoCore4::JMP_CC_HS:
    return llvm::VideoCore4::JMP_CC_LO;
  case llvm::VideoCore4::JMP_CC_LO:
    return llvm::VideoCore4::JMP_CC_HS;
  case llvm::VideoCore4::JMP_CC_LS:
    return llvm::VideoCore4::JMP_CC_HI;
  default:
    llvm_unreachable("cannot handle this branch");
  }

  // error
  return UINT_MAX;
}

inline bool isRawBranch(unsigned opcode) {
  switch (opcode) {
  case llvm::VideoCore4::JMP_CC_EQ:
  case llvm::VideoCore4::JMP_CC_NE:
  case llvm::VideoCore4::JMP_CC_GT:
  case llvm::VideoCore4::JMP_CC_GE:
  case llvm::VideoCore4::JMP_CC_LT:
  case llvm::VideoCore4::JMP_CC_LE:
  case llvm::VideoCore4::JMP_CC_HI:
  case llvm::VideoCore4::JMP_CC_HS:
  case llvm::VideoCore4::JMP_CC_LO:
  case llvm::VideoCore4::JMP_CC_LS:
    return true;
  default:
    return false;
  }

  // error
  return false;
}

inline unsigned
reverseCmovConditon(unsigned opcode) {
  switch (opcode) {
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
  llvm::VideoCore4::JMP_COMP_LS_P,
  llvm::VideoCore4::JMP_FCOMP_OEQ_P,
  llvm::VideoCore4::JMP_FCOMP_ONE_P,
  llvm::VideoCore4::JMP_FCOMP_OGT_P,
  llvm::VideoCore4::JMP_FCOMP_OGE_P,
  llvm::VideoCore4::JMP_FCOMP_OLT_P,
  llvm::VideoCore4::JMP_FCOMP_OLE_P,
  llvm::VideoCore4::JMP_FCOMP_UEQ_P,
  llvm::VideoCore4::JMP_FCOMP_UNE_P,
  llvm::VideoCore4::JMP_FCOMP_UGT_P,
  llvm::VideoCore4::JMP_FCOMP_UGE_P,
  llvm::VideoCore4::JMP_FCOMP_ULT_P,
  llvm::VideoCore4::JMP_FCOMP_ULE_P
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
  llvm::VideoCore4::JMP_COMP_LS_F_P,
  llvm::VideoCore4::JMP_FCOMP_OEQ_F_P,
  llvm::VideoCore4::JMP_FCOMP_ONE_F_P,
  llvm::VideoCore4::JMP_FCOMP_OGT_F_P,
  llvm::VideoCore4::JMP_FCOMP_OGE_F_P,
  llvm::VideoCore4::JMP_FCOMP_OLT_F_P,
  llvm::VideoCore4::JMP_FCOMP_OLE_F_P,
  llvm::VideoCore4::JMP_FCOMP_UEQ_F_P,
  llvm::VideoCore4::JMP_FCOMP_UNE_F_P,
  llvm::VideoCore4::JMP_FCOMP_UGT_F_P,
  llvm::VideoCore4::JMP_FCOMP_UGE_F_P,
  llvm::VideoCore4::JMP_FCOMP_ULT_F_P,
  llvm::VideoCore4::JMP_FCOMP_ULE_F_P
};

inline bool isCondTrueBranch(unsigned opcode) {
  for (int i=0; i<BRANCH_KIND_NUM; i++) {
    if (BranchTakenOpcode[i] == opcode) return true;
  }
  return false;
}

inline bool isCondFalseBranch(unsigned opcode) {
  for (int i=0; i<BRANCH_KIND_NUM; i++) {
    if (BranchNotTakenOpcode[i] == opcode) return true;
  }
  return false;
}

inline bool isCondBranch(unsigned opcode) {
  if (isCondTrueBranch(opcode) || isCondFalseBranch(opcode)) return true;
  return false;
}

inline bool
isUnconditionalJump(int opcode) {
  return (opcode    == llvm::VideoCore4::JMP
	  || opcode == llvm::VideoCore4::JMP_R);
}

inline bool
isCall(int opcode) {
  return (opcode    == llvm::VideoCore4::CALL
	  || opcode == llvm::VideoCore4::CALL_R);
}

inline bool isBranch(unsigned opcode) {
  if (isCondBranch(opcode)
      || isUnconditionalJump(opcode)
      || isRawBranch(opcode)) return true;
  return false;
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
