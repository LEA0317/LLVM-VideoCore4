//===-- VideoCore4ScheduleVC4.td - VideoCore4 Scheduling Definitions ------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// FIX ME
def VC4Itineraries : ProcessorItineraries<[ALU, LSU, SFU, FPU], [], [
  InstrItinData<IIBranch, [InstrStage<4,  [ALU]>]>,
  InstrItinData<IIAlu   , [InstrStage<4,  [ALU]>]>,
  InstrItinData<IIFpu   , [InstrStage<4,  [FPU]>]>,
  InstrItinData<IILsu   , [InstrStage<4,  [ALU, LSU]>]>,
  InstrItinData<IISfu   , [InstrStage<16, [SFU]>]>,
  InstrItinData<IIPseudo, [InstrStage<4,  [ALU], 0>, InstrStage<4, [ALU]>]>
]>;

// FIX ME
def VC4Model : SchedMachineModel {
  let IssueWidth        = 2; // Max micro-ops that may be scheduled per cycle.
  let LoadLatency       = 4; // Cycles for loads to access the cache.
  let HighLatency       = 16; // Approximation of cycles for "high latency" ops.
  let MicroOpBufferSize = 0; // in-order
  let MispredictPenalty = 10; // Extra cycles for a mispredicted branch.

  // Per-cycle resources tables.
  let Itineraries     = VC4Itineraries;
  let PostRAScheduler = 1;

  // TODO: this is templary used in developing phase
  let CompleteModel = 0;
}