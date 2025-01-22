#pragma once

// for json file proccessing
#include "json.hpp"

// global variables
#include "globalVars.h"

// for exceptions
#include "GeneralException.h"

// If a constant or define is being used more than 1 time across multiple classes it should be placed here.

//general
#define RUN_PLAY 1; // normal run without a debugg menu
#define RUN_DEBUG 2; // a debug menu will run in the background
#define RUN_SUCCESS 100; // the emulator ran without error
#define RUN_FAIL 200; // an exception of sorts happend
#define RUN_INTERRUPTED -1; // in debug mode if the user decided to stop the run

//memory
#define MEMORY_BANK_SIZE 0x4000 // size of a single memory bank
#define RAM_BANK_SIZE 0x2000 // size of a single ram bank
#define BOOT_SIZE 0x100
#define BOOT_LOC "dmg_boot.bin"

//interrupts
#define INTERRUPT_ENABLE_LOC 0xFFFF //location of master interrupt enable
#define INTERRUPT_FLAG_LOC 0xFF0F //location of active interrupts flag
#define INT_VBLANK_LOC 0x40
#define INT_STAT_LOC 0x48
#define INT_TIMER_LOC 0x50
#define INT_SERIAL_LOC 0x58
#define INT_JOYPAD_LOC 0x60

//timing
#define SYS_FPS 59.7 //master fps
#define MECHINE_CYCLES_PER_FRAME 17564 //mechine cycles per frame achivable by the gameboy
#define CLOCK_CYCLES_PER_FRAME 70256 //clock cycles per frame achivable by the gameboy
#define TIMER_DIV_LOC 0xFF04 //location of div timer
#define TIMER_TIMA_LOC 0xFF05 //location of timer counter
#define TIMER_TMA_LOC 0xFF06 //location of timer modulo
#define TIMER_TAC_LOC 0xFF07 //location of timer control

//
//// memory offesets
//#define RAM_OFFSET 0x8000
//#define MEM_BANK_OFFSET 0x4000
//#define MEM_BANK_0_OFFSET 0x0 // always 0 const
