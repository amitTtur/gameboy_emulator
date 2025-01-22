#pragma once
#ifndef CARTRIDGE_VIEW_H
#define CARTRIDGE_VIEW_H

#include <cstdint>
#include <vector>
#include <iostream>

/*

SPLECULATION

-rom files are divided into two part rom and ram (if any)

rom start from 0x0 to 0x4000 * number of existing rom bank (non inclusive)

ram start from that offset untill eof


*/

// add an offset of 0x100 | it is the agreed range of the gb headers 
const int headerRange = 0x014F - 0x100;

enum CartridgeConsts
{
	cartridge_type = 0x147, // the type of mbc the game requests 
	rom_size = 0x148,
	ram_size = 0x149,
	cgb_flag = 0x143, // if the game supports gameboy color, in this emulator we will ignore this flag
	sgb_flag = 0x146, // if the game supports gameboy super, in this emulator we will ignore this flag
};

enum MbcType {
	romOnly = 0,
	mbc1 = 1,
	mbc2 = 2,
	mbc3 = 3,
	mbc4 = 4,
	mbc5 = 5,
	unknown = -1
};

// each bank is 16kb
enum RomSize {
	KB32 = 2,
	KB64 = 4,
	KB128 = 8,
	KB256 = 16,
	KB512 = 32,
	MB1 = 64,
	MB2 = 128,
	MB4 = 256,
	MB8 = 512,

	// those are speculative and are not verified by a reliable source
	/*MB1p1,
	MB1p2,
	MB1p5,*/
};

// each bank is 8kb
enum RamSize {
	None = 0,
	// KB2, used by homebrew roms and not used or recognized by the original gb or roms at all. it will be ignored
	ramKB8 = 1,
	ramKB32 = 4,
	ramKB128 = 16,
	ramKB64 = 8,
};


class cartridgeView 
{
public:
	cartridgeView(const uint8_t* romCpy);
	~cartridgeView();

	// TODO, beteween 0x104-0x133
	bool checkNintendoSymbol() const;

	void printROMdetails() const;

	MbcType _type;
	RomSize _romSize;
	RamSize _ramSize;
	bool _ramSupport;
	bool _timerSupport; // MBC3 only feature (0xF and 0x10 modes only)
	/// <summary>
	/// in the gameboy cartridge also had a battery option, which is basicily a autosave function.
	/// in this emulator the user will decide if to save a copy of the current rom.
	/// </summary>
	
	/// <summary>
	///  the mbc5 (which is the most advanced mbc the gb basic has) have a rumble mode
	///  the rumble mode is basicily a vibration sensor. in this emulator since it is 
	///  aimed towards a windows pc's it will not support the rumble mode and will ignore rumble instuctions.
	/// </summary>

	static MbcType getType(const uint8_t* romCpy);

private:
	// it is a temp value that should not be changed and should only be read from
	const uint8_t* _romCpy;
	//
	MbcType getType();
	RomSize getRomSize();
	RamSize getRamSize();
};


#endif // !CARTRIDGE_VIEW_H
