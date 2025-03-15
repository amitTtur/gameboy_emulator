#pragma once
#ifndef MBC3_H
#define MBC3_H

#include "MBC.h"

/*

memloc - 0x0 - 0x7FFF are read only, no write would affect those
writing to those address would affect the mbc registers

0000–1FFF — RAM Enable (when Writing Only)
2000–3FFF — ROM Bank Number (when Writing Only)
4000–5FFF — RAM Bank Number — or — Upper Bits of ROM Bank Number (when Writing Only)  || explanation in the registers
6000–7FFF — Banking Mode Select (when Writing Only)

*/


class MBC3 : public MBC
{
public:
	MBC3(uint8_t* memory, const std::string& romPath, const std::string& saveFolderPath, const bool saveFlag);

	virtual MBC* getMBC() override;

private:
	// timer needs to be added, per second it will increase by 1
	//globalvars -> clock is your timer

	// ramg is both for ram and rtc in this mbc
	// _bank1 - is the bank number
	// _bank2 - is the rtc ram selector
	uint8_t _rtcSeconds; // 0-59
	uint8_t _rtcMinutes; // 0-59
	uint8_t _rtcHours;   // 0-23
	uint8_t _rtcDaysLv; // the lower value of days  
	union {
		struct
		{
			bool _rtcDaysHv : 1; // high bit of the days register
			bool _none : 5; // means nothing
			bool _clockHalt : 1; // if the clock should tick or not
			bool _daysCarry : 1; // if there was a carry from days register
		};
		uint8_t _generalRtcReg;
	};

	char _latchSequence;

	// ranges
	const unsigned short LATCH_HIGH = 0x7FFF;
	const unsigned short LATCH_LOW = 0x6000;
	const unsigned short RAM_RTC_HIGH = 0x5FFF;
	const unsigned short RAM_RTC_LOW = 0x4000;
	const unsigned short ROM_HIGH = 0x3FFF;
	const unsigned short ROM_LOW = 0x2000;

	// functions
	virtual void updateBanks() override;
	virtual void bankSwitchUpdate(const uint16_t& address, const uint8_t& value) override;
	// here update the rtc registers based on the timer variable
	void updateRTCRegs();

	// special read write happend in ram range in mbc2
	virtual void writeToRam(const uint16_t& address, const uint8_t& value) override;
	virtual uint8_t readFromRam(const uint16_t& address) const override;

	
};




#endif // !MBC3_H