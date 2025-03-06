#pragma once
#ifndef NOMBC_H
#define NOMBC_H

#include "MBC.h"


class NoMBC : public MBC
{
public:
	NoMBC(uint8_t* memory, const std::string& romPath);

	virtual MBC* getMBC() override;

private:
	virtual void updateBanks() override;
};




#endif NOMBC_H