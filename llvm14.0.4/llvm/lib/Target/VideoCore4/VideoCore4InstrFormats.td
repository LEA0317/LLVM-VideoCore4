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
  bits<3> mul_b;
  bits<3> mul_a;
  bits<3> add_b;
  bits<3> add_a;
  bits<6> raddr_b;
  bits<6> raddr_a;
  bits<5> op_add;
  bits<3> op_mul;
  bits<6> waddr_mul;
  bits<6> waddr_add;
  bits<1> ws;
  bits<1> sf;
  bits<3> cond_mul;
  bits<3> cond_add;
  bits<4> pack;
  bits<1> pm;
  bits<3> unpack;
  bits<4> sig;

  let Inst{2-0}   = mul_b;
  let Inst{5-3}   = mul_a;
  let Inst{8-6}   = add_b;
  let Inst{11-9}  = add_a;
  let Inst{17-12} = raddr_b;
  let Inst{23-18} = raddr_a;
  let Inst{28-24} = op_add;
  let Inst{31-29} = op_mul;
  let Inst{37-32} = waddr_mul;
  let Inst{43-38} = waddr_add;
  let Inst{44}    = ws;
  let Inst{45}    = sf;
  let Inst{48-46} = cond_mul;
  let Inst{51-49} = cond_add;
  let Inst{55-52} = pack;
  let Inst{56}    = pm;
  let Inst{59-57} = unpack;
  let Inst{63-60} = sig;
}

class VC4_ALU_IMM<dag       outs,
                  dag       ins,
                  string    asmstr,
                  list<dag> pattern> :
      S64<outs, ins,
          asmstr,
          pattern>
{
  bits<3> mul_b;
  bits<3> mul_a;
  bits<3> add_b;
  bits<3> add_a;
  bits<6> small_immed;
  bits<6> raddr_a;
  bits<5> op_add;
  bits<3> op_mul;
  bits<6> waddr_mul;
  bits<6> waddr_add;
  bits<1> ws;
  bits<1> sf;
  bits<3> cond_mul;
  bits<3> cond_add;
  bits<4> pack;
  bits<1> pm;
  bits<3> unpack;
  bits<4> sig;

  let Inst{2-0}   = mul_b;
  let Inst{5-3}   = mul_a;
  let Inst{8-6}   = add_b;
  let Inst{11-9}  = add_a;
  let Inst{17-12} = small_immed;
  let Inst{23-18} = raddr_a;
  let Inst{28-24} = op_add;
  let Inst{31-29} = op_mul;
  let Inst{37-32} = waddr_mul;
  let Inst{43-38} = waddr_add;
  let Inst{44}    = ws;
  let Inst{45}    = sf;
  let Inst{48-46} = cond_mul;
  let Inst{51-49} = cond_add;
  let Inst{55-52} = pack;
  let Inst{56}    = pm;
  let Inst{59-57} = unpack;
  let Inst{63-60} = 0b1101;
}

class VC4_BRANCH<dag       outs,
                 dag       ins,
                 string    asmstr,
                 list<dag> pattern> :
      S64<outs, ins,
          asmstr,
          pattern>
{
  bits<32> immediate;
  bits<6>  waddr_mul;
  bits<6>  waddr_add;
  bits<1>  ws;
  bits<5>  raddr_a;
  bits<1>  reg;
  bits<1>  rel;
  bits<4>  cond_br;

  let Inst{31-0}  = immediate;
  let Inst{37-32} = waddr_mul;
  let Inst{43-38} = waddr_add;
  let Inst{44}    = ws;
  let Inst{49-45} = raddr_a;
  let Inst{50}    = reg;
  let Inst{51}    = rel;
  let Inst{55-52} = cond_br;
  let Inst{59-56} = 0; // don't care?
  let Inst{63-60} = 0b1111;
}

class VC4_LI32<dag       outs,
               dag       ins,
               string    asmstr,
               list<dag> pattern> :
      S64<outs, ins,
          asmstr,
          pattern>
{
  bits<32> immediate;
  bits<6>  waddr_mul;
  bits<6>  waddr_add;
  bits<1>  ws;
  bits<1>  sf;
  bits<3>  cond_mul;
  bits<3>  cond_add;
  bits<4>  pack;
  bits<1>  pm;

  let Inst{31-0}  = immediate;
  let Inst{37-32} = waddr_mul;
  let Inst{43-38} = waddr_add;
  let Inst{44}    = ws;
  let Inst{45}    = sf;
  let Inst{48-46} = cond_mul;
  let Inst{51-49} = cond_add;
  let Inst{55-52} = pack;
  let Inst{56}    = pm;
  let Inst{63-57} = 0b1110000;
}

class VC4_LI_PER_ELEM_S<dag       outs,
                        dag       ins,
                        string    asmstr,
                        list<dag> pattern> :
      S64<outs, ins,
          asmstr,
          pattern>
{
  bits<16> per_elem_ls;
  bits<16> per_elem_ms;
  bits<6>  waddr_mul;
  bits<6>  waddr_add;
  bits<1>  ws;
  bits<1>  sf;
  bits<3>  cond_mul;
  bits<3>  cond_add;
  bits<4>  pack;
  bits<1>  pm;

  let Inst{15-0}  = per_elem_ls;
  let Inst{31-16} = per_elem_ms;
  let Inst{37-32} = waddr_mul;
  let Inst{43-38} = waddr_add;
  let Inst{44}    = ws;
  let Inst{45}    = sf;
  let Inst{48-46} = cond_mul;
  let Inst{51-49} = cond_add;
  let Inst{55-52} = pack;
  let Inst{56}    = pm;
  let Inst{63-57} = 0b1110001;
}

class VC4_LI_PER_ELEM_US<dag       outs,
                         dag       ins,
                         string    asmstr,
                         list<dag> pattern> :
      S64<outs, ins,
          asmstr,
          pattern>
{
  bits<16> per_elem_ls;
  bits<16> per_elem_ms;
  bits<6>  waddr_mul;
  bits<6>  waddr_add;
  bits<1>  ws;
  bits<1>  sf;
  bits<3>  cond_mul;
  bits<3>  cond_add;
  bits<4>  pack;
  bits<1>  pm;

  let Inst{15-0}  = per_elem_ls;
  let Inst{31-16} = per_elem_ms;
  let Inst{37-32} = waddr_mul;
  let Inst{43-38} = waddr_add;
  let Inst{44}    = ws;
  let Inst{45}    = sf;
  let Inst{48-46} = cond_mul;
  let Inst{51-49} = cond_add;
  let Inst{55-52} = pack;
  let Inst{56}    = pm;
  let Inst{63-57} = 0b1110011;
}

class VC4_SEMAPHORE<dag       outs,
                    dag       ins,
                    string    asmstr,
                    list<dag> pattern> :
      S64<outs, ins,
          asmstr,
          pattern>
{
  bits<4> semaphore;
  bits<1> sa;
  bits<6> waddr_mul;
  bits<6> waddr_add;
  bits<1> ws;
  bits<1> sf;
  bits<3> cond_mul;
  bits<3> cond_add;
  bits<4> pack;
  bits<1> pm;

  let Inst{3-0}   = semaphore;
  let Inst{4}     = sa;
  let Inst{31-5}  = 0; // don't care
  let Inst{37-32} = waddr_mul;
  let Inst{43-38} = waddr_add;
  let Inst{44}    = ws;
  let Inst{45}    = sf;
  let Inst{48-46} = cond_mul;
  let Inst{51-49} = cond_add;
  let Inst{55-52} = pack;
  let Inst{56}    = pm;
  let Inst{63-57} = 0b1110100;
}
