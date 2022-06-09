//===-- VideoCore4Schedule.td - VideoCore4 Scheduling Definitions ------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Functional units across VideoCore4 chips sets. 
//===----------------------------------------------------------------------===//
// temporal
def FPU : FuncUnit;
def ALU : FuncUnit;
def LSU : FuncUnit;
def SFU : FuncUnit;

//===----------------------------------------------------------------------===//
// Instruction Itinerary classes used for VideoCore4
//===----------------------------------------------------------------------===//
def IIFpu    : InstrItinClass;
def IIAlu    : InstrItinClass;
def IILsu    : InstrItinClass;
def IISfu    : InstrItinClass;
def IIBranch : InstrItinClass;
def IIPseudo : InstrItinClass;

//===----------------------------------------------------------------------===//
// VideoCore4 Generic instruction itineraries.
//===----------------------------------------------------------------------===/

// FIX ME
def VC4GenericItineraries : ProcessorItineraries<[ALU, LSU, SFU, FPU], [], [
  InstrItinData<IIBranch, [InstrStage<4,  [ALU]>]>,
  InstrItinData<IIAlu   , [InstrStage<4,  [ALU]>]>,
  InstrItinData<IIFpu   , [InstrStage<4,  [FPU]>]>,
  InstrItinData<IILsu   , [InstrStage<4,  [ALU, LSU]>]>,
  InstrItinData<IISfu   , [InstrStage<16, [SFU]>]>,
  InstrItinData<IIPseudo, [InstrStage<4,  [ALU], 0>, InstrStage<4, [ALU]>]>
]>;

// FIX ME
def VC4GenericModel : SchedMachineModel {
  let IssueWidth        = 2;  // Max micro-ops that may be scheduled per cycle.
  let LoadLatency       = 4;  // Cycles for loads to access the cache.
  let HighLatency       = 16; // Approximation of cycles for "high latency" ops.
  let MicroOpBufferSize = 0;  // in-order
  let MispredictPenalty = 10; // Extra cycles for a mispredicted branch.

  // Per-cycle resources tables.
  let Itineraries     = VC4GenericItineraries;
  let PostRAScheduler = 1;

  // TODO: this is templary used in developing phase
  let CompleteModel = 0;
}

include "VideoCore4ScheduleVC4.td"