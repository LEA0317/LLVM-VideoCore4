//===- VideoCore4.td - Describe the VideoCore4 Target Machine -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This is the top level entry point for the VideoCore4 target.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"
include "VideoCore4Schedule.td"

//===----------------------------------------------------------------------===//
// Subtarget Features. 
//===----------------------------------------------------------------------===//

def FeatureVideoCore4 : SubtargetFeature<"vc4",
                                         "VideoCoreArchVersion", 
                                         "videocore4I",
		                         "videocore4I ISA Support",
		 		         []>;

//===----------------------------------------------------------------------===//
// VideoCore4 supported processors.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
  : ProcessorModel<Name, VC4GenericModel, Features>;
class ProcVc4<string Name, list<SubtargetFeature> Features>
  : ProcessorModel<Name, VC4Model, Features>;

// FIX ME (konda)
def : Proc   <"generic", [FeatureVideoCore4]>;
def : ProcVc4<"vc4",     [FeatureVideoCore4]>;

//===----------------------------------------------------------------------===//
// Register File Description
//===----------------------------------------------------------------------===//

include "VideoCore4RegisterInfo.td"

//===----------------------------------------------------------------------===//
// Calling Convention Description
//===----------------------------------------------------------------------===//

include "VideoCore4CallingConv.td"

//===----------------------------------------------------------------------===//
// Instruction Descriptions
//===----------------------------------------------------------------------===//

include "VideoCore4InstrInfo.td"

def VideoCore4InstrInfo : InstrInfo;

def VideoCore4InstPrinter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  bit isMCAsmWriter          = 1;
}

//===----------------------------------------------------------------------===//
// Target Declaration
//===----------------------------------------------------------------------===//

def VideoCore4 : Target {
  let InstructionSet  = VideoCore4InstrInfo;
  let AssemblyWriters = [VideoCore4InstPrinter];
}

