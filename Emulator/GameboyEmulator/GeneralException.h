#pragma once
#include <string>
#include <exception>
#include <iostream>

// saved numbers that indicate an error
enum {
	UNKNOWN_OPCODE = 1, // unknown opcode
	FILE_WONT_OPEN = 2, // self explained
	NO_NAME_FOUND = 3, // if the json has a opcodes that has no mnemonic or oprand doesnt have a name. SERIOUS ERROR
	UNKNWON_OPERAND = 4,
	ERROR_HANDELING_ROM = 5, // if read from or write to rom has failed
	UNDEFINED_INSTRUCTION_BEHAVIOR = 6, // if an instruction asks for something weird
	MBC_UNDEFINED_BEHAVIOR = 7, // somthing weird happend in the MBC
	MBC_NOT_SUPPORTED = 8, // somthing weird happend in the MBC
};

class GeneralException : public std::exception // basic exception class
{
public:
	GeneralException(const std::string& message, const int code = 1)
		: _var{ code,message }, _message("Code: " + std::to_string(_var.code) + ", Message: " + _var.e) {};
	virtual ~GeneralException() noexcept = default;
	virtual const char* what() const noexcept { return _message.c_str(); };


protected:
	struct { 
		int code;
		std::string e;
	} _var;

	std::string _message;
};