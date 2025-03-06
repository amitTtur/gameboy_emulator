#include <iostream>
#include "Gameboy.h"
#include "Register.hpp"
#include "Memory.h"
#include <chrono>


// the main should look inoccent while all the real work is happening in the other classes
int main()
{
    Gameboy gb("dmg-acid2.gb");

    //std::this_thread::sleep_for(std::chrono::seconds(1));

    gb.run();

    return 0;
}