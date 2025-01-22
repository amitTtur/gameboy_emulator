#pragma once
#ifndef NOMBC_H
#define NOMBC_H

#include "MBC.h"


class NoMBC : public MBC
{
public:
	NoMBC(uint8_t* memory, const std::string& romPath);

	virtual MBC* getMBC() override;

	virtual uint8_t read(const uint16_t& address) const override;
	virtual void write(const uint16_t& address, uint8_t value) override;

	virtual uint16_t read16(const uint16_t& address) const override;
	virtual void write16(const uint16_t& address, uint16_t value) override;

private:
	// the upper bits 7-4 are ignored ( bitwise end with 0xF )
	// by default it is turned of ( there is no accesss to SRAM )
	uint8_t _ramg = 0; // whether there is access to te SRAM (external cartrige ram)
};




#endif NOMBC_H