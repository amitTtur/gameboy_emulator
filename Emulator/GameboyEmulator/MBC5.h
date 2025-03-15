#pragma once
#ifndef MBC5_H
#define MBC5_H

#include "MBC.h"

/*

memloc - 0x0 - 0x7FFF are read only, no write would affect those
writing to those address would affect the mbc registers

0000–1FFF — RAM Enable (when Writing Only)
2000–3FFF — ROM Bank Number (when Writing Only)
4000–5FFF — RAM Bank Number — or — Upper Bits of ROM Bank Number (when Writing Only)  || explanation in the registers

***
* PLEASE NOTE: BIT 3 OF RAM BANK IS A RUMBLE INDICATOR
***

*/


class MBC5 : public MBC
{
public:
	MBC5(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag);

	virtual MBC* getMBC() override;

private:
	const unsigned short ROM_BANK_1_LOW = 0x2000;
	const unsigned short ROM_BANK_1_HIGH = 0x2FFF;
	uint8_t _romBankMsb; // the msb of the rom bank number
	// secontary bank (_bank2) will take the role of ram bank here.
	const unsigned short ROM_BANK_2_LOW = 0x3000;
	const unsigned short ROM_BANK_2_HIGH = 0x3FFF;
	/*
	* in mbc class
	const unsigned short RAM_BANK_HIGH = 0x5FFF;
	const unsigned short RAM_BANK_LOW = 0x4000;
	*/
	
	virtual void updateBanks() override;
	virtual void bankSwitchUpdate(const uint16_t& address, const uint8_t& value) override;
};




#endif // !MBC5_H