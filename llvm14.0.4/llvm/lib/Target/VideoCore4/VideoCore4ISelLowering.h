//===-- VideoCore4ISelLowering.h - VideoCore4 DAG Lowering Interface --*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that VideoCore4 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_VideoCore4_ISELLOWERING_H
#define LLVM_TARGET_VideoCore4_ISELLOWERING_H

#include "VideoCore4.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
  namespace VideoCore4ISD {
    enum {
      FIRST_NUMBER = ISD::BUILTIN_OP_END,

      /// Return with a flag operand. Operand 0 is the chain operand.
      RET_FLAG,
      
      /// CALL - These operations represent an abstract call instruction, which
      /// includes a bunch of information.
      CALL,
      
      /// Contains a globaladdr. Used to stop the compiler getting confused by
      /// recursive references.
      GLOBAL,
      
      BR_JT,

      JmpLink,

      RCP,
      RSQRT
    };
  }
  
  class VideoCore4Subtarget;
  class VideoCore4TargetMachine;
  
  class VideoCore4TargetLowering : public TargetLowering {
  public:
    explicit VideoCore4TargetLowering(const VideoCore4TargetMachine &TM);
    
    static VideoCore4TargetLowering*
    create(const VideoCore4TargetMachine &TM);
    
    /// LowerOperation - Provide custom lowering hooks for some operations.
    virtual SDValue LowerOperation(SDValue       Op,
				   SelectionDAG &DAG) const override;

    /// getTargetNodeName - This method returns the name of a target specific
    /// DAG node.
    virtual const char *getTargetNodeName(unsigned Opcode) const override;
    
  private:
    const VideoCore4Subtarget &Subtarget;
    const DataLayout          *TD;

    SDValue LowerVAARG(SDValue       Op,
		       SelectionDAG &DAG) const;
    SDValue LowerVASTART(SDValue       Op,
			 SelectionDAG &DAG) const;

    SDValue LowerBR_JT(SDValue       Op,
		       SelectionDAG &DAG) const;
    SDValue LowerJumpTable(SDValue       Op,
			   SelectionDAG &DAG) const;

    SDValue LowerCCCCallTo(SDValue                                Chain,
			   SDValue                                Callee,
                           CallingConv::ID                        CallConv,
			   bool                                   isVarArg,
                           bool                                   isTailCall,
                           const SmallVectorImpl<ISD::OutputArg> &Outs,
                           const SmallVectorImpl<SDValue>        &OutVals,
                           const SmallVectorImpl<ISD::InputArg>  &Ins,
                           const SDLoc                           &dl,
			   SelectionDAG                          &DAG,
                           SmallVectorImpl<SDValue>              &InVals) const;

    SDValue
    LowerCCCArguments(SDValue                               Chain,
		      CallingConv::ID                       CallConv,
		      bool                                  isVarArg,
		      const SmallVectorImpl<ISD::InputArg> &Ins,
		      const SDLoc                          &dl,
		      SelectionDAG                         &DAG,
		      SmallVectorImpl<SDValue>             &InVals) const;
    
    SDValue
    getRecipEstimate(SDValue       Operand,
                     SelectionDAG &DAG,
                     int           Enabled,
                     int          &RefinementSteps) const override;

    SDValue
    getSqrtEstimate(SDValue       Operand,
                    SelectionDAG &DAG,
                    int           Enabled,
                    int          &RefinementSteps,
                    bool         &UseOneConstNR,
                    bool          Reciprocal) const override;

    EVT
    getOptimalMemOpType(const MemOp         &Op,
			const AttributeList &FuncAttributes) const override;

    /*SDValue LowerCallResult(SDValue Chain, SDValue InFlag,
                            CallingConv::ID CallConv, bool isVarArg,
                            const SmallVectorImpl<ISD::InputArg> &Ins,
                            const SDLoc &dl, SelectionDAG &DAG,
                            SmallVectorImpl<SDValue> &InVals) const;*/

    SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                      SmallVectorImpl<SDValue>         &InVals) const override;

    SDValue LowerFormalArguments(SDValue                               Chain,
				 CallingConv::ID                       CallConv,
				 bool                                  IsVarArg,
				 const SmallVectorImpl<ISD::InputArg> &Ins,
				 const SDLoc                          &DL,
				 SelectionDAG                         &DAG,
				 SmallVectorImpl<SDValue>             &InVals) const override;

    SDValue LowerReturn(SDValue                                Chain,
                        CallingConv::ID                        CallConv,
			bool                                   isVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                        const SmallVectorImpl<SDValue>        &OutVals,
                        const SDLoc                           &dl,
			SelectionDAG                          &DAG) const override;
  };
} // namespace llvm

#endif // LLVM_TARGET_VIDEOCORE4_ISELLOWERING_H
