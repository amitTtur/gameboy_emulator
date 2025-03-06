#include "CartridgeView.h"

cartridgeView::cartridgeView(const uint8_t* romCpy) : _romCpy(romCpy)
{
    _type = getType();
    _romSize = getRomSize();
    _ramSize = getRamSize();
    _ramSupport = getRamSupport();
    printROMdetails();
}

cartridgeView::~cartridgeView()
{
    delete _romCpy;
}

bool cartridgeView::checkNintendoSymbol() const
{
	std::vector<uint8_t> symbol = {
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
        0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
        0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
        0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
    };

    for (size_t i = 0; i < symbol.size(); i++)
    {

    }

    return true;
}

void cartridgeView::printROMdetails() const
{
    int tmp = int(_type);
    std::cout << "this rom uses mbc" << tmp << std::endl;
    std::cout << "this rom holds " << _romSize << " banks of rom, which is " << _romSize*16 << "kb of memory." << std::endl;
    std::cout << "this rom holds " << _ramSize << " banks of ram, which is " << _ramSize * 8 << "kb of exteranal ram." << std::endl;
    std::cout << "this rom turned " << (_ramSupport ? "on" : "off") << " ram support on initialization" << std::endl;
}

MbcType cartridgeView::getType(const uint8_t* headersCopy)
{

    switch (headersCopy[cartridge_type])
    {
        // no mbc
    case 0x00:
    case 0x08:
    case 0x09:
        delete[] headersCopy;
        return MbcType::romOnly;

        // mbc 1
    case 0x01:
    case 0x02:
    case 0x03:
        delete[] headersCopy;
        return MbcType::mbc1;

        // mbc 2
    case 0x05:
    case 0x06:
        delete[] headersCopy;
        return MbcType::mbc2;

        // mbc 3
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
        delete[] headersCopy;
        return MbcType::mbc3;

        // mbc 4
    case 0x15:
    case 0x16:
    case 0x17:
        delete[] headersCopy;
        return MbcType::mbc4;

        // mbc 5
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
        delete[] headersCopy;
        return MbcType::mbc5;

        // those mbc are recognized but they hold a more advanced mbc type that the original gameboy does not support
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x20:
    case 0x22:
    case 0xFC:
    case 0xFD:
    case 0xFE:
        delete[] headersCopy;
        return MbcType::unknown;

    default:
        delete[] headersCopy;
        std::cout << "Cartridge view encounterd a unrecognized mbc type... please check the rom." << std::endl;
        return MbcType::unknown;
    }
}

MbcType cartridgeView::getType()
{
    _ramSupport = false;
    _timerSupport = false;
    switch (_romCpy[cartridge_type])
    {
    // no mbc
    case 0x00:
        return MbcType::romOnly;
    case 0x08:
    case 0x09:
        _ramSupport = true;
        return MbcType::romOnly;

    // mbc 1
    case 0x01:
        return MbcType::mbc1;
    case 0x02:
    case 0x03:
        _ramSupport = true;
        return MbcType::mbc1;
    
    // mbc 2
    case 0x05:
    case 0x06:
        return MbcType::mbc2;

    // mbc 3
    case 0x0F:
        _timerSupport = true;
        return MbcType::mbc3;
    case 0x10:
        _timerSupport = true;
        _ramSupport = true;
        return MbcType::mbc3;
    case 0x11:
        return MbcType::mbc3;
    case 0x12:
    case 0x13:
        _ramSupport = true;
        return MbcType::mbc3;

    // mbc 4
    case 0x15:
    case 0x16:
    case 0x17:
        return MbcType::mbc4;

    // mbc 5
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
        return MbcType::mbc5;

    // those mbc are recognized but they hold a more advanced mbc type that the original gameboy does not support
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x20:
    case 0x22:
    case 0xFC:
    case 0xFD:
    case 0xFE:
        return MbcType::unknown;

    default: 
        std::cout << "Cartridge view encounterd a unrecognized mbc type... please check the rom." << std::endl;
        return MbcType::unknown;
    }
}


RomSize cartridgeView::getRomSize()
{
    switch (_romCpy[rom_size])
    {
    case 0x00:
        return RomSize::KB32;
    case 0x01:
        return RomSize::KB64;
    case 0x02:
        return RomSize::KB128;
    case 0x03:
        return RomSize::KB256;
    case 0x04:
        return RomSize::KB512;
    case 0x05:
        return RomSize::MB1;
    case 0x06:
        return RomSize::MB2;
    case 0x07:
        return RomSize::MB4;
    case 0x08:
        return RomSize::MB8;
    default:
        std::cout << "Cartridge view encounterd a unrecognized rom size... please check the rom." << std::endl;
        return RomSize::KB32;
    }
}

RamSize cartridgeView::getRamSize()
{
    switch (_romCpy[ram_size]) {
    case 0x00:
        return RamSize::None;
     // case 0x01: // not used by this emulator
     //    return RamSize::KB2;
    case 0x02:
        return RamSize::ramKB8;
    case 0x03:
        return RamSize::ramKB32;
    case 0x04:
        return RamSize::ramKB128;
    case 0x05:
        return RamSize::ramKB64;
    default:
        std::cout << "Cartridge view encounterd a unrecognized ram size... please check the rom." << std::endl;
        return RamSize::None;
    }
}

bool cartridgeView::getRamSupport()
{
    int mode = _romCpy[cartridge_type];
    return mode == 2 || mode == 3 || mode == 8 || mode == 9 || mode == 0x10 || 
        mode == 0x12 || mode == 0x13 || mode == 0x1A || mode == 0x1B || mode == 0x1D ||
        mode == 0x1E;
}
