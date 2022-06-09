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
class S48<dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin = IIAlu> :
	  VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  let Size = 6;
}

// 32-bit VC4 instruction
class S32<dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin = IIAlu> :
	  VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  let Size = 4;
}

// 16-bit VC4 instruction
class S16<dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin = IIAlu> :
	  VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
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

class S16_OP4<bits<4>        opcode,
              dag            outs,
	      dag            ins,
              string         asmstr,
              list<dag>      pattern,
	      InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  // I don't know the actual encoding for now
  bits<5> rd;
  bits<5> ra;

  let Inst{4-0}   = rd;
  let Inst{9-5}   = ra;
  let Inst{10}    = 0;
  let Inst{14-11} = opcode;
  let Inst{15}    = 0;
}

class S16_OP5<bits<5>       opcode,
              dag           outs,
	      dag           ins,
              string        asmstr,
              list<dag>     pattern,
	     InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  // I don't know the actual encoding for now
  bits<5> rd;
  bits<5> ra;

  let Inst{4-0}   = rd;
  let Inst{9-5}   = ra;
  let Inst{14-10} = opcode;
  let Inst{15}    = 0;
}

class S16_OP6<bits<6>        opcode,
              dag            outs,
	      dag            ins,
              string         asmstr,
              list<dag>      pattern,
	      InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  // I don't know the actual encoding for now
  bits<5> rd;
  bits<5> ra;

  let Inst{4-0}   = rd;
  let Inst{9-5}   = ra;
  let Inst{15-10} = opcode;
}

class S32_3_OP11<bits<11>       opcode,
                 dag            outs,
	         dag            ins,
                 string         asmstr,
                 list<dag>      pattern,
	         InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  // I don't know the actual encoding for now
  bits<5> rd;
  bits<5> ra;
  bits<5> rb;
  bits<4> c;

  let Inst{4-0}   = rb;
  let Inst{6-5}   = 0;
  let Inst{10-7}  = c;
  let Inst{15-11} = ra;
  let Inst{20-16} = rd;
  let Inst{31-21} = opcode;
}

class S16_R<dag            outs,
	    dag            ins,
            string         asmstr,
            list<dag>      pattern,
	    InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  bits<5> rd;
  let Inst{4-0} = rd;
}

class S16_U<dag            outs,
	    dag            ins,
            string         asmstr,
            list<dag>      pattern,
	    InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  bits<6> imm;
  let Inst{5-0} = imm;
}

class S16_OP5_IMM<bits<5>        opcode,
                  dag            outs,
	          dag            ins,
                  string         asmstr,
                  list<dag>      pattern,
	          InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  bits<5> rd;
  bits<6> imm;

  let Inst{4-0}   = rd;
  let Inst{10-5}  = imm;
  let Inst{15-11} = opcode;
}

class S48_ARITH_IMM<bits<6>        opcode,
                    dag            outs,
	            dag            ins,
                    string         asmstr,
                    list<dag>      pattern,
	            InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  bits<5>  rd;
  bits<5>  rs;
  bits<32> imm;

  let Inst{31-0}  = imm;
  let Inst{36-32} = rd;
  let Inst{41-37} = rs;
  let Inst{47-42} = opcode;
}

class S32_ARITH_IMM_ACC<bits<5>        opcode,
                        dag            outs,
	                dag            ins,
                        string         asmstr,
                        list<dag>      pattern,
	                InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  bits<5>  rd;
  bits<16> imm;

  let Inst{15-0}  = imm;
  let Inst{20-16} = rd;
  let Inst{25-21} = opcode;
  let Inst{31-26} = 0b101100;
}

class S32_FOP<bits<4>        opcode,
              bits<4>        cc,
              dag            outs,
	      dag            ins,
              string         asmstr,
              list<dag>      pattern,
	      InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  bits<5>  rd;
  bits<5>  ra;
  bits<5>  rb;
  bits<16> imm;

  let Inst{4-0}   = rb;
  let Inst{6-5}   = 0;
  let Inst{10-7}  = cc;
  let Inst{15-11} = ra;
  let Inst{20-16} = rd;
  let Inst{24-21} = opcode;
  let Inst{31-25} = 0b1100100;
}

class S32_FOPI<bits<4>        opcode,
               bits<4>        cc,
               dag            outs,
	       dag            ins,
               string         asmstr,
               list<dag>      pattern,
	       InstrItinClass itin> :
      VideoCore4Inst<outs, ins, asmstr, pattern, itin> {
  bits<5>  rd;
  bits<5>  ra;
  bits<6>  b;
  bits<16> imm;

  let Inst{5-0}   = b;
  let Inst{6}     = 1;
  let Inst{10-7}  = cc;
  let Inst{15-11} = ra;
  let Inst{20-16} = rd;
  let Inst{24-21} = opcode;
  let Inst{31-25} = 0b1100100;
}
