#pragma once


#include "json.hpp"
#include "globalVars.h"
#include "GeneralException.h"


//general
#define RUN_PLAY 1; // normal run without a debugg menu
#define RUN_DEBUG 2; // a debug menu will run in the background
#define RUN_SUCCESS 100; // the emulator ran without error
#define RUN_FAIL 200; // an exception of sorts happend
#define RUN_INTERRUPTED -1; // in debug mode if the user decided to stop the run

//memory
#define MEMORY_BANK_SIZE 0x4000 // size of a single memory bank
#define RAM_BANK_SIZE 0x2000 // size of a single ram bank
#define RAM_AREA_START 0xA000  //location of ram start
#define BOOT_SIZE 0x100 //size of boot rom
#define BOOT_LOC "dmg_boot.bin" //name of boot file
#define RAM_START 0xA000 //location of ram start

//interrupts
#define INTERRUPT_ENABLE_LOC 0xFFFF //location of master interrupt enable
#define INTERRUPT_FLAG_LOC 0xFF0F //location of active interrupts flag
#define INT_VBLANK_LOC 0x40 //location to jump to when vblank interrupt is called
#define INT_STAT_LOC 0x48 //location to jump to when stat interrupt is called
#define INT_TIMER_LOC 0x50 //location to jump to when timer interrupt is called
#define INT_SERIAL_LOC 0x58 //location to jump to when serial interrupt is called
#define INT_JOYPAD_LOC 0x60 //location to jump to when joypad interrupt is called

//timing
#define LIMIT_SYS_FPS true //debugging - limit the systems fps
#define SYS_FPS 59.7 //master fps
#define MECHINE_CYCLES_PER_FRAME 17564 //mechine cycles per frame achivable by the gameboy
#define CLOCK_CYCLES_PER_FRAME 70256 //clock cycles per frame achivable by the gameboy
#define CLOCK_CYCLES_PER_SECOND 4194304 //clock cycles per second achivable by the gameboy
#define TIMER_DIV_LOC 0xFF04 //location of div timer
#define TIMER_TIMA_LOC 0xFF05 //location of timer counter
#define TIMER_TMA_LOC 0xFF06 //location of timer modulo
#define TIMER_TAC_LOC 0xFF07 //location of timer control

//screen
#define SCREEN_HEIGHT 144 //in pixels
#define SCREEN_WIDTH 160 //in pixels
#define WINDOW_NAME "Gameboy EmulAAtor" //window name in windows
#define COLOR_BLACK1 0x00, 0x00, 0x00, 0xFF //palette 1 black
#define COLOR_DARK_GRAY1 0x50, 0x50, 0x50, 0xFF //palette 1 dark gray
#define COLOR_LIGHT_GRAY1 0xc0, 0xc0, 0xc0, 0xFF //palette 1 light gray
#define COLOR_WHITE1 0xFF, 0xFF, 0xFF, 0xFF //palette 1 white

#define COLOR_BLACK2 0x2d, 0x1b, 0x00, 0xFF //palette 2 black
#define COLOR_DARK_GRAY2 0x1e, 0x60, 0x6e, 0xFF //palette 2 dark gray
#define COLOR_LIGHT_GRAY2 0x5a, 0xb9, 0xa8, 0xFF //palette 2 light gray
#define COLOR_WHITE2 0xc4, 0xf0, 0xc2, 0xFF //palette 2 white

//ppu
#define REG_LCDC_LOC 0xFF40 //location of lcdc register
#define REG_STAT_LOC 0xFF41 //location of stat register
#define LY_LOC 0xFF44 //location of current line on screen
#define LYC_LOC 0xFF45 //location of y offset
#define SCX_LOC 0xFF43 //location of scroll x
#define SCY_LOC 0xFF42 //location of scroll y
#define WX_LOC 0xFF4B //location of window x
#define WY_LOC 0xff4a //location of window y
#define DMA_LOC 0xff46 //location of dma refister
#define TILE_SET_1 0x8000 //location of tile set 1
#define TILE_SET_2 0x8800 //location of tile set 2
#define TILE_MAP_1_BGWIN 0x9800 //location of tile map 1 (bg and window)
#define TILE_MAP_2_BGWIN 0x9C00 //location of tile map 2 (bg and window)
#define BG_PLATTE_LOC 0xFF47 //location of palette for bg and window
#define SP_PLATTE0_LOC 0xFF48 //location of sprite palette 0
#define SP_PLATTE1_LOC 0xFF49 //location of sprite palette 1
#define MAP_SIZE 256 //size of tile map
#define CYCLES_OAM 80 //time for oam search in t cycles
#define CYCLES_TRANSFER 172 //time for transfer in t cycles
#define CYCLES_HBLANK 204 //time for hblank in t cycles
#define CYCLES_VBLANK 456 //time for vblank in t cycles
#define OAM_MEMORY_START 0xFE00 //location of oam memory

//input
#define INPUT_REG_LOC 0xff00 //location of input register

//config
#define CONFIG_FILE_NAME "config.txt" //name of config file
