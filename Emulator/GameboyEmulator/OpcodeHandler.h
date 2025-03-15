#include <map>
#include <fstream>
#include <memory>
#include <set>
#include "Memory.h"

#include "./CodesAndDefines.h"
using json = nlohmann::json;


#ifndef OPCODE_HANDLER
#define OPCODE_HANDLER



class OpcodeHandler {
public:
	OpcodeHandler(Memory& memoryReferance);

	int run(uint8_t opcode);

	void setPrefix(const bool& val);
	bool getPrefix() const;

	OpcodeElementHolder* getCurrentOpcode();

private:

	// if the previous command was prefix, do a prefix command in the next oppcode executed.
	bool _prefix;

	Memory& _mem;

	OpcodeElementHolder* _currentOpcode;

	//opcodes map
	std::pair<int (OpcodeHandler::*)(), OpcodeElementHolder> _opcodes[0x100];
	std::pair<int (OpcodeHandler::*)(), OpcodeElementHolder> _CBopcodes[0x100];

	//for easy conversion
	std::map<std::string, int (OpcodeHandler::*)()> _mnemonicToFunc;

	//init opcode map with operands
	void initStrToFunc();
	void initializeOpcodes();

	//json handling
	inline json getOpcodesJsonContents();

	//check for operands
	bool areOperands16bit();

	//arithmetic
	int OR();
	int INC();
	int DEC();
	int ADD();
	int SUB();
	int SBC();
	int ADC();
	int AND();
	int XOR();
	int CP();
	int DAA();
	int CPL();
	int SCF();
	int CCF();

	//jumps
	int JR();
	int RET(); // incldues RETI in it
	int JP();
	int CALL();
	int RST();

	//loads
	int LD();
	int POP();
	int PUSH();
	int specialLD();

	//shifts and rotates
	int RLC();
	int RRC();
	int RL();
	int RR();
	int SLA();
	int SRA();
	int SWAP();
	int SRL();
	int BIT();
	int RES();
	int SET();
	int RLCA();
	int RRCA();
	int RLA();
	int RRA();

	//misc
	int NOP();
	int STOP();
	int HALT();
	int PREFIX();
	int DI();
	int EI();
};


#endif