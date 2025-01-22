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

