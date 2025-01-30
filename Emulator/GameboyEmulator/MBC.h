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

	virtual uint8_t read(const uint16_t& address) const = 0;
	virtual void write(const uint16_t& address,uint8_t value) = 0;
	virtual uint16_t read16(const uint16_t& address) const = 0;

	// only 1 instruction uses it, LD (a16), SP | it is not made to change the 
	// registers since its supposed to save the stack pointer. if a write
	// would occurto a memory unsave regions, eg... mbc registers,
	// io memory locaction it would overrid the memory at that location
	// because its a given that this instruction should not do that
	virtual void write16(const uint16_t& address, uint16_t value) = 0;

	// creates a copy of local rom at the requested folder with the given name, if file
	// already exists it will override it
	// given no folder path it will create it at the downloads folder, given it exists.
	void createRomFileCopy(const std::string& folderPath = "", const std::string& copyName = "");


	static uint8_t* getRomHeaders(const std::string& romPath);

protected:
	uint8_t* _mem;
	cartridgeView _romView;
	std::vector<char> _romFileCopy;
	uint8_t _activeRomBank;
	uint8_t _activeRamBank;
	
	// load the external ram to 0xA000 to 0xBFFF
	// changes active ram to given ram number
	void LoadRam(const uint8_t& ramBankNumber);
	// max banks number is 256 (such an odd number)
	// changes active rom to given rom number
	virtual void loadBankToMem(const uint8_t& bankNumber);


private:
	void loadRomToCopy(const std::string& romPath);

	// on hold at the moment
	// ONLY USE 1 TIME AT THE START OF THE RUN
	 void loadBank0();


	 //TODO: add a bios load at 0 - 0xFF
	 void loadBiosToMem();

	// SAVE FILES
	// Since a rom can save its save values at diffrent places a new rom is created with the save 
};


#endif MBC_H