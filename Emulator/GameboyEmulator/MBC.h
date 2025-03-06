#pragma once
#ifndef MBC_H
#define MBC_H

#include <cstdint>
#include <fstream>
#include <vector>
#include <filesystem>
#include "GeneralException.h"
#include "CodesAndDefines.h"
#include "CartridgeView.h"

//#define DOWNLOADS_FOLDER_PATH_CODE "374DE290-123F-4565-9164-39C4925E467B"


class MBC
{
public:
	MBC() = delete; // dont want this contractor
    MBC(const MBC&) = delete;

	MBC(uint8_t* memory, const std::string& romPath);
	~MBC();

	// cant use smart pointers, gives allocation errors
	virtual MBC* getMBC() = 0;

	virtual uint8_t read(const uint16_t& address) const;
	virtual void write(const uint16_t& address,uint8_t value);
	virtual uint16_t read16(const uint16_t& address) const;

	// only 1 instruction uses it, LD (a16), SP | it is not made to change the 
	// registers since its supposed to save the stack pointer. if a write
	// would occurto a memory unsave regions, eg... mbc registers,
	// io memory locaction it would overrid the memory at that location
	// because its a given that this instruction should not do that
	virtual void write16(const uint16_t& address, uint16_t value);

	// creates a copy of local rom at the requested folder with the given name, if file
	// already exists it will override it
	// given no folder path it will create it at the downloads folder, given it exists.
	void createRomFileCopy(const std::string& folderPath = "", const std::string& copyName = "");


	static uint8_t* getRomHeaders(const std::string& romPath);

protected:
	uint8_t* _mem;
	cartridgeView _romView;
	std::vector<char> _romFileCopy;

	bool _isNoMbc;

	// on initialization a resize is called putting it at the size of the actual sram
	std::vector<uint8_t> _sram;
	// calling it more then once can lead to an undefined behaviour
	void resizeSram(RamSize size); // will only be called once a gb file
	
	// load the external ram to 0xA000 to 0xBFFF
	// changes active ram to given ram number
	void LoadRam(const uint8_t& ramBankNumber);
	// max banks number is 256 (such an odd number)
	// changes active rom to given rom number
	virtual void loadBankToMem(const uint8_t& bankNumber,bool range);

	bool checkTimerWrite(uint16_t address, uint8_t value);

	/******* Bank switching  *********/
	// no ram
	void bankSwitchUpdate(bool noMbc,const uint16_t& address, const uint8_t& value);
	virtual void updateBanks() = 0;
	const unsigned short BANK_SELECT_HIGH = 0x7FFF;
	const unsigned short BANK_SELECT_LOW = 0x6000;
	const unsigned short RAM_BANK_HIGH = 0x5FFF;
	const unsigned short RAM_BANK_LOW = 0x4000;
	const unsigned short RAM_ENABLE_HIGH = 0x1FFF;
	const unsigned short RAM_ENABLE_LOW = 0x0000;
	const unsigned short ROM_BANK_SET_HIGH = 0x3FFF;
	const unsigned short ROM_BANK_SET_LOW = 0x2000;

	// mode 0 - 0x0 - 0x3FFF and 0xA000 - 0xBFFF are locked to bank 0 of
	// ram and rom (limited to 8kb of ram, but support up to 2mb)
	bool _mode;
	/********* BANK REGISTERS ***********/

	// the upper bits 7-4 are ignored ( bitwise end with 0xF )
	// by default it is turned of ( there is no accesss to SRAM )
	bool _ramg = false; // whether there is access to te SRAM (external cartrige ram)


	// bits 7-5 are ignored  ( bitwise end with 0x1F )
	// it can never hold 0 (0b00000) as a value (duplication of bank 0, which is already loaded into memory)
	// as a result 0x0 0x20 0x40 0x60 will be turned to 0x1 
	uint8_t _bank1 = 1;
	uint8_t _bank1Mask;
	uint8_t _bank2;
	uint8_t _ramMask;
	uint8_t _activeRomBank; // the currently loaded rom bank (0x4000 - 0x7FFF)
	uint8_t _activeRamBank; // the currently loaded ram bank (0xA000 - 0xBFFF)
	uint8_t _activeRomBank0; // the currently loaded rom bank (0x0000 - 0x3FFF)

private:
	void loadRomToCopy(const std::string& romPath);


	 //TODO: add a bios load at 0 - 0xFF
	 void loadBiosToMem();

	// SAVE FILES
	// Since a rom can save its save values at diffrent places a new rom is created with the save 
};


#endif MBC_H