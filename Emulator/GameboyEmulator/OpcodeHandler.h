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

	bool _prefix; // if the previous command was prefix, do a prefix command in the next oppcode executed.

	Memory& _mem;

	OpcodeElementHolder* _currentOpcode;

	std::pair<int (OpcodeHandler::*)(), OpcodeElementHolder> _opcodes[0x100];
	std::pair<int (OpcodeHandler::*)(), OpcodeElementHolder> _CBopcodes[0x100];

	std::map<std::string, int (OpcodeHandler::*)()> _mnemonicToFunc;

	void initStrToFunc();
	void initializeOpcodes();
	inline json getOpcodesJsonContents();

	bool areOperands16bit();


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

	int INC_16();
	int DEC_16();
	int ADD_16();

	int JR();
	int RET(); // incldues RETI in it
	int JP();
	int CALL();
	int RST();

	int LD();

	int LD_16();
	int POP();
	int PUSH();
	int specialLD();

	int NOP();
	int STOP();
	int HALT();
	int PREFIX();
	int DI();
	int EI();

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
};


#endif // !OPCODE_FACTORY