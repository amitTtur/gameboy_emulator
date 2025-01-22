#include <cstdint>
#include <vector>
#include <string>
#include <cstdint>
#include <map>
#include "CodesAndDefines.h"

#ifndef REGISTER_H
#define REGISTER_H

template <typename T>
class Register {
public:
	const std::string _name;

	Register(std::string name, T& val) : _name(name), _value(val) {};

    Register& operator=(Register& other) {
        _value = other._value;
        return *this;
    }

    operator T() //returns value
    {
        return _value;
    }

    Register& operator=(T value) {
        _value = value;
        if (_name == "F")
        {
            _value &= 0xF0;
        }
        else if (_name == "AF")
        {
            _value &= 0xFFF0;
        }
        return *this;
    }

    T& operator()() {
        return _value;
    }

    Register& operator+=(const T& r) {
        _value += r;
        return *this;
    }

    Register& operator-=(const T& r) {
        _value -= r;
        return *this;
    }

    Register& operator^=(const T& r) {
        _value ^= r;
        return *this;
    }

    Register& operator&=(const T& r) {
        _value &= r;
        return *this;
    }

    Register& operator|=(const T& r) {
        _value |= r;
        return *this;
    }

    Register& operator<<=(const T& r) {
        _value <<= r;
        return *this;
    }

    Register& operator>>=(const T& r) {
        _value >>= r;
        return *this;
    }

    Register& operator++() {
        _value++;
        return *this;
    }

    Register& operator--() {
        _value--;
        return *this;
    }

    Register& operator~() {
        _value = ~_value;
        return *this;
    }

private:
	T& _value;
};


class RegisterFile {
public:
	Register<uint8_t> A, F, B, C, D, E, H, L, SPmsb, SPlsb, PCmsb, PClsb; 
	Register<uint16_t> AF, BC, DE, HL, SP, PC;

    RegisterFile() :
        A("A", _data[1]),
        F("F", _data[0]),
        B("B", _data[3]),
        C("C", _data[2]),
        D("D", _data[5]),
        E("E", _data[4]),
        H("H", _data[7]),
        L("L", _data[6]),
        SPmsb("SPmsb", _data[9]),
        SPlsb("SPlsb", _data[8]),
        PCmsb("PCmsb", _data[11]),
        PClsb("PClsb", _data[10]),
        AF("AF", reinterpret_cast<uint16_t&>(_data[0])),
        BC("BC", reinterpret_cast<uint16_t&>(_data[2])),
        DE("DE", reinterpret_cast<uint16_t&>(_data[4])),
        HL("HL", reinterpret_cast<uint16_t&>(_data[6])),
        SP("SP", reinterpret_cast<uint16_t&>(_data[8])),
        PC("PC", reinterpret_cast<uint16_t&>(_data[10])),
        strTo8bit({
            { "A", &A },
            { "F", &F },
            { "B", &B },
            { "C", &C },
            { "D", &D },
            { "E", &E },
            { "H", &H },
            { "L", &L }
            }),
        strTo16bit({
            {"AF", &AF},
            {"BC", &BC},
            {"DE", &DE},
            {"HL", &HL},
            {"SP", &SP},
            {"PC", &PC}
            }) {};

    int ZF() { return (F >> 7) & 1; };
    int NF() { return (F >> 6) & 1; };
    int HF() { return (F >> 5) & 1; };
    int CF() { return (F >> 4) & 1; };

    void ZF(int n) { F = (F & ~(1 << 7)) | (n << 7); };
    void NF(int n) { F = (F & ~(1 << 6)) | (n << 6); };
    void HF(int n) { F = (F & ~(1 << 5)) | (n << 5); };
    void CF(int n) { F = (F & ~(1 << 4)) | (n << 4); };

    std::vector<Register<uint16_t>*>::iterator _begin = _registerVector.begin();
    std::vector<Register<uint16_t>*>::iterator _end = _registerVector.end();

    std::map<std::string, Register<uint8_t>*> strTo8bit;

    std::map<std::string, Register<uint16_t>*> strTo16bit;

private:
	uint8_t _data[12]{ 0 };
	std::vector<Register<uint16_t>*> _registerVector{ &AF, &BC, &DE, &HL, &SP, &PC };
};
#endif
