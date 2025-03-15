#pragma once
#ifndef MBC1_H
#define MBC1_H

#include "MBC.h"

/*

memloc - 0x0 - 0x7FFF are read only, no write would affect those
writing to those address would affect the mbc registers

0000–1FFF — RAM Enable (when Writing Only)
2000–3FFF — ROM Bank Number (when Writing Only)
4000–5FFF — RAM Bank Number — or — Upper Bits of ROM Bank Number (when Writing Only)  || explanation in the registers
6000–7FFF — Banking Mode Select (when Writing Only)

*/


/*********************************************************************
* 
* HUGE disclaimer
* the mbc1 has multiple variants including mbc1m (mbc1 multy game)
* which is basicily a cartridge with multiple games in it, altough
* it is possible to implement this mbc variation it is not possible
* to detect whether or not a cartridge would like to use this specific
* mbc type. so as a result of limited doucumentation on this subject
* a not enough knowladge of how things were implemented or not 
* enough grounds to assume an implementation thi feature would 
* not be implemented. so a game that would use this mbc type would
* probably encounter undefined behaviour.
* 
***********************************************************************/

class MBC1 : public MBC
{
public:
	MBC1(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag);

	virtual MBC* getMBC() override;

private:
	virtual void updateBanks() override;
	virtual void bankSwitchUpdate(const uint16_t& address, const uint8_t& value) override;
};




#endif MBC1_H