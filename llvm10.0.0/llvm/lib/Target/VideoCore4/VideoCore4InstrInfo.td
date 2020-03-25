//===-- VideoCore4InstrInfo.td - VideoCore4 Instruction defs ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the VideoCore4 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "VideoCore4InstrFormats.td"

def bitcast_fpimm_to_i32 : SDNodeXForm<fpimm, [{
  return CurDAG->getTargetConstant(N->getValueAPF().bitcastToAPInt().getZExtValue(), SDLoc(N), MVT::i32);
}]>;

def retflag       : SDNode<"VideoCore4ISD::RET_FLAG", SDTNone,
                           [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def callseq_start : SDNode<"ISD::CALLSEQ_START",
			    SDCallSeqStart<[SDTCisVT<0, i32>]>,
			    [SDNPHasChain, SDNPOutGlue]>;
def callseq_end : SDNode<"ISD::CALLSEQ_END",
		   	  SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>,
			  [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def call : SDNode<"VideoCore4ISD::CALL",
	           SDTypeProfile<0, 1, [SDTCisVT<0, i32>]>,
		   [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic]>;

def VCrsqrt : SDNode<"VideoCore4ISD::RSQRT", SDTFPUnaryOp, []>;
def VCrcp   : SDNode<"VideoCore4ISD::RCP",   SDTFPUnaryOp, []>;


//===----------------------------------------------------------------------===//
// VideoCore4 Operand Definitions.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// VideoCore4 Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

// An address of something in memory.
//def addr : ComplexPattern<iPTR, 1, "SelectAddr", [], []>;

def GlobalWrapper : SDNode<"VideoCore4ISD::GLOBAL", SDTIntUnaryOp>;

def VideoCoreBR_JT : SDNode<"VideoCore4ISD::BR_JT",
                             SDTypeProfile<0, 2, [SDTCisVT<0, i32>, SDTCisVT<1, i32>]>,
                             [SDNPHasChain]>;

// An address of something on the stack.
def stacked : Operand<i32>,
		ComplexPattern<i32, 2, "SelectStacked", [frameindex], []> {
	let PrintMethod = "printStacked";
	let MIOperandInfo = (ops GR32, imm);
}

def addrRegImm :
  ComplexPattern<iPTR, 2, "SelectStacked", [frameindex]>;

def InlineJT : Operand<i32> {
//  let PrintMethod = "printInlineJT";
}

//===----------------------------------------------------------------------===//
// Pattern Fragments
//===----------------------------------------------------------------------===//
def LO16 : SDNodeXForm<imm, [{
  // Transformation function: return low 16 bits.
  return getI16Imm(N->getZExtValue() & 0xFFFFULL);
}]>;

def HI16 : SDNodeXForm<imm, [{
  // Transformation function: return bits 16-31.
  return getI16Imm(N->getZExtValue() >> 16);
}]>;

def i32lo16 : PatLeaf<(i32 imm), [{
  // i32lo16 predicate - true if the 32-bit immediate has only rightmost 16
  // bits set.
  return ((N->getZExtValue() & 0FFFFULL) == N->getZExtValue());
}], LO16>;

def i32hi16 : PatLeaf<(i32 imm), [{
  // i32lo16 predicate - true if the 32-bit immediate has only leftmost 16
  // bits set.
  return ((N->getZExtValue() & 0xFFFF0000ULL) == N->getZExtValue());
}], HI16>;

def immSExt16 : PatLeaf<(i32 imm), [{
  // immSExt16 predicate - true if the immediate fits in a 16-bit sign extended
  // field.
  uint64_t val = N->getZExtValue();
  return ((int64_t)val == (int16_t)val);
}]>;

// true if the immediate fits in a 5-bit unsigned field.
def immU5 : PatLeaf<
			(i32 imm),
			[{
				return isUInt<5>(N->getZExtValue());
			}]
		>;

// true if the immediate fits in a 6-bit signed field.
def immS6 : PatLeaf<
			(i32 imm),
			[{
				return isInt<6>(N->getSExtValue());
			}]
		>;

// true if the immediate fits in a 6-bit unsigned field.
def immU6 : PatLeaf<
			(i32 imm),
			[{
				return isUInt<6>(N->getZExtValue());
			}]
		>;

// true if the immediate fits in a 12-bit unsigned field.
def immU12 : PatLeaf<
			(i32 imm),
			[{
				return isUInt<12>(N->getZExtValue());
			}]
		>;

// true if the immediate fits in a 16-bit unsigned field.
def immU16 : PatLeaf<
			(i32 imm),
			[{
				return isUInt<16>(N->getZExtValue());
			}]
		>;

def brtarget : Operand<OtherVT>;
def calltarget : Operand<i32>;

//===----------------------------------------------------------------------===//
// Instruction list..
//===----------------------------------------------------------------------===//

let hasSideEffects = 0 in
	def NOP : VideoCore4Inst<(outs), (ins),
	                          "nop",
				  []>;

let isReturn = 1, isTerminator = 1, isBarrier = 1, Uses = [LR] in {
  def RET : S32<(outs), (ins variable_ops),
		 "b\t%lr",
		 [(retflag)]>;
}

/* --- Calls ---------------------------------------------------------------- */

// ADJCALLSTACKDOWN/UP implicitly use/def SP because they may be expanded into
// a stack adjustment and the codegen must know that they may modify the stack
// pointer before prolog-epilog rewriting occurs.
// Pessimistically assume ADJCALLSTACKDOWN / ADJCALLSTACKUP will become
// sub / add which can clobber SP.
let Defs = [SP], Uses = [SP] in {
	def ADJCALLSTACKDOWN : VideoCore4Inst<
				(outs), (ins i32imm:$d, i32imm:$e),
				"#ADJCALLSTACKDOWN $d",
				[(callseq_start timm:$d, (i32 timm:$e))]
			>;

	def ADJCALLSTACKUP : VideoCore4Inst<
				(outs), (ins i32imm:$a, i32imm:$b),
				"#ADJCALLSTACKUP $a $b",
				[(callseq_end timm:$a, timm:$b)]
			>;
}

let Defs = [LR], Uses = [LR], isCall = 1 in {
	def CALL : S32<(outs), (ins calltarget:$D),
			"bl\t$D",
			[(call texternalsym:$D)]>;

	def CALL_R : S16<(outs), (ins GR32:$D),
			  "bl\t$D",
			  [(call GR32:$D)]>;
}

////////////////////////////////////////////////////////////////////////////////////
// ALU Instruction Formats
////////////////////////////////////////////////////////////////////////////////////

multiclass ALU1_REG<string op, RegisterClass rc, dag node> {
	let Constraints = "$D = $A" in {
		def _R : S16<(outs rc:$D), (ins rc:$A),
			      op,
			      [(set (i32 rc:$D), node)]>;
	}
}

multiclass ALU1_REG_REG<string op, RegisterClass rc, dag node> {
	def _RR : S16<(outs rc:$D), (ins rc:$A),
		       op,
		       [(set (i32 rc:$D), node)]>;
}

multiclass ALU1_REG_IMM<string op, RegisterClass rc, dag node> {
	def _RI : S16<(outs rc:$D), (ins i32imm:$A),
		       op,
		       [(set (i32 rc:$D), node)]>;
}

multiclass ALU1_F_RI<string op2, string op3, PatFrag node> {
	defm _F : ALU1_REG<op2, FR32, (node FR32:$A)>;
	defm _F : ALU1_REG_REG<op3, FR32, (node FR32:$A)>;

	defm _R : ALU1_REG<op2, GR32, (node GR32:$A)>;
	defm _R : ALU1_REG_REG<op3, GR32, (node GR32:$A)>;
	defm _R : ALU1_REG_IMM<op3, GR32, (node immU5:$A)>;
}

multiclass ALU1_FI_RI<string op2, string op3, PatFrag node> {
	defm _F : ALU1_REG<op2, FR32, (node FR32:$A)>;
	defm _F : ALU1_REG_REG<op3, FR32, (node FR32:$A)>;
	defm _F : ALU1_REG_IMM<op3, FR32, (node immU5:$A)>;

	defm _R : ALU1_REG<op2, GR32, (node GR32:$A)>;
	defm _R : ALU1_REG_REG<op3, GR32, (node GR32:$A)>;
	defm _R : ALU1_REG_IMM<op3, GR32, (node immU5:$A)>;
}

/* --- ALU two-input instruction forms -------------------------------------- */

multiclass ALU2_REG_IMM<string op, RegisterClass rc, dag node> {
	let Constraints = "$D = $A" in {
		def _RI : S16<(outs rc:$D), (ins rc:$A, i32imm:$B),
			       op,
			       [(set (i32 rc:$D), node)]>;
	}
}

multiclass ALU2_REG_IMM_PSEUDO<string op, RegisterClass rc> {
	let Constraints = "$D = $A" in {
		def _RI : S16<(outs rc:$D), (ins rc:$A, i32imm:$B),
			       op,
			       []>;
	}
}

multiclass ALU2_REG_REG<string op, RegisterClass rc, dag node> {
	let Constraints = "$D = $A" in {
		def _RR : S16<(outs rc:$D), (ins rc:$A, rc:$B),
			       op,
			       [(set (i32 rc:$D), node)]>;
	}
}

multiclass ALU2_REG_REG_C<string op, RegisterClass rc, dag node> {
	let isCommutable = 1 in {
		defm _C : ALU2_REG_REG<op, rc, node>;
	}
}

multiclass ALU2_REG_REG_IMM<string op, RegisterClass rc, dag node> {
	def _RRI : S32<(outs rc:$D), (ins rc:$A, i32imm:$B),
			op,
			[(set (i32 rc:$D), node)]>;
}

multiclass ALU2_REG_REG_REG<string op, RegisterClass rc, dag node> {
	def _RRR : S32<(outs rc:$D), (ins rc:$A, rc:$B),
			op,
			[(set (i32 rc:$D), node)]>;
}

multiclass ALU2_REG_REG_REG_C<string op, RegisterClass rc, dag node> {
	let isCommutable = 1 in {
		defm _C : ALU2_REG_REG_REG<op, rc, node>;
	}
}

multiclass ALU2_FI_RI<string op2, string op3, PatFrag node> {
	defm _F : ALU2_REG_IMM<op2#" # short", FR32, (node (i32 FR32:$A), (immU5:$B))>;
	defm _F : ALU2_REG_REG<op2#" # short", FR32, (node (i32 FR32:$A), (i32 FR32:$B))>;
	defm _R : ALU2_REG_REG_IMM<op3#" # medium", GR32, (node (i32 GR32:$A), (immU5:$B))>;
	defm _R : ALU2_REG_REG_REG<op3#" # medium", GR32, (node (i32 GR32:$A), (i32 GR32:$B))>;
	defm _R : ALU2_REG_IMM<op2#" # long", GR32, (node (i32 GR32:$A), (imm:$B))>;
}
	
multiclass ALU2_F_RI<string op2, string op3, PatFrag node> {
	defm _F : ALU2_REG_REG<op2#" # short", FR32, (node (i32 FR32:$A), (i32 FR32:$B))>;
	defm _F : ALU2_REG_REG_IMM<op3#" # medium", GR32, (node (i32 GR32:$A), (immU5:$B))>;
	defm _R : ALU2_REG_REG_REG<op3#" # medium", GR32, (node (i32 GR32:$A), (i32 GR32:$B))>;
	defm _R : ALU2_REG_IMM<op2#" # long", GR32, (node (i32 GR32:$A), (imm:$B))>;
}

/* --- ALU instructions ----------------------------------------------------- */

defm ADD : ALU2_FI_RI<
		"add\t$D, $B",
		"add\t$D, $A, $B",
		PatFrag<
			(ops node:$inA, node:$inB),
			(add node:$inA, node:$inB)
		>
	>;
defm ADD_3_LONG : ALU2_REG_REG_IMM<
		"add\t$D, $A, $B # long",
		GR32,
		(add (i32 GR32:$A), imm:$B)
	>;

defm MUL : ALU2_FI_RI<
		"mul\t$D, $B",
		"mul\t$D, $A, $B",
		PatFrag<
			(ops node:$inA, node:$inB),
			(mul node:$inA, node:$inB)
		>
	>;

defm XOR : ALU2_F_RI<
		"eor\t$D, $B",
		"eor\t$D, $A, $B",
		PatFrag<
			(ops node:$inA, node:$inB),
			(xor node:$inA, node:$inB)
		>
	>;

defm SUB : ALU2_FI_RI<
		"sub\t$D, $B",
		"sub\t$D, $A, $B",
		PatFrag<
			(ops node:$inA, node:$inB),
			(sub node:$inA, node:$inB)
		>
	>;

defm AND : ALU2_F_RI<
		"and\t$D, $B",
		"and\t$D, $A, $B",
		PatFrag<
			(ops node:$inA, node:$inB),
			(and node:$inA, node:$inB)
		>
	>;

defm NOT : ALU1_F_RI<
		"not\t$D, $D",
		"not\t$D, $A",
		PatFrag<
			(ops node:$in),
			(xor node:$in, -1)
		>
	>;

defm RSUB : ALU2_FI_RI<
		"rsub\t$D, $B",
		"rsub\t$D, $A, $B",
		PatFrag<
			(ops node:$inA, node:$inB),
			(sub node:$inB, node:$inA)
		>
	>;

defm OR : ALU2_F_RI<
		"or\t$D, $B",
		"or\t$D, $A, $B",
		PatFrag<
			(ops node:$inA, node:$inB),
			(or node:$inA, node:$inB)
		>
	>;

defm SHR : ALU2_FI_RI<
		"lsr\t$D, $B",
		"lsr\t$D, $A, $B",
		PatFrag<
			(ops node:$inA, node:$inB),
			(srl node:$inA, node:$inB)
		>
	>;

defm SHL : ALU2_FI_RI<
		"lsl\t$D, $B",
		"lsl\t$D, $A, $B",
		PatFrag<
			(ops node:$inA, node:$inB),
			(shl node:$inA, node:$inB)
		>
	>;

defm ASR : ALU2_FI_RI<
		"asr\t$D, $B",
		"asr\t$D, $A, $B",
		PatFrag<
			(ops node:$inA, node:$inB),
			(sra node:$inA, node:$inB)
		>
	>;

defm SEXT8 : ALU1_FI_RI<
		"exts\t$D, 8",
		"exts\t$D, $A, 8",
		PatFrag<
			(ops node:$in),
			(sext_inreg node:$in, i8)
		>
	>;

defm SEXT16 : ALU1_FI_RI<
		"exts\t$D, 16",
		"exts\t$D, $A, 16",
		PatFrag<
			(ops node:$in),
			(sext_inreg node:$in, i16)
		>
	>;

defm NEG : ALU1_F_RI<
		"neg\t$D, $D",
		"neg\t$D, $A",
		PatFrag<
			(ops node:$in),
			(sub 0, node:$in)
		>
	>;

multiclass ADDSCALE<int n> {
	defm _ : ALU2_F_RI<
		"addscale\t$D, $B scale "#n,
		"addscale\t$D, $A, $B scale "#n,
		PatFrag<
			(ops node:$inA, node:$inB),
			(add node:$inA,
						(shl node:$inB, n))
		>
	>;
}
defm ADDs1 : ADDSCALE<1>;
defm ADDs2 : ADDSCALE<2>;
defm ADDs3 : ADDSCALE<3>; // actually an ALU2_FI_R instruction
defm ADDs4 : ADDSCALE<4>;

defm MULHD_UU_R : ALU2_REG_REG_REG<"mulhdu\t$D, $A, $B",
	                            GR32,
	                            (mulhu (i32 GR32:$A), (i32 GR32:$B))>;

defm MULHD_UU_I : ALU2_REG_REG_IMM<"mulhdu\t$D, $A, $B",
	                            GR32,
	                            (mulhu (i32 GR32:$A), immS6:$B)>;

defm MULHD_SS_R : ALU2_REG_REG_REG<"mulhds\t$D, $A, $B",
	                            GR32,
	                            (mulhs (i32 GR32:$A), (i32 GR32:$B))>;

defm MULHD_SS_I : ALU2_REG_REG_IMM<"mulhds\t$D, $A, $B",
	                            GR32,
	                            (mulhs (i32 GR32:$A), immS6:$B)>;

defm DIV_R : ALU2_REG_REG_REG<"divs\t$D, $A, $B",
	                       GR32,
	                       (sdiv (i32 GR32:$A), (i32 GR32:$B))>;

defm MAX_RR : ALU2_REG_REG_REG<"max\t$D, $A, $B",
	                        GR32,
	                        (smax (i32 GR32:$A), (i32 GR32:$B))>;
defm MIN_RR : ALU2_REG_REG_REG<"min\t$D, $A, $B",
	                        GR32,
	                        (smin (i32 GR32:$A), (i32 GR32:$B))>;

def CTLZ_P : VideoCore4Pseudo<(outs GR32:$rD), (ins GR32:$rA),
    	     		       "clz\t$rD, $rA",
			       [(set (i32 GR32:$rD), (ctlz (i32 GR32:$rA)))]>;

/*
defm DIV_I : ALU2_REG_REG_IMM<
	"divs\t$D, $A, $B",
	GR32,
	(sdiv (i32 GR32:$A), immS6:$B)
>;
*/

defm DIVU_R : ALU2_REG_REG_REG<"divu\t$D, $A, $B",
	                        GR32,
	                        (udiv (i32 GR32:$A), (i32 GR32:$B))>;

defm DIVU_I : ALU2_REG_REG_IMM<"divu\t$D, $A, $B",
	                        GR32,
	                        (udiv (i32 GR32:$A), immS6:$B)>;

////////////////////////////////////////////////////////////////////////////////////
// Memory Operations
////////////////////////////////////////////////////////////////////////////////////

/* Small fastreg loads/stores. */

multiclass MEMST_F<string op, ValueType vt, PatFrag stop> {
	def _F : S16<
			(outs), (ins FR32:$S, FR32:$D),
			"st"#op#"\t$S, ($D)",
			[(stop (vt FR32:$S), (iPTR FR32:$D))]
		>;
}

multiclass MEMLD_F<string op, ValueType vt, PatFrag ldop> {
	def _F : S16<
			(outs FR32:$D), (ins FR32:$S),
			"ld"#op#"\t$D, ($S)",
			[(set (vt FR32:$D), (ldop (iPTR FR32:$S)))]
		>;
}

multiclass MEM_F<string op, ValueType vt, PatFrag stop, PatFrag ldop> {
	defm _ST : MEMST_F<op, vt, stop>;
	defm _LD : MEMLD_F<op, vt, ldop>;
}

defm MEM8 : MEM_F<"b", i32, truncstorei8, zextloadi8>;
defm MEM8E_LD : MEMLD_F<"b", i32, extloadi8>;

defm MEM16 : MEM_F<"h", i32, truncstorei16, zextloadi16>;
defm MEM16S_LD : MEMLD_F<"cc", i32, sextloadi16>;
defm MEM16E_LD : MEMLD_F<"h", i32, extloadi16>;

defm MEM32 : MEM_F<"", i32, store, load>;
defm MEM32F : MEM_F<"", f32, store, load>;

/* Indexed loads/stores. */

multiclass MEMST_R_I<string op, ValueType vt, PatFrag stop, PatFrag scale> {
	def _R : S32<
			(outs), (ins GR32:$S, GR32:$A, GR32:$B),
			"st"#op#"\t$S, ($A, $B)",
			[(stop (vt GR32:$S),
				(iPTR (scale (i32 GR32:$A), (i32 GR32:$B))))]
		>;

	def _I : S32<
			(outs), (ins GR32:$S, GR32:$A, i32imm:$B),
			"st"#op#"\t$S, $B ($A) # s12-bit displacement",
			[(stop (vt GR32:$S),
				(iPTR (add (i32 GR32:$A), immU12:$B)))]
		>;

	def _LI : S32<
			(outs), (ins GR32:$S, IR32:$A, i32imm:$B),
			"st"#op#"\t$S, $B ($A) # s16-bit displacement",
			[(stop (vt GR32:$S),
				(iPTR (add (i32 IR32:$A), immU16:$B)))]
		>;
}

multiclass MEMLD_R_I<string op, ValueType vt, PatFrag ldop, PatFrag scale> {
	def _R : S32<
			(outs GR32:$D), (ins GR32:$A, GR32:$B),
			"ld"#op#"\t$D, ($A, $B)",
			[(set (vt GR32:$D),
				(ldop (iPTR (scale (i32 GR32:$A), (i32 GR32:$B)))))]
		>;

	def _I : S32<
			(outs GR32:$D), (ins GR32:$A, i32imm:$B),
			"ld"#op#"\t$D, $B ($A) # s12-bit displacement",
			[(set (vt GR32:$D),
				(ldop (iPTR (add (i32 GR32:$A), immU12:$B))))]
		>;

	def _LI : S32<
			(outs GR32:$D), (ins IR32:$A, i32imm:$B),
			"ld"#op#"\t$D, $B ($A) # s16-bit displacement",
			[(set (vt GR32:$D),
				(ldop (iPTR (add (i32 IR32:$A), immU16:$B))))]
		>;
}

multiclass MEM_R_I<string op, ValueType vt, PatFrag stop, PatFrag ldop,
			PatFrag scale> {
	defm _LD : MEMLD_R_I<op, vt, ldop, scale>;
	defm _ST : MEMST_R_I<op, vt, stop, scale>;
}

def memb_frag : PatFrag<
				(ops node:$inA, node:$inB),
				(add node:$inA, node:$inB)
			>;
defm MEM8 : MEM_R_I<"b", i32, truncstorei8, zextloadi8, memb_frag>;
defm MEM8E_LD : MEMLD_R_I<"b", i32, extloadi8, memb_frag>;

def memh_frag : PatFrag<
				(ops node:$inA, node:$inB),
				(add node:$inA, (shl node:$inB, 1))
			>;
defm MEM16 : MEM_R_I<"h", i32, truncstorei16, zextloadi16, memh_frag>;
defm MEM16S_LD : MEMLD_R_I<"cc", i32, sextloadi16, memh_frag>;
defm MEM16E_LD : MEMLD_R_I<"cc", i32, extloadi16, memh_frag>;

defm MEM32 : MEM_R_I<"", i32, store, load,
			PatFrag<
				(ops node:$inA, node:$inB),
				(add node:$inA, (shl node:$inB, 2))
			>
		>;

/* Stack frame loads/stores. */

multiclass MEMFRAME<string op, ValueType vt> {
	def _LD : S32<
			(outs GR32:$D), (ins stacked:$A),
			"ld"#op#"\t$D, $A",
			[(set (vt GR32:$D), (load (iPTR stacked:$A)))]
		>;

	def _ST : S32<
			(outs), (ins GR32:$A, stacked:$B),
			"st"#op#"\t$A, $B",
			[(store (vt GR32:$A), (iPTR stacked:$B))]
		>;
}

def PUSH : S32<
			(outs), (ins PUSHPOP_START_REGS:$StartReg, GR32:$EndReg),
			"push\t$StartReg-$EndReg",
			[]
		>;

def PUSH_LR : S32<
			(outs), (ins PUSHPOP_START_REGS:$StartReg, GR32:$EndReg),
			"push\t$StartReg-$EndReg, %lr",
			[]
		>;

def POP : S32<
			(outs), (ins PUSHPOP_START_REGS:$StartReg, GR32:$EndReg),
			"pop\t$StartReg-$EndReg",
			[]
		>;

def POP_PC : S32<
			(outs), (ins PUSHPOP_START_REGS:$StartReg, GR32:$EndReg),
			"push\t$StartReg-$EndReg, %pc",
			[]
		>;

defm MEMFRAME : MEMFRAME<"", i32>;
defm MEMFRAMEF : MEMFRAME<"", f32>;

/* --- Moves and specials --------------------------------------------------- */

let isAsCheapAsAMove = 1 in
{
	class MOV<RegisterClass rc, dag node> :
		S16<(outs rc:$D), (ins rc:$S),
		     "mov\t$D, $S",
		     [(set rc:$D, node)]>;

	def MOV_FI : MOV<FR32, (i32 immU5:$S)>;
	def MOV_RI : MOV<GR32, (i32 immU5:$S)>;

	def MOV_F : S16<(outs FR32:$D), (ins FR32:$A),
			 "mov\t$D, $A # fast",
			 []>;

	def MOV_R : S32<(outs GR32:$D), (ins GR32:$A),
			 "mov\t$D, $A # medium",
			 []>;

	def MOV_LI : S48<(outs GR32:$D), (ins i32imm:$A),
			 "mov\t$D, $A # long",
			 [(set GR32:$D, imm:$A)]>;

	def MOVF_LI : S48<(outs GR32:$D), (ins f32imm:$A),
			   "mov\t$D, $A # float long",
			   [(set GR32:$D, fpimm:$A)]>;
}

def LEA_PC_LI : S48<(outs GR32:$D), (ins i32imm:$A),
		     "lea\t$D, $A(%pc) # PCrel load",
		     [(set GR32:$D, imm:$A)]>
{
	bits<5> D;
	bits<32> A;
	let Inst{47-37} = 0b11100101000;
	let Inst{36-32} = D;
	let Inst{31-0} = A;

	let Uses = [PC];
}

def : Pat<
		(GlobalWrapper tglobaladdr:$addr),
		(LEA_PC_LI tglobaladdr:$addr)
	>;

let isBranch=1, isIndirectBranch=1, isTerminator=1, isBarrier = 1 in {
    def BR_JT : S48<(outs), (ins InlineJT:$t, GR32:$i),
                     "# BR_JT Pseudo Instruction $i\n$t",
                     [(VideoCoreBR_JT tjumptable:$t, GR32:$i)]>;
}

// --- Branch instructions -------------------------------------------------- 

let Defs = [SR] in {
	def CMP_F : S16<(outs), (ins FR32:$A, FR32:$B),
			 "cmp\t$A, $B # fast",
			 []>;
	def CMP_G : S16<(outs), (ins GR32:$A, GR32:$B),
			 "cmp\t$A, $B",
			 []>;
	def CMP_LI : S48<(outs), (ins GR32:$A, i32imm:$B),
			  "cmp\t$A, $B # long imm",
			  []>;
}

let isTerminator = 1, isBranch = 1 in {
	let isBarrier = 1, hasDelaySlot = 1 in {
		def JMP : S32<(outs), (ins brtarget:$dst),
			       "b\t$dst",
			       [(br bb:$dst)]>;
		
		def JMP_R : S16<(outs), (ins GR32:$dst),
				 "b\t$dst",
				 [(brind GR32:$dst)]>;
	}
	
	/* Generates a 3op compare-and-branch instruction. */
	let Uses = [SR], hasDelaySlot = 1 in {
		class JMP_CC<string cond> : S32<
			(outs), (ins brtarget:$dst),
			"b"#cond#"\t$dst",
			[]
		>;

		def JMP_CC_EQ : JMP_CC<"eq">;
		def JMP_CC_NE : JMP_CC<"ne">;
		def JMP_CC_GT : JMP_CC<"gt">;
		def JMP_CC_GE : JMP_CC<"ge">;
		def JMP_CC_LT : JMP_CC<"lt">;
		def JMP_CC_LE : JMP_CC<"le">;
		def JMP_CC_HI : JMP_CC<"hi">;
		def JMP_CC_HS : JMP_CC<"cc">;
		def JMP_CC_LO : JMP_CC<"cs">;
		def JMP_CC_LS : JMP_CC<"ls">;

		def JMP_TRUE_P : VideoCore4Pseudo<(outs), (ins brtarget:$dst),
		    	       	 	           "brt\t$dst",
						   []>;
	}

  	let Defs = [SR], isCodeGenOnly = 1 in {
	    class JMP_COMP_P<string cond> : VideoCore4Pseudo<(outs), (ins FR32:$ra, FR32:$rb, brtarget:$dst),
	  	 	                                      "",
			                                      []>;
	    class JMP_COMP_F_P<string cond> : VideoCore4Pseudo<(outs), (ins FR32:$ra, FR32:$rb, brtarget:$dst),
	  	 	                                        "",
			                                        []>;
            // True
            def JMP_COMP_EQ_P : JMP_COMP_P<"eq">;
	    def JMP_COMP_NE_P : JMP_COMP_P<"ne">;
	    def JMP_COMP_GT_P : JMP_COMP_P<"gt">;
	    def JMP_COMP_GE_P : JMP_COMP_P<"ge">;
	    def JMP_COMP_LT_P : JMP_COMP_P<"lt">;
	    def JMP_COMP_LE_P : JMP_COMP_P<"le">;
	    def JMP_COMP_HI_P : JMP_COMP_P<"ugt">;
	    def JMP_COMP_HS_P : JMP_COMP_P<"uge">;
	    def JMP_COMP_LO_P : JMP_COMP_P<"ult">;
	    def JMP_COMP_LS_P : JMP_COMP_P<"ule">;


	    // False
            def JMP_COMP_EQ_F_P : JMP_COMP_F_P<"eq">;
	    def JMP_COMP_NE_F_P : JMP_COMP_F_P<"ne">;
	    def JMP_COMP_GT_F_P : JMP_COMP_F_P<"gt">;
	    def JMP_COMP_GE_F_P : JMP_COMP_F_P<"ge">;
	    def JMP_COMP_LT_F_P : JMP_COMP_F_P<"lt">;
	    def JMP_COMP_LE_F_P : JMP_COMP_F_P<"le">;
	    def JMP_COMP_HI_F_P : JMP_COMP_F_P<"ugt">;
	    def JMP_COMP_HS_F_P : JMP_COMP_F_P<"uge">;
	    def JMP_COMP_LO_F_P : JMP_COMP_F_P<"ult">;
	    def JMP_COMP_LS_F_P : JMP_COMP_F_P<"ule">;
        }

	/* Likewise, for floats. */
	let Defs = [SR], isCodeGenOnly = 1 in {
	    class JMP_FCOMP_P<string cond> : VideoCore4Pseudo<(outs), (ins FR32:$ra, FR32:$rb, brtarget:$dst),
	  	                                               "",
		                                               []>;
	    class JMP_FCOMP_F_P<string cond> : VideoCore4Pseudo<(outs), (ins FR32:$ra, FR32:$rb, brtarget:$dst),
	  	                                                 "",
		                                                 []>;
            // True
            def JMP_FCOMP_OEQ_P : JMP_FCOMP_P<"oeq">;
	    def JMP_FCOMP_ONE_P : JMP_FCOMP_P<"one">;
	    def JMP_FCOMP_OGT_P : JMP_FCOMP_P<"ogt">;
	    def JMP_FCOMP_OGE_P : JMP_FCOMP_P<"oge">;
	    def JMP_FCOMP_OLT_P : JMP_FCOMP_P<"olt">;
	    def JMP_FCOMP_OLE_P : JMP_FCOMP_P<"ole">;
            def JMP_FCOMP_UEQ_P : JMP_FCOMP_P<"ueq">;
	    def JMP_FCOMP_UNE_P : JMP_FCOMP_P<"une">;
	    def JMP_FCOMP_UGT_P : JMP_FCOMP_P<"ugt">;
	    def JMP_FCOMP_UGE_P : JMP_FCOMP_P<"uge">;
	    def JMP_FCOMP_ULT_P : JMP_FCOMP_P<"ult">;
	    def JMP_FCOMP_ULE_P : JMP_FCOMP_P<"ule">;

            // False
            def JMP_FCOMP_OEQ_F_P : JMP_FCOMP_F_P<"oeq">;
	    def JMP_FCOMP_ONE_F_P : JMP_FCOMP_F_P<"one">;
	    def JMP_FCOMP_OGT_F_P : JMP_FCOMP_F_P<"ogt">;
	    def JMP_FCOMP_OGE_F_P : JMP_FCOMP_F_P<"oge">;
	    def JMP_FCOMP_OLT_F_P : JMP_FCOMP_F_P<"olt">;
	    def JMP_FCOMP_OLE_F_P : JMP_FCOMP_F_P<"ole">;
            def JMP_FCOMP_UEQ_F_P : JMP_FCOMP_F_P<"ueq">;
	    def JMP_FCOMP_UNE_F_P : JMP_FCOMP_F_P<"une">;
	    def JMP_FCOMP_UGT_F_P : JMP_FCOMP_F_P<"ugt">;
	    def JMP_FCOMP_UGE_F_P : JMP_FCOMP_F_P<"uge">;
	    def JMP_FCOMP_ULT_F_P : JMP_FCOMP_F_P<"ult">;
	    def JMP_FCOMP_ULE_F_P : JMP_FCOMP_F_P<"ule">;
        }
}

let isCodeGenOnly = 1, Uses = [SR] in {
    def FCMP_P : VideoCore4Pseudo<(outs), (ins FR32:$ra, FR32:$rb),
			           "",
			           []>;

    let Constraints = "$d = $f" in {
	class SELECT_P<string cond> : VideoCore4Pseudo<(outs FR32:$d), (ins FR32:$t, FR32:$f, FR32:$lhs, FR32:$rhs),
	  	 	                                "",
			                                []>;
	class FSELECT_P<string cond> : VideoCore4Pseudo<(outs FR32:$d), (ins FR32:$t, FR32:$f, FR32:$lhs, FR32:$rhs),
	  	   	                                 "",
			                                 []>;
	class F_SELECT_P<string cond> : VideoCore4Pseudo<(outs GR32:$d), (ins GR32:$t, GR32:$f, GR32:$lhs, GR32:$rhs),
	  	 	                                  "",
			                                  []>;
	class F_FSELECT_P<string cond> : VideoCore4Pseudo<(outs GR32:$d), (ins GR32:$t, GR32:$f, GR32:$lhs, GR32:$rhs),
	  	 	                                   "",
			                                   []>;
        // int select
	def SELECT_EQ_P : SELECT_P<"eq">;
	def SELECT_NE_P : SELECT_P<"ne">;
	def SELECT_GT_P : SELECT_P<"gt">;
	def SELECT_GE_P : SELECT_P<"ge">;
	def SELECT_LT_P : SELECT_P<"lt">;
	def SELECT_LE_P : SELECT_P<"le">;
	def SELECT_HI_P : SELECT_P<"ugt">;
	def SELECT_HS_P : SELECT_P<"uge">;
	def SELECT_LO_P : SELECT_P<"ult">;
	def SELECT_LS_P : SELECT_P<"ule">;

	def FSELECT_EQ_P : FSELECT_P<"eq">;
	def FSELECT_NE_P : FSELECT_P<"ne">;
	def FSELECT_GT_P : FSELECT_P<"gt">;
	def FSELECT_GE_P : FSELECT_P<"ge">;
	def FSELECT_LT_P : FSELECT_P<"lt">;
	def FSELECT_LE_P : FSELECT_P<"le">;
	def FSELECT_HI_P : FSELECT_P<"ugt">;
	def FSELECT_HS_P : FSELECT_P<"uge">;
	def FSELECT_LO_P : FSELECT_P<"ult">;
	def FSELECT_LS_P : FSELECT_P<"ule">;

        // float select
	def F_SELECT_OEQ_P : F_SELECT_P<"oeq">;
	def F_SELECT_ONE_P : F_SELECT_P<"one">;
	def F_SELECT_OGT_P : F_SELECT_P<"ogt">;
	def F_SELECT_OGE_P : F_SELECT_P<"oge">;
	def F_SELECT_OLT_P : F_SELECT_P<"oge">;
	def F_SELECT_OLE_P : F_SELECT_P<"ole">;
	def F_SELECT_UEQ_P : F_SELECT_P<"ueq">;
	def F_SELECT_UNE_P : F_SELECT_P<"une">;
	def F_SELECT_UGT_P : F_SELECT_P<"ugt">;
	def F_SELECT_UGE_P : F_SELECT_P<"uge">;
	def F_SELECT_ULT_P : F_SELECT_P<"ult">;
	def F_SELECT_ULE_P : F_SELECT_P<"ule">;

	def F_FSELECT_OEQ_P : F_FSELECT_P<"oeq">;
	def F_FSELECT_ONE_P : F_FSELECT_P<"one">;
	def F_FSELECT_OGT_P : F_FSELECT_P<"ogt">;
	def F_FSELECT_OGE_P : F_FSELECT_P<"oge">;
	def F_FSELECT_OLT_P : F_FSELECT_P<"oge">;
	def F_FSELECT_OLE_P : F_FSELECT_P<"ole">;
	def F_FSELECT_UEQ_P : F_FSELECT_P<"ueq">;
	def F_FSELECT_UNE_P : F_FSELECT_P<"une">;
	def F_FSELECT_UGT_P : F_FSELECT_P<"ugt">;
	def F_FSELECT_UGE_P : F_FSELECT_P<"uge">;
	def F_FSELECT_ULT_P : F_FSELECT_P<"uge">;
	def F_FSELECT_ULE_P : F_FSELECT_P<"ule">;
    }

    class CMOV_RR_P<string cond> : VideoCore4Pseudo<(outs FR32:$d), (ins FR32:$t),
    	 	                                     "mov"#cond#"\t$d, $t",
		 	                             []>;
    class CMOV_RI_P<string cond> : VideoCore4Pseudo<(outs FR32:$d), (ins i32imm:$t),
    	 	                                     "mov"#cond#"\t$d, $t",
		 	                             []>;
    def CMOV_EQ_RR_P : CMOV_RR_P<"eq">;
    def CMOV_NE_RR_P : CMOV_RR_P<"ne">;
    def CMOV_GT_RR_P : CMOV_RR_P<"gt">;
    def CMOV_GE_RR_P : CMOV_RR_P<"ge">;
    def CMOV_LT_RR_P : CMOV_RR_P<"lt">;
    def CMOV_LE_RR_P : CMOV_RR_P<"le">;
    def CMOV_HI_RR_P : CMOV_RR_P<"hi">;
    def CMOV_HS_RR_P : CMOV_RR_P<"cc">;
    def CMOV_LO_RR_P : CMOV_RR_P<"cs">;
    def CMOV_LS_RR_P : CMOV_RR_P<"ls">;

    def CMOV_EQ_RI_P : CMOV_RI_P<"eq">;
    def CMOV_NE_RI_P : CMOV_RI_P<"ne">;
    def CMOV_GT_RI_P : CMOV_RI_P<"gt">;
    def CMOV_GE_RI_P : CMOV_RI_P<"ge">;
    def CMOV_LT_RI_P : CMOV_RI_P<"lt">;
    def CMOV_LE_RI_P : CMOV_RI_P<"le">;
    def CMOV_HI_RI_P : CMOV_RI_P<"hi">;
    def CMOV_HS_RI_P : CMOV_RI_P<"cc">;
    def CMOV_LO_RI_P : CMOV_RI_P<"cs">;
    def CMOV_LS_RI_P : CMOV_RI_P<"ls">;
}

class SETCC_RR_P<string cond> : VideoCore4Pseudo<(outs FR32:$d), (ins FR32:$a, FR32:$b),
    	 	                                  "",
		 	                          []>;
class SETCC_RI_P<string cond> : VideoCore4Pseudo<(outs FR32:$d), (ins FR32:$a, i32imm:$b),
    	 	                                  "",
		 	                          []>;
class F_SETCC_RR_P<string cond> : VideoCore4Pseudo<(outs GR32:$d), (ins GR32:$a, GR32:$b),
    	 	                                    "",
		 	                            []>;
def SETCC_EQ_RR_P : SETCC_RR_P<"eq">;
def SETCC_NE_RR_P : SETCC_RR_P<"ne">;
def SETCC_GT_RR_P : SETCC_RR_P<"gt">;
def SETCC_GE_RR_P : SETCC_RR_P<"ge">;
def SETCC_LT_RR_P : SETCC_RR_P<"lt">;
def SETCC_LE_RR_P : SETCC_RR_P<"le">;
def SETCC_HI_RR_P : SETCC_RR_P<"ugt">;
def SETCC_HS_RR_P : SETCC_RR_P<"uge">;
def SETCC_LO_RR_P : SETCC_RR_P<"ult">;
def SETCC_LS_RR_P : SETCC_RR_P<"ule">;

def SETCC_EQ_RI_P : SETCC_RI_P<"eq">;
def SETCC_NE_RI_P : SETCC_RI_P<"ne">;
def SETCC_GT_RI_P : SETCC_RI_P<"gt">;
def SETCC_GE_RI_P : SETCC_RI_P<"ge">;
def SETCC_LT_RI_P : SETCC_RI_P<"lt">;
def SETCC_LE_RI_P : SETCC_RI_P<"le">;
def SETCC_HI_RI_P : SETCC_RI_P<"ugt">;
def SETCC_HS_RI_P : SETCC_RI_P<"uge">;
def SETCC_LO_RI_P : SETCC_RI_P<"ult">;
def SETCC_LS_RI_P : SETCC_RI_P<"ule">;


def F_SETCC_OEQ_RR_P : F_SETCC_RR_P<"oeq">;
def F_SETCC_ONE_RR_P : F_SETCC_RR_P<"one">;
def F_SETCC_OGT_RR_P : F_SETCC_RR_P<"ogt">;
def F_SETCC_OGE_RR_P : F_SETCC_RR_P<"oge">;
def F_SETCC_OLT_RR_P : F_SETCC_RR_P<"olt">;
def F_SETCC_OLE_RR_P : F_SETCC_RR_P<"ole">;
def F_SETCC_UEQ_RR_P : F_SETCC_RR_P<"ueq">;
def F_SETCC_UNE_RR_P : F_SETCC_RR_P<"une">;
def F_SETCC_UGT_RR_P : F_SETCC_RR_P<"ugt">;
def F_SETCC_UGE_RR_P : F_SETCC_RR_P<"uge">;
def F_SETCC_ULT_RR_P : F_SETCC_RR_P<"ult">;
def F_SETCC_ULE_RR_P : F_SETCC_RR_P<"ule">;

// int setcc ri patterns
def : Pat<(seteq (i32 FR32:$A), (i32 immU5:$B)),
          (SETCC_EQ_RI_P (i32 FR32:$A), (immU5:$B))>;
def : Pat<(setne (i32 FR32:$A), (i32 immU5:$B)),
          (SETCC_NE_RI_P (i32 FR32:$A), (immU5:$B))>;
def : Pat<(setgt (i32 FR32:$A), (i32 immU5:$B)),
          (SETCC_GT_RI_P (i32 FR32:$A), (immU5:$B))>;
def : Pat<(setge (i32 FR32:$A), (i32 immU5:$B)),
          (SETCC_GE_RI_P (i32 FR32:$A), (immU5:$B))>;
def : Pat<(setlt (i32 FR32:$A), (i32 immU5:$B)),
          (SETCC_LT_RI_P (i32 FR32:$A), (immU5:$B))>;
def : Pat<(setle (i32 FR32:$A), (i32 immU5:$B)),
          (SETCC_LE_RI_P (i32 FR32:$A), (immU5:$B))>;
def : Pat<(setugt (i32 FR32:$A), (i32 immU5:$B)),
          (SETCC_HI_RI_P (i32 FR32:$A), (immU5:$B))>;
def : Pat<(setuge (i32 FR32:$A), (i32 immU5:$B)),
          (SETCC_HS_RI_P (i32 FR32:$A), (immU5:$B))>;
def : Pat<(setult (i32 FR32:$A), (i32 immU5:$B)),
          (SETCC_LO_RI_P (i32 FR32:$A), (immU5:$B))>;
def : Pat<(setule (i32 FR32:$A), (i32 immU5:$B)),
          (SETCC_LS_RI_P (i32 FR32:$A), (immU5:$B))>;

// int setcc rr patterns
def : Pat<(seteq (i32 FR32:$A), (i32 FR32:$B)),
          (SETCC_EQ_RR_P (i32 FR32:$A), (i32 FR32:$B))>;
def : Pat<(setne (i32 FR32:$A), (i32 FR32:$B)),
          (SETCC_NE_RR_P (i32 FR32:$A), (i32 FR32:$B))>;
def : Pat<(setgt (i32 FR32:$A), (i32 FR32:$B)),
          (SETCC_GT_RR_P (i32 FR32:$A), (i32 FR32:$B))>;
def : Pat<(setge (i32 FR32:$A), (i32 FR32:$B)),
          (SETCC_GE_RR_P (i32 FR32:$A), (i32 FR32:$B))>;
def : Pat<(setlt (i32 FR32:$A), (i32 FR32:$B)),
          (SETCC_LT_RR_P (i32 FR32:$A), (i32 FR32:$B))>;
def : Pat<(setle (i32 FR32:$A), (i32 FR32:$B)),
          (SETCC_LE_RR_P (i32 FR32:$A), (i32 FR32:$B))>;
def : Pat<(setugt (i32 FR32:$A), (i32 FR32:$B)),
          (SETCC_HI_RR_P (i32 FR32:$A), (i32 FR32:$B))>;
def : Pat<(setuge (i32 FR32:$A), (i32 FR32:$B)),
          (SETCC_HS_RR_P (i32 FR32:$A), (i32 FR32:$B))>;
def : Pat<(setult (i32 FR32:$A), (i32 FR32:$B)),
          (SETCC_LO_RR_P (i32 FR32:$A), (i32 FR32:$B))>;
def : Pat<(setule (i32 FR32:$A), (i32 FR32:$B)),
          (SETCC_LS_RR_P (i32 FR32:$A), (i32 FR32:$B))>;

// float setcc rr patterns
def : Pat<(setoeq (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_OEQ_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setone (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_ONE_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setogt (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_OGT_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setoge (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_OGE_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setolt (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_OLT_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setole (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_OLE_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setueq (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_UEQ_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setune (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_UNE_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setugt (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_UGT_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setuge (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_UGE_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setult (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_ULT_RR_P (i32 GR32:$A), (i32 GR32:$B))>;
def : Pat<(setule (i32 GR32:$A), (i32 GR32:$B)),
          (F_SETCC_ULE_RR_P (i32 GR32:$A), (i32 GR32:$B))>;

// int branch patterns
def : Pat<(brcond (i32 (seteq (i32 GR32:$lhs), (i32 GR32:$rhs))), bb:$dst),
          (JMP_COMP_EQ_P (i32 GR32:$lhs), (i32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setne (i32 GR32:$lhs), (i32 GR32:$rhs))), bb:$dst),
          (JMP_COMP_NE_P (i32 GR32:$lhs), (i32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setgt (i32 GR32:$lhs), (i32 GR32:$rhs))), bb:$dst),
          (JMP_COMP_GT_P (i32 GR32:$lhs), (i32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setge (i32 GR32:$lhs), (i32 GR32:$rhs))), bb:$dst),
          (JMP_COMP_GE_P (i32 GR32:$lhs), (i32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setlt (i32 GR32:$lhs), (i32 GR32:$rhs))), bb:$dst),
          (JMP_COMP_LT_P (i32 GR32:$lhs), (i32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setle (i32 GR32:$lhs), (i32 GR32:$rhs))), bb:$dst),
          (JMP_COMP_LE_P (i32 GR32:$lhs), (i32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setugt (i32 GR32:$lhs), (i32 GR32:$rhs))), bb:$dst),
          (JMP_COMP_HI_P (i32 GR32:$lhs), (i32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setuge (i32 GR32:$lhs), (i32 GR32:$rhs))), bb:$dst),
          (JMP_COMP_HS_P (i32 GR32:$lhs), (i32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setult (i32 GR32:$lhs), (i32 GR32:$rhs))), bb:$dst),
          (JMP_COMP_LO_P (i32 GR32:$lhs), (i32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setule (i32 GR32:$lhs), (i32 GR32:$rhs))), bb:$dst),
          (JMP_COMP_LS_P (i32 GR32:$lhs), (i32 GR32:$rhs), bb:$dst)>;

// float branch patterns
def : Pat<(brcond (i32 (setoeq (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_OEQ_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setone (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_ONE_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setogt (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_OGT_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setoge (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_OGE_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setolt (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_OLT_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setole (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_OLE_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setueq (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_UEQ_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setune (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_UNE_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setugt (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_UGT_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setuge (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_UGE_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setult (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_ULT_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;
def : Pat<(brcond (i32 (setule (f32 GR32:$lhs), (f32 GR32:$rhs))), bb:$dst),
          (JMP_FCOMP_ULE_P (f32 GR32:$lhs), (f32 GR32:$rhs), bb:$dst)>;


// int selectcc patterns
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (i32 FR32:$T), (i32 FR32:$F), SETEQ),
          (SELECT_EQ_P (i32 FR32:$T), (i32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (i32 FR32:$T), (i32 FR32:$F), SETNE),
          (SELECT_NE_P (i32 FR32:$T), (i32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (i32 FR32:$T), (i32 FR32:$F), SETGT),
          (SELECT_GT_P (i32 FR32:$T), (i32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (i32 FR32:$T), (i32 FR32:$F), SETGE),
          (SELECT_GE_P (i32 FR32:$T), (i32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (i32 FR32:$T), (i32 FR32:$F), SETLT),
          (SELECT_LT_P (i32 FR32:$T), (i32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (i32 FR32:$T), (i32 FR32:$F), SETLE),
          (SELECT_LE_P (i32 FR32:$T), (i32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (i32 FR32:$T), (i32 FR32:$F), SETUGT),
          (SELECT_HI_P (i32 FR32:$T), (i32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (i32 FR32:$T), (i32 FR32:$F), SETUGE),
          (SELECT_HS_P (i32 FR32:$T), (i32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (i32 FR32:$T), (i32 FR32:$F), SETULT),
          (SELECT_LO_P (i32 FR32:$T), (i32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (i32 FR32:$T), (i32 FR32:$F), SETULE),
          (SELECT_LS_P (i32 FR32:$T), (i32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (f32 FR32:$T), (f32 FR32:$F), SETEQ),
          (FSELECT_EQ_P (f32 FR32:$T), (f32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (f32 FR32:$T), (f32 FR32:$F), SETNE),
          (FSELECT_NE_P (f32 FR32:$T), (f32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (f32 FR32:$T), (f32 FR32:$F), SETGT),
          (FSELECT_GT_P (f32 FR32:$T), (f32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (f32 FR32:$T), (f32 FR32:$F), SETGE),
          (FSELECT_GE_P (f32 FR32:$T), (f32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (f32 FR32:$T), (f32 FR32:$F), SETLT),
          (FSELECT_LT_P (f32 FR32:$T), (f32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (f32 FR32:$T), (f32 FR32:$F), SETLE),
          (FSELECT_LE_P (f32 FR32:$T), (f32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (f32 FR32:$T), (f32 FR32:$F), SETUGT),
          (FSELECT_HI_P (f32 FR32:$T), (f32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (f32 FR32:$T), (f32 FR32:$F), SETUGE),
          (FSELECT_HS_P (f32 FR32:$T), (f32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (f32 FR32:$T), (f32 FR32:$F), SETULT),
          (FSELECT_LO_P (f32 FR32:$T), (f32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;
def : Pat<(selectcc (i32 FR32:$lhs), (i32 FR32:$rhs), (f32 FR32:$T), (f32 FR32:$F), SETULE),
          (FSELECT_LS_P (f32 FR32:$T), (f32 FR32:$F), (i32 FR32:$lhs), (i32 FR32:$rhs))>;


// float selectcc patterns
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETOEQ),
          (F_SELECT_OEQ_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETONE),
          (F_SELECT_ONE_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETOGT),
          (F_SELECT_OGT_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETOGE),
          (F_SELECT_OGE_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETOLT),
          (F_SELECT_OLT_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETOLE),
          (F_SELECT_OLE_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETUEQ),
          (F_SELECT_UEQ_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETUNE),
          (F_SELECT_UNE_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETUGT),
          (F_SELECT_UGT_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETUGE),
          (F_SELECT_UGE_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETULT),
          (F_SELECT_ULT_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (i32 GR32:$T), (i32 GR32:$F), SETULE),
          (F_SELECT_ULE_P (i32 GR32:$T), (i32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETOEQ),
          (F_FSELECT_OEQ_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETONE),
          (F_FSELECT_ONE_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETOGT),
          (F_FSELECT_OGT_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETOGE),
          (F_FSELECT_OGE_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETOLT),
          (F_FSELECT_OLT_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETOLE),
          (F_FSELECT_OLE_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETUEQ),
          (F_FSELECT_UEQ_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETUNE),
          (F_FSELECT_UNE_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETUGT),
          (F_FSELECT_UGT_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETUGE),
          (F_FSELECT_UGE_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETULT),
          (F_FSELECT_ULT_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;
def : Pat<(selectcc (f32 GR32:$lhs), (f32 GR32:$rhs), (f32 GR32:$T), (f32 GR32:$F), SETULE),
          (F_FSELECT_ULE_P (f32 GR32:$T), (f32 GR32:$F), (f32 GR32:$lhs), (f32 GR32:$rhs))>;


/* --- Floating point arithmetic -------------------------------------------- */

let isCommutable = 1 in {
    def FADDrr : S32<(outs GR32:$rD), (ins GR32:$rA, GR32:$rB),
 	  	      "fadd\t{$rD, $rA, $rB}",
		      [(set (f32 GR32:$rD), (fadd (f32 GR32:$rA), (f32 GR32:$rB)))]>;
    def FMAXrr : S32<(outs GR32:$rD), (ins GR32:$rA, GR32:$rB),
		      "fmax\t{$rD, $rA, $rB}",
		      [(set (f32 GR32:$rD), (fmaximum (f32 GR32:$rA), (f32 GR32:$rB)))]>;
    def FMINrr : S32<(outs GR32:$rD), (ins GR32:$rA, GR32:$rB),
		      "fmin\t{$rD, $rA, $rB}",
		      [(set (f32 GR32:$rD), (fminimum (f32 GR32:$rA), (f32 GR32:$rB)))]>;
    def FMAXABSrr : S32<(outs GR32:$rD), (ins GR32:$rA, GR32:$rB),
		         "fmaxabs\t{$rD, $rA, $rB}",
		         [(set (f32 GR32:$rD), (fmaximum (fabs (f32 GR32:$rA)), (fabs (f32 GR32:$rB))))]>;
    def FMINABSrr : S32<(outs GR32:$rD), (ins GR32:$rA, GR32:$rB),
		         "fminabs\t{$rD, $rA, $rB}",
		         [(set (f32 GR32:$rD), (fminimum (fabs (f32 GR32:$rA)), (fabs (f32 GR32:$rB))))]>;
}

let isCommutable = 0 in {
    def FSUBrr : S32<(outs GR32:$rD), (ins GR32:$rA, GR32:$rB),
		      "fsub\t{$rD, $rA, $rB}",
		      [(set (f32 GR32:$rD), (fsub (f32 GR32:$rA), (f32 GR32:$rB)))]>;

    def FMULrr : S32<(outs GR32:$rD), (ins GR32:$rA, GR32:$rB),
   	   	      "fmul\t{$rD, $rA, $rB}",
		      [(set (f32 GR32:$rD), (fmul (f32 GR32:$rA), (f32 GR32:$rB)))]>;

    def FDIVrr : S32<(outs GR32:$rD), (ins GR32:$rA, GR32:$rB),
		      "fdiv\t{$rD, $rA, $rB}",
		      [(set (f32 GR32:$rD), (fdiv (f32 GR32:$rA), (f32 GR32:$rB)))]>;

    def FTOIrr : S32<(outs GR32:$rD), (ins GR32:$rA),
 		      "ftoi\t{$rD, $rA}",
		      [(set (i32 GR32:$rD), (fp_to_sint (f32 GR32:$rA)))]>;
    def ITOFrr : S32<(outs GR32:$rD), (ins GR32:$rA),
 		      "itof\t{$rD, $rA}",
		      [(set (f32 GR32:$rD), (sint_to_fp (i32 GR32:$rA)))]>;
}

// bitcast
let Constraints = "$rD = $rA" in { 
    def FTOICONV_P : VideoCore4Pseudo<(outs GR32:$rD), (ins GR32:$rA),
    	                               "",
	                               [(set (i32 GR32:$rD), (bitconvert (f32 GR32:$rA)))]>;
    def ITOFCONV_P : VideoCore4Pseudo<(outs GR32:$rD), (ins GR32:$rA),
                                       "",
	                               [(set (f32 GR32:$rD), (bitconvert (i32 GR32:$rA)))]>;
}

def FLOG2 : S32<(outs GR32:$rD), (ins GR32:$rA),
	         "log2\t{$rD, $rA}",
	         [(set (f32 GR32:$rD), (flog2 (f32 GR32:$rA)))]>;
		
def FEXP2 : S32<(outs GR32:$rD), (ins GR32:$rA),
	         "exp2\t{$rD, $rA}",
	         [(set (f32 GR32:$rD), (fexp2 (f32 GR32:$rA)))]>;

// recip
def FRSQRT : S32<(outs GR32:$rD), (ins GR32:$rA),
	          "rsqrt\t{$rD, $rA}",
	          [(set (f32 GR32:$rD), (VCrsqrt (f32 GR32:$rA)))]>;
def FRCP : S32<(outs GR32:$rD), (ins GR32:$rA),
	        "rcp\t{$rD, $rA}",
	        [(set (f32 GR32:$rD), (VCrcp (f32 GR32:$rA)))]>;
			 
def : Pat<(fabs (f32 GR32:$rA)),
          (FMAXABSrr (f32 $rA), (FSUBrr (f32 TMP), (f32 TMP)))>;

def : Pat<(fneg (f32 GR32:$rA)),
          (FSUBrr (FSUBrr (f32 TMP), (f32 TMP)), (f32 $rA))>;

