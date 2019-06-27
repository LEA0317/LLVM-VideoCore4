//===-- VideoCore4ScheduleVC4.td - VideoCore4 Scheduling Definitions ------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

def VC4Itineraries : ProcessorItineraries<[ALU, LSU, FPU], [], [
  InstrItinData<IIALU              , [InstrStage<1, [ALU]>]>,
  InstrItinData<IILSU              , [InstrStage<1, [LSU]>]>,
  InstrItinData<IIFPU              , [InstrStage<1, [FPU]>]>,
  InstrItinData<IIPseudo           , [InstrStage<1, [ALU]>]>
]>;

// FIX ME
def VC4Model : SchedMachineModel {
  let IssueWidth        = 1;  // Max micro-ops that may be scheduled per cycle.
  let MicroOpBufferSize = 0; 
  let LoadLatency       = 20; // Cycles for loads to access the cache.
  let HighLatency       = 38; // Approximation of cycles for "high latency" ops.
  let MicroOpBufferSize = 0; //	in-order
  let MispredictPenalty = 10; // Extra cycles for a mispredicted branch.

  // Per-cycle resources tables.
  let Itineraries     = VC4Itineraries;
  let PostRAScheduler = 1;

  // TODO: this is templary used in developing phase
  let CompleteModel = 0;
}