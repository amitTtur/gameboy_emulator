#pragma once
#include <vector>
#include <string>

struct operandElementHolder
{
	std::string name;
	short bytes = 1; // default =1 if not told otherwise
	bool immediate;
	bool decrement = false;
	bool increment = false;
};

struct OpcodeElementHolder
{
	bool immediate;
	std::string mnemonic;
	short bytes;
	short cycles;
	std::vector<operandElementHolder> operands;
};

// T is either a 8 bit or 16 bit variable
template <typename T>
struct operandReturn {
	std::string isRegister = ""; // if empty then no register, else the name of the register
	bool isMemoryLocation = false; // if a memory location in gb is accessed give control to the mbc.
	T value;
	uint16_t address = 0; // the address of value in case of value beign a memory location
};


///////
struct spriteSave {
	uint16_t tileAddress; // the memPlace of the sprite row (tile+his row)
	int xPos; // mem xpos - 8 | no y needed, checked at oam search
	bool pallate; // 1 or 0
	bool xFlip; // true yes, false no | no need to save yflip see oamsearch of fetcher to understand
	bool priority; // false-normal on top of bg and window, true under
	bool heightMode; // true - 16x8 , false 8x8 ,, if true then second byte is tileRow + 1  
};

struct pixelSave {
	uint8_t color; // value between 0-3
	bool pallate; // 0 is default for bg and window, 1 can be a value only for sprites
	// sprite priority -- non dmg only
	bool bgPriority; // 0 no - sprites over bg and window, 1- yes reverse.
};

