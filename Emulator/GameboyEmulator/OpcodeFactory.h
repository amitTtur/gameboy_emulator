#pragma once
#include <map>
#include <fstream>
#include <memory>
#include <set>

// json redefining as a new name type.
#include "./CodesAndDefines.h"
using json = nlohmann::json;
#include "Opcode.h"
#include "arithmetic8bit.h"
#include "Load8Bit.h"
#include "Load16Bit.h"
#include "arithmetic16bit.h"
#include "JumpCallOpcodes.h"
#include "ShiftRotate8bit.h"
#include "MiscOpcodes.h"

#ifndef OPCODE_FACTORY
#define OPCODE_FACTORY



class OpcodeFactory {
public:
	OpcodeFactory(Memory& memoryReferance);
	OpcodeFactory(OpcodeFactory&) = delete; // the class should be created only through normal constarctor not a copy one;

	// DELETE AFTER USE.
	Opcode* getInstance(uint8_t opcode);

	void setPrefix(const bool& val);
	bool getPrefix() const;

private:
	bool _prefix; // if the previous command was prefix, do a prefix command in the next oppcode executed.



	Memory& _memRef;
	// unprefix the cbPrefix in the first map, in the second (inner)
	// map is the opcodes hexadecimal number then that opcodes contents.
	std::map<std::string, std::map<int, OpcodeElementHolder>> _opcodesMap;

	void initializeOpcodeMap();
	inline json getOpcodesJsonContents();

	inline bool isArithmetic(const std::string& mnemonic) const;

	inline bool isLoad(const std::string& mnemonic) const;

	inline bool isShift(const std::string& mnemonic) const;

	inline bool isMisc(const std::string& mnemonic) const;

	inline bool isJump(const std::string& mnemonic) const;

	// shifters that are in the normal table
	inline bool isUniqueShifters(const std::string& mnemonic) const;

	inline bool areOperands16BitsLD(const OpcodeElementHolder& opcode);

	std::set<std::string> _arithmeticNames = { "INC", "DEC", "ADD", "SUB", "ADC", "SBC", "AND", "XOR", "OR", "CP", "DAA", "CPL", "SCF", "CCF" };
	std::set<std::string> _loadNames = { "LD", "LDH", "POP", "PUSH" };
	std::set<std::string> _shiftNames = { "RLC", "RRC", "RL", "RR", "SLA", "SRA", "SWAP", "SRL", "BIT", "RES", "SET" };
	std::set<std::string> _miscNames = { "NOP", "STOP", "HALT", "PREFIX", "EI", "DI" };
	std::set<std::string> _jumpNames = { "JR", "JP", "RET", "RETI", "RST", "CALL"};
	std::set<std::string> _uniqueShftersNames = { "RLCA", "RLA", "RRCA", "RRA"};
};


#endif // !OPCODE_FACTORY