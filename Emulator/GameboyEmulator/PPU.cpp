#include "PPU.h"

PPU::PPU(Memory& _memRef, Screen& sdl) : _mem(_memRef), _statReg(_memRef), _lcdcReg(_memRef), _sdlScreen(sdl),
_ly(_mem[LY_LOC]), _lyc(_mem[LYC_LOC]), _scx(_mem[SCX_LOC]), _scy(_mem[SCY_LOC]), _wx(_mem[WX_LOC]), _wy(_mem[WY_LOC])
{
	logFile.open("logFile.txt", std::ios::out);
	resetFrameBuffer();
	_ly = 0x91;
	_lyc = 0;
	_scx = 0;
	_scy = 0;
	_wx = 0;
	_wy = 0;

}

PPU::~PPU() { logFile.close(); }



void PPU::update(uint8_t cycles)
{
	_allocatedCycles += cycles;

	switch (_statReg.getMode())
	{
	case 0: //hblank
		if (_allocatedCycles >= 204)
		{
			_allocatedCycles -= 204;

			drawBackground();
			drawWindow();
			drawSprites();

			_ly++;

			if (_statReg.mode0IEget())
			{
				_mem.int_LCDCStatus(1);
			}

			if (_ly == 144)
			{
				_statReg.setMode(1);
			}
			else
			{
				_statReg.setMode(2);
			}
		}
		break;
	case 1: //vblank
		if (_allocatedCycles >= 456)
		{
			_allocatedCycles -= 456;


			if (_ly == 144)
			{
				//globalVars::dma(true);
				_mem.int_Vblank(1);
				_sdlScreen.updateScreen(_frameBuffer);
				resetFrameBuffer();
			}


			if (_ly == 154)
			{
				_ly = 0;
				_statReg.setMode(2);
			}
			else
			{
				_ly++;
			}
		}
		break;
	case 2: //oam
		if (_allocatedCycles >= 80)
		{
			_allocatedCycles -= 80;
			_statReg.setMode(3);

			if (_statReg.mode2IEget())
			{
				_mem.int_LCDCStatus(1);
			}

			_statReg.setLineComperator(_ly == _lyc);
			if (_statReg.LYCIEget() && _statReg.getLineComparator())
			{
				_mem.int_LCDCStatus(1);
			}
		}
		break;
	case 3: //access vram
		if (_allocatedCycles >= 172)
		{
			_allocatedCycles -= 172;

			_statReg.setMode(0);
		}
		break;
	default:
		break;
	}
}

void PPU::resetFrameBuffer()
{
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	{
		_frameBuffer[i] = 0;
	}
}

uint8_t(&PPU::getFrameBuffer())[SCREEN_WIDTH * SCREEN_HEIGHT]
{
	return _frameBuffer;
}


void PPU::drawBackground() {

	if (!_lcdcReg.getBit(0)) { return; }

	//set up settings for rendering
	bool useTileSetZero = _lcdcReg.getBit(4);
	bool useTileMapZero = !_lcdcReg.getBit(3);

	//get the background pallete
	uint8_t bgPaletteValue = _mem[BG_PLATTE_LOC];

	//find out which tile map and tile set to use
	uint16_t tileSetAddress = useTileSetZero ? TILE_SET_1 : TILE_SET_2;
	uint16_t tileMapAddress = useTileMapZero ? TILE_MAP_1_BGWIN : TILE_MAP_2_BGWIN;

	//draw screen line
	for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {

		//find current location after scrolling
		uint8_t scrolledX = x + _scx;
		uint8_t scrolledY = _ly + _scy;

		//find location and specific pixel on tile map in memory, also find its address
		uint8_t tileX = scrolledX / 8;
		uint8_t tileY = scrolledY / 8;
		uint8_t tilePixelX = scrolledX % 8;
		uint8_t tilePixelY = scrolledY % 8;
		uint16_t tileIndex = (tileY % 32) * 32 + (tileX % 32);
		uint16_t tileIdAddress = tileMapAddress + tileIndex;

		//get the tile itself from memory, doing the required calculations on it
		uint8_t tileId = _mem[tileIdAddress];
		int16_t tileDataMemOffset = useTileSetZero ? tileId * 16 : (static_cast<int8_t>(tileId) + 128) * 16;
		uint16_t tileDataLineOffset = tilePixelY * 2;
		uint16_t tileLineDataStartAddress = tileSetAddress + tileDataMemOffset + tileDataLineOffset;

		//get the actual pixels
		uint8_t pixels1 = _mem[tileLineDataStartAddress];
		uint8_t pixels2 = _mem[tileLineDataStartAddress + 1];

		//find out the required pixels color, and map it to the current pallete
		uint8_t colorBit = 7 - tilePixelX;
		uint8_t colorValue = ((pixels2 >> colorBit) & 0x01) << 1 | ((pixels1 >> colorBit) & 0x01);
		uint8_t paletteColor = (bgPaletteValue >> (colorValue * 2)) & 0x03;

		//print pixel to screen
		_frameBuffer[_ly * SCREEN_WIDTH + x] = paletteColor;
	}

}



void PPU::drawWindow()
{
	if (!_lcdcReg.getBit(7) || !_lcdcReg.getBit(5)) { return; }

	if (_ly < _wy) { return; }

	//set up settings for rendering
	bool useTileSetZero = _lcdcReg.getBit(4);
	bool useTileMapZero = !_lcdcReg.getBit(6);

	//get the background pallete
	uint8_t bgPaletteValue = _mem[BG_PLATTE_LOC];

	//find the tile map and tileset to use
	uint16_t tileSetAddress = useTileSetZero ? TILE_SET_1 : TILE_SET_2;
	uint16_t tileMapAddress = useTileMapZero ? TILE_MAP_1_BGWIN : TILE_MAP_2_BGWIN;

	//get the current location offset to the window to stay with location relative to the window
	uint8_t windowY = _ly - _wy;

	//get the tiles location on the map, and the y value of the current line
	uint8_t tileY = windowY / 8;
	uint8_t tilePixelY = windowY % 8;

	//draw line
	for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {

		//if pixel is outside of window, skip it
		if (x + 7 < _wx) { continue; }

		//find out current tiles x coordinate
		uint8_t windowX = x + 7 - _wx;
		uint8_t tileX = windowX / 8;
		uint8_t tilePixelX = windowX % 8;

		//get the tiles address in map
		uint16_t tileIndex = tileY * 32 + tileX;
		uint16_t tileIdAddress = tileMapAddress + tileIndex;

		//if tile is out of bounds, skip it
		if (tileIndex >= 1024) { continue; }

		//get the tiles location in memory, doing the required calculations on it
		uint8_t tileId = _mem[tileIdAddress];
		uint16_t tileDataMemOffset = useTileSetZero ? tileId * 16 : (static_cast<int8_t>(tileId) + 128) * 16;
		uint16_t tileDataLineOffset = tilePixelY * 2;
		uint16_t tileLineDataStartAddress = tileSetAddress + tileDataMemOffset + tileDataLineOffset;

		//get the actual tile pixels from memory
		uint8_t pixels1 = _mem[tileLineDataStartAddress];
		uint8_t pixels2 = _mem[tileLineDataStartAddress + 1];

		//getting the pixels color and mapping it to the pallete
		uint8_t colorBit = 7 - tilePixelX;
		uint8_t colorValue = ((pixels2 >> colorBit) & 0x01) << 1 | ((pixels1 >> colorBit) & 0x01);
		uint8_t paletteColor = (bgPaletteValue >> (colorValue * 2)) & 0x03;

		//drawing the pixel on screen
		_frameBuffer[_ly * SCREEN_WIDTH + x] = paletteColor;
	}
}

void PPU::drawSprites()
{
	// Check if sprites are enabled in LCDC register
	if (!_lcdcReg.getBit(1)) { return; }

	// Get sprite palettes
	uint8_t spritePalette0 = _mem[SP_PLATTE0_LOC];
	uint8_t spritePalette1 = _mem[SP_PLATTE1_LOC];
	 
	std::vector<spriteSave> spriteVector;
	performOamSearch(spriteVector); // gets the sprites on line sorted by the lowest x to highest xpos
	writeCurrentStateToLogFile(spriteVector);

	for (char i = spriteVector.size() - 1; i >= 0; i--)
	{
		spriteSave tmp = spriteVector[i];
		uint8_t firstByte = _mem[tmp.tileAddress];
		uint8_t secondByte = _mem[tmp.tileAddress+1];
		for (int j = 0; j < 8; j++)
		{
			// getting pallate index
			char pixel = (((secondByte >> j) & 1) << 1) | ((firstByte >> j) & 1);
			pixel = ((tmp.pallate ? spritePalette1 : spritePalette0) >> (pixel * 2)) & 0x3;; // pallte color

			if (pixel && !tmp.priority) // if color not 0 (if not transparent), and sprite on top of bg and window
			{
				// int pixelPos = (_ly * SCREEN_WIDTH) + tmp.xPos + (tmp.xFlip ? 7 - j : j);
				_frameBuffer[(_ly * SCREEN_WIDTH) + tmp.xPos + (tmp.xFlip ? j : (7 - j))] = pixel;
			}
		}
	}
}

void PPU::performOamSearch(std::vector<spriteSave>& spriteVector)
{
	spriteVector.clear();
	bool heightFlag = _lcdcReg.getBit(2); // if the sprites are 8*8 (false) ,or 8*16 (true)
	uint8_t spritesOnScanLine = 0;
	for (int i = 0; i < 40; i++)
	{
		// can read directily from mem because ,bc doesnt matter for those calculations
		volatile short spriteActualYCord = _mem[0xFE00 + i * 4] - 16; // int cause it can be a signed value

		// if a sprite is on the current scan line and the limit didnt extant to 10
		if (spriteActualYCord <= _ly && (spriteActualYCord + (heightFlag ? 16 : 8)) > _ly)
		{
			spritesOnScanLine++;
			// saving the sprite
			spriteSave tmp;

			uint8_t xposNonacc = _mem[0xFE00 + i * 4 + 1]; // xpos -8 so we would start from his first bit
			// if sprite his hidden dont add to the vector
			// this is purely to increase performance and isnt what the actual gb did
			if (xposNonacc == 0 || xposNonacc >= 168)
			{
				continue; // no need to save it because its hidden
			}
			tmp.xPos = xposNonacc - 8;


			uint8_t spriteAtrributes = _mem[0xFE00 + i * 4 + 3];
			tmp.xFlip = (spriteAtrributes & 0b00100000) != 0; // fifth bit
			tmp.priority = (spriteAtrributes & 0b10000000) != 0; // seventh bit
			tmp.pallate = (spriteAtrributes & 0b00010000) != 0; // fourth bit

			uint8_t tileRow;
			// tile row
			// y flip is only used here so no need to save
			if (spriteAtrributes & 0b01000000) // the calc is for the y flip flag
			{
				tileRow = ((heightFlag ? 15 : 7) - (_ly - spriteActualYCord));
			}
			else
			{
				tileRow = _ly - spriteActualYCord;
			}
			tmp.heightMode = heightFlag;
			// offest + (((tileIndex & 1 or 0)+tile row) * (individual tile length))
			// if heightFlag the tile index needs to be even so outof bounds wount happened
			uint8_t tileIndex = _mem[0xFE00 + i * 4 + 2];
			if (tmp.heightMode)
			{
				tileIndex &= 0xFE;
			}
			tmp.tileAddress = 0x8000 + (tileIndex * 16) + tileRow * 2;


			// addint to scanline vector
			spriteVector.push_back(tmp);
		}

		if (spritesOnScanLine == 10) // limit reached stopping to save cpu power
		{
			break;
		}
	}
	// now we will sort the vector from the highest xpos to lowest (index 0)
	std::sort(spriteVector.begin(), spriteVector.end(), [](const spriteSave& a, const spriteSave& b)
	{
		return a.xPos < b.xPos; // Sort in ascending order (lowest xPos first)
	});
}

void PPU::writeCurrentStateToLogFile(std::vector<spriteSave> vector)
{
	for (auto i = vector.begin(); i != vector.end(); i++)
	{
		logFile <<
			"line: " << int(_ly) << "    " <<
			"tile address: " << i->tileAddress << "    " <<
			"x pos: " << i->xPos << std::endl;
	}
}
