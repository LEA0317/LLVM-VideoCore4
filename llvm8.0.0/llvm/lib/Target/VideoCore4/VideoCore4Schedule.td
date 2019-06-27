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
def ALU     : FuncUnit;
def FPU     : FuncUnit;
def LSU     : FuncUnit;

//===----------------------------------------------------------------------===//
// Instruction Itinerary classes used for VideoCore4
//===----------------------------------------------------------------------===//
def IIALU              : InstrItinClass;
def IIFPU              : InstrItinClass;
def IILSU              : InstrItinClass;
def IIPseudo           : InstrItinClass;

//===----------------------------------------------------------------------===//
// VideoCore4 Generic instruction itineraries.
//===----------------------------------------------------------------------===//
def VC4GenericItineraries : ProcessorItineraries<[ALU, LSU, FPU], [], [
  InstrItinData<IIALU              , [InstrStage<1, [ALU]>]>,
  InstrItinData<IILSU              , [InstrStage<1, [LSU]>]>,
  InstrItinData<IIFPU              , [InstrStage<1, [FPU]>]>,
  InstrItinData<IIPseudo           , [InstrStage<1, [ALU]>]>
]>;

// FIX ME
def VC4GenericModel : SchedMachineModel {
  let IssueWidth        = 1;  // Max micro-ops that may be scheduled per cycle.
  let LoadLatency       = 20; // Cycles for loads to access the cache.
  let HighLatency       = 38; // Approximation of cycles for "high latency" ops.
  let MicroOpBufferSize = 0; // in-order
  let MispredictPenalty = 10; // Extra cycles for a mispredicted branch.

  // Per-cycle resources tables.
  let Itineraries     = VC4GenericItineraries;
  let PostRAScheduler = 1;

  // TODO: this is templary used in developing phase
  let CompleteModel = 0;
}

include "VideoCore4ScheduleVC4.td"