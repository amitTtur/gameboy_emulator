#pragma once
#ifndef MBC2_H
#define MBC2_H

#include "MBC.h"

/*

memloc - 0x0 - 0x7FFF are read only, no write would affect those
writing to those address would affect the mbc registers

0000–1FFF — RAM Enable (when Writing Only)
2000–3FFF — ROM Bank Number (when Writing Only)
4000–5FFF — RAM Bank Number — or — Upper Bits of ROM Bank Number (when Writing Only)  || explanation in the registers
6000–7FFF — Banking Mode Select (when Writing Only)

*/

/*******

DISCALIMER -- in mbc2 memory range 0xA200-0xBFFF are undefined

********/

#define GET_ACTUAL_MBC2_RAM_ADDRESS(address) ((address) & 0x1FF)

class MBC2 : public MBC
{
public:
	MBC2(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag);

	virtual MBC* getMBC() override;

private:
	virtual void updateBanks() override;
	virtual void bankSwitchUpdate(const uint16_t& address, const uint8_t& value) override;

	// special read write happend in ram range in mbc2
	virtual void writeToRam(const uint16_t& address, const uint8_t& value) override;
	virtual uint8_t readFromRam(const uint16_t& address) const override;

	const int BANK_SELECT_RANGE = 0x4000;
};




#endif // !MBC2_H