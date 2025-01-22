#include <iostream>
#include "Gameboy.h"
#include "Register.hpp"
#include "Memory.h"


// the main should look inoccent while all the real work is happening in the other classes
int main()
{
    Gameboy gb("02-interrupts.gb");
    gb.run();
    return 0;
}