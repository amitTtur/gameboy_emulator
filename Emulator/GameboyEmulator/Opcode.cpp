#include "Opcode.h"

Opcode::Opcode(Memory& mem_ref, OpcodeElementHolder* current_opcode) : _mem(mem_ref), _currentOpcode(current_opcode) {}

OpcodeElementHolder* Opcode::getCurrentOpcode()
{
    return _currentOpcode;
}
