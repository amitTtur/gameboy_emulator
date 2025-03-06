#pragma once
#ifndef PPU_REGISTER_H
#define PPU_REGISTER_H

#include "Memory.h"

class REG_LCDC
{
public:
	REG_LCDC(Memory& mem);

	//getting bit index (between 7 and 0)
	//Bit 7 : LCD Display Enable
	//Bit 6 : Window Tile Map Display Select
	//Bit 5 : Window Display Enable
	//Bit 4 : BG & Window Tile Data Select
	//Bit 3 : BG Tile Map Display Select
	//Bit 2 : OBJ(Sprite) Size
	//Bit 1 : OBJ(Sprite) Display Enable
	//Bit 0 : BG & Window Display Enable
	bool getBit(const short index) const;

	//setting bit index (between 7 and 0)
	//Bit 7 : LCD Display Enable
	//Bit 6 : Window Tile Map Display Select
	//Bit 5 : Window Display Enable
	//Bit 4 : BG & Window Tile Data Select
	//Bit 3 : BG Tile Map Display Select
	//Bit 2 : OBJ(Sprite) Size
	//Bit 1 : OBJ(Sprite) Display Enable
	//Bit 0 : BG & Window Display Enable
	void setBit(const short index, const bool val) const;


private:
	uint8_t& _val;
};

class REG_STAT
{
public:
	REG_STAT(Memory& mem);

	// mode is bit 0 and 1 of the stat and varies between values
	// 0 to 3 based on the ppu current mode
	short getMode() const;
	void setMode(const short& val);

	// bit 2 of stat, if the lineCounter (LCY) is equal to the current line (LY)
	bool getLineComparator() const;
	void setLineComperator(const bool val);

	// if the ppu enters mode 0, this bit decides if a stat interrupt is raised or not
	bool mode0IEget() const;
	void mode0IEset(const bool val) const;

	// if the ppu enters mode 1, this bit decides if a stat interrupt is raised or not
	bool mode1IEget() const;
	void mode1IEset(const bool val) const;

	// if the ppu enters mode 2, this bit decides if a stat interrupt is raised or not
	bool mode2IEget() const;
	void mode2IEset(const bool val) const;

	// if bit 2 is set, this bit decides if a stat interrupt is raised or not
	bool LYCIEget() const;
	void LYCIEset(const bool val) const;


private:
	uint8_t& _val;
	void setBit(const short index, const bool val) const;
};

#endif // !PPU_REGISTER_H