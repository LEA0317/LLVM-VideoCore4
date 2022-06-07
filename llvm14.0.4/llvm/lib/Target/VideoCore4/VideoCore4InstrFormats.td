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

class VC4_OP4_S16<bits<4>   opcode,
                  dag       outs,
                  dag       ins,
                  string    asmstr,
                  list<dag> pattern> :
      S16<opcode,
          outs, ins,
          asmstr,
          pattern> {
  // I don't know the actual encoding so far
  bits<5> rd;
  bits<5> ra;

  let Inst{4-0}   = rd;
  let Inst{9-5}   = ra;
  let Inst{10}    = 0;
  let Inst{14-11} = opcode;
  let Inst{15}    = 0;
}

class VC4_OP5_S16<bits<5>   opcode,
                  dag       outs,
                  dag       ins,
                  string    asmstr,
                  list<dag> pattern> :
      S16<opcode,
          outs, ins,
          asmstr,
          pattern> {
  // I don't know the actual encoding so far
  bits<5> rd;
  bits<5> ra;

  let Inst{4-0}   = rd;
  let Inst{9-5}   = ra;
  let Inst{14-10} = opcode;
  let Inst{15}    = 0;
}

class VC4_OP6_S16<bits<6>   opcode,
                  dag       outs,
                  dag       ins,
                  string    asmstr,
                  list<dag> pattern> :
      S16<opcode,
          outs, ins,
          asmstr,
          pattern> {
  // I don't know the actual encoding so far
  bits<5> rd;
  bits<5> ra;

  let Inst{4-0}   = rd;
  let Inst{9-5}   = ra;
  let Inst{16-10} = opcode;
}
