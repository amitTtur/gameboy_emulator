#pragma once
#ifndef NOMBC_H
#define NOMBC_H

#include "MBC.h"


class NoMBC : public MBC
{
public:
	NoMBC(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag);

	virtual MBC* getMBC() override;

private:
	virtual void updateBanks() override;
	virtual void bankSwitchUpdate(const uint16_t& address, const uint8_t& value) override;
};




#endif NOMBC_H