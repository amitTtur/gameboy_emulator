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
	MBC1(uint8_t* memory, const std::string& romPath);

	virtual MBC* getMBC() override;

	virtual uint8_t read(const uint16_t& address) const override;
	virtual void write(const uint16_t& address, uint8_t value) override;

	virtual uint16_t read16(const uint16_t& address) const override;
	virtual void write16(const uint16_t& address, uint16_t value) override;

private:
	// mode 0 - 0x0 - 0x3FFF and 0xA000 - 0xBFFF are locked to bank 0 of
	// ram and rom (limited to 8kb of ram, but support up to 2mb)
	bool _mode;

	/********* BANK REGISTERS ***********/

	// the upper bits 7-4 are ignored ( bitwise end with 0xF )
	// by default it is turned of ( there is no accesss to SRAM )
	uint8_t _ramg = 0; // whether there is access to te SRAM (external cartrige ram)


	// bits 7-5 are ignored  ( bitwise end with 0x1F )
	// it can never hold 0 (0b00000) as a value (duplication of bank 0, which is already loaded into memory)
	// as a result 0x0 0x20 0x40 0x60 will be turned to 0x1 
	uint8_t _bank1 = 1;
	uint8_t _bank1Mask;
	void setBank1(const uint8_t& val);

	// 0x4000 ~ 0x5FFF
	uint8_t _secondaryBank; // limited to only 2 bits
	uint8_t _secondaryBankMask = 0x3; // the mak of 2 bits
	void setSecondaryBank(const uint8_t& val);

	//
	uint8_t giveCurrentActiveRamBank() const;
	uint8_t giveCurrentActiveRomBank() const;
};




#endif MBC1_H