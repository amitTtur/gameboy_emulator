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
#define RAM_AREA_START 0xA000
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
#define LIMIT_SYS_FPS true
#define SYS_FPS 59.7 //master fps
#define MECHINE_CYCLES_PER_FRAME 17564 //mechine cycles per frame achivable by the gameboy
#define CLOCK_CYCLES_PER_FRAME 70256 //clock cycles per frame achivable by the gameboy
#define TIMER_DIV_LOC 0xFF04 //location of div timer
#define TIMER_TIMA_LOC 0xFF05 //location of timer counter
#define TIMER_TMA_LOC 0xFF06 //location of timer modulo
#define TIMER_TAC_LOC 0xFF07 //location of timer control

//screen
#define SCREEN_HEIGHT 144
#define SCREEN_WIDTH 160
#define SCREEN_TO_WORLD_PIXEL 4
#define WINDOW_NAME "gameboy"
#define COLOR_BLACK 0x00, 0x00, 0x00, 0xFF
#define COLOR_DARK_GRAY 0x50, 0x50, 0x50, 0xFF
#define COLOR_LIGHT_GRAY 0xc0, 0xc0, 0xc0, 0xFF
#define COLOR_WHITE 0xFF, 0xFF, 0xFF, 0xFF

//ppu
#define REG_LCDC_LOC 0xFF40
#define REG_STAT_LOC 0xFF41
#define LY_LOC 0xFF44
#define LYC_LOC 0xFF45
#define SCX_LOC 0xFF43
#define SCY_LOC 0xFF42
#define WX_LOC 0xFF4B
#define WY_LOC 0xff4a
#define DMA_LOC 0xff46
#define TILE_SET_1 0x8000
#define TILE_SET_2 0x8800
#define TILE_MAP_1_BGWIN 0x9800
#define TILE_MAP_2_BGWIN 0x9C00
#define BG_PLATTE_LOC 0xFF47
#define SP_PLATTE0_LOC 0xFF48
#define SP_PLATTE1_LOC 0xFF49
#define MAP_SIZE 256
#define CYCLES_OAM 80
#define CYCLES_TRANSFER 172 
#define CYCLES_HBLANK 204
#define CYCLES_VBLANK 456
#define OAM_MEMORY_START 0xFE00

//input
#define INPUT_REG_LOC 0xff00

//
//// memory offesets
//#define RAM_OFFSET 0x8000
//#define MEM_BANK_OFFSET 0x4000
//#define MEM_BANK_0_OFFSET 0x0 // always 0 const
