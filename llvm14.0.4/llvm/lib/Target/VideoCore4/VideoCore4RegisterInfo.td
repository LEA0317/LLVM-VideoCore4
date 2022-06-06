//===-- VideoCore4RegisterInfo.td - VideoCore4 Register defs --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the VideoCore4 register file
//===----------------------------------------------------------------------===//

class VideoCore4Reg<string n> : Register<n> {
  field bits<5> Num = 0;
  let Namespace = "VideoCore4";
}

// Alias reg
class GRegAlias<string n, VideoCore4Reg R> : VideoCore4Reg<n> {
  let Num = R.Num;
}

//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//

foreach i = 0-31 in {
  def R#i : VideoCore4Reg<"r"#i>;
}
def TMP : GRegAlias<"tmp", R23>;
def GP  : GRegAlias<"gp",  R24>;
def SP  : GRegAlias<"sp",  R25>;
def LR  : GRegAlias<"lr",  R26>;
def ESP : GRegAlias<"esp", R28>;
def SR  : GRegAlias<"sr",  R30>;
def PC  : GRegAlias<"pc",  R31>;

//===----------------------------------------------------------------------===//
//  Register Classes
//===----------------------------------------------------------------------===//

def SR_CLASS : RegisterClass<"VideoCore4", [i32], 32, (add SR)> {
  let CopyCost = -1;  // Don't allow copying of status registers.

  // SR_CLASS is not allocatable.
  let isAllocatable = 0;
}

/* All registers */
def GR32 : RegisterClass<"VideoCore4", [i32, f32], 32,
    (add (sequence "R%u", 0, 29), TMP, GP, SP, LR)>;

/* Fast registers */
def FR32 : RegisterClass<"VideoCore4", [i32, f32], 32,
    (sequence "R%u", 0, 15)>;

/* Indexable registers */
def IR32 : RegisterClass<"VideoCore4", [i32], 32,
	(add R0, GP, SP)>;

def PUSHPOP_START_REGS : RegisterClass<"VideoCore4", [i32], 32,
	(add R0, R6, R16, GP)>;