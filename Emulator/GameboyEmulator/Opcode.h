#pragma once
#include "Memory.h"
#include "dataStructures.hpp"

#ifndef OPCODE
#define OPCODE

/******************

PREEEEEETYYYYYYYYYY big explaination:

when doing an opcode there are two things that can happened:
1. write to a register
2. write to memory

writing to a register is direct with no dependancy on anything
writing to a memory location need to go through the mbc in case any special registers are involved

*******************/





class Opcode {

public:
	Opcode(Memory& mem_ref) : _mem(mem_ref) {};
	virtual ~Opcode() = default;

	virtual void printOpcodeMnemonic() const = 0;
	virtual int run() = 0;

protected:

	Memory& _mem;
};



#endif // !OPCODE