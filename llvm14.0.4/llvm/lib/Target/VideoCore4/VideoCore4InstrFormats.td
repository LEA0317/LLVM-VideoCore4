//==- VideoCore4InstrFormats.td - VideoCore4 Instruction Formats -*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

class Format<bits<16> val> {
  bits<16> Value = val;
}

def VC4: Format<0>;

// Generic VideoCore4 instruction
class VideoCore4Inst<dag            outs,
                     dag            ins,
   		     string         asmstr,
		     list<dag>      pattern,
     		     InstrItinClass itin=IIPseudo> : Instruction {
  field bits<80> Inst;

  let Namespace        = "VideoCore4";
  let DecoderNamespace = "VideoCore4";

  Format Form = VC4;

  dag OutOperandList = outs;
  dag InOperandList  = ins;

  let AsmString = asmstr;
  let Pattern   = pattern;
  let Itinerary = itin;

  field bits<80> SoftFail = 0;
}

// 64-bit VC4 instruction
class S64<dag outs, dag ins, string asmstr, list<dag> pattern> :
	  VideoCore4Inst<outs, ins, asmstr, pattern> {
  let Size = 8;
}

// 48-bit VC4 instruction
class S48<dag outs, dag ins, string asmstr, list<dag> pattern> :
	  VideoCore4Inst<outs, ins, asmstr, pattern> {
  let Size = 6;
}

// 32-bit VC4 instruction
class S32<dag outs, dag ins, string asmstr, list<dag> pattern> :
	  VideoCore4Inst<outs, ins, asmstr, pattern> {
  let Size = 4;
}

// 16-bit VC4 instruction
class S16<dag outs, dag ins, string asmstr, list<dag> pattern> :
	  VideoCore4Inst<outs, ins, asmstr, pattern> {
  let Size = 2;
}

// Pseudo VC4 instructions
class VideoCore4Pseudo<dag       outs,
                       dag       ins,
                       string    asmstr,
                       list<dag> pattern> :
      VideoCore4Inst<outs, ins,
                     asmstr,
                     pattern> {
  let isCodeGenOnly = 1;
  let isPseudo      = 1;
}

class VC4_ALU<dag       outs,
              dag       ins,
              string    asmstr,
              list<dag> pattern> :
      S64<outs, ins,
          asmstr,
          pattern>
{
  bits<4> sig;
  bits<3> unpack;
  bits<1> pm;
  bits<4> pack;
  bits<3> cond_add;
  bits<3> cond_mul;
  bits<1> sf;
  bits<1> ws;
  bits<6> waddr_add;
  bits<6> waddr_mul;
  bits<3> op_mul;
  bits<5> op_add;
  bits<6> raddr_a;
  bits<6> raddr_b;
  bits<3> add_a;
  bits<3> add_b;
  bits<3> mul_a;
  bits<3> mul_b;

  let Inst{3-0}   = sig;
  let Inst{6-4}   = unpack;
  let Inst{7}     = pm;
  let Inst{11-8}  = pack;
  let Inst{14-12} = cond_add;
  let Inst{17-15} = cond_mul;
  let Inst{18}    = sf;
  let Inst{19}    = ws;
  let Inst{25-20} = waddr_add;
  let Inst{31-26} = waddr_mul;
  let Inst{34-32} = op_mul;
  let Inst{39-35} = op_add;
  let Inst{45-40} = raddr_a;
  let Inst{51-46} = raddr_b;
  let Inst{54-52} = add_a;
  let Inst{57-55} = add_b;
  let Inst{60-58} = mul_a;
  let Inst{63-61} = mul_b;
}

class VC4_ALU_IMM<dag       outs,
                  dag       ins,
                  string    asmstr,
                  list<dag> pattern> :
      S64<outs, ins,
          asmstr,
          pattern>
{
  bits<4> sig;
  bits<3> unpack;
  bits<1> pm;
  bits<4> pack;
  bits<3> cond_add;
  bits<3> cond_mul;
  bits<1> sf;
  bits<1> ws;
  bits<6> waddr_add;
  bits<6> waddr_mul;
  bits<3> op_mul;
  bits<5> op_add;
  bits<6> raddr_a;
  bits<6> small_immed;
  bits<3> add_a;
  bits<3> add_b;
  bits<3> mul_a;
  bits<3> mul_b;

  let Inst{3-0}   = sig;
  let Inst{6-4}   = unpack;
  let Inst{7}     = pm;
  let Inst{11-8}  = pack;
  let Inst{14-12} = cond_add;
  let Inst{17-15} = cond_mul;
  let Inst{18}    = sf;
  let Inst{19}    = ws;
  let Inst{25-20} = waddr_add;
  let Inst{31-26} = waddr_mul;
  let Inst{34-32} = op_mul;
  let Inst{39-35} = op_add;
  let Inst{45-40} = raddr_a;
  let Inst{51-46} = small_immed;
  let Inst{54-52} = add_a;
  let Inst{57-55} = add_b;
  let Inst{60-58} = mul_a;
  let Inst{63-61} = mul_b;
}

