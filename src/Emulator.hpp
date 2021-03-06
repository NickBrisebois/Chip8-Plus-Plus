#ifndef EMULATOR_H
#define EMULATOR_H

#include <fstream>

class Emulator 
{
	private:
		// Chip8 has 35 opcodes of which all are 2 bytes long which is the size of a short
		unsigned short opcode;
		// Chip8 has 4K memory in total
		// Each opcode is split into two chars
		unsigned char memory[4096];
		// The chip 8 has 15 8-bit general purpose registers named VO, V1, ... VE
		unsigned char V[16];
		// The index register and the program counters can have values from 0x000 to 0xFFF
		unsigned short I;
		unsigned short pc;
		unsigned char delay_timer;
		unsigned char sound_timer;

		// The stack and stack pointer
		unsigned short stack[16];
		unsigned short sp;
		
		std::ifstream game;

		void handleOpcode();
		void clearScreen();
		void storeBCDVX();
		void storeRegisters( unsigned char b );
		void loadRegisters( unsigned char b );
		void debug();
		void draw();
	public:
		Emulator();
		void initialize();
		bool loadGame( std::string gamePath );
		void emulateCycle();
		void setKeys();
		bool canDraw();
		bool canBeep();
		struct debugInfo {
			unsigned short prevOpcode;
			unsigned short nextOpcode;
			unsigned char memory[4096];
			unsigned char V[16];
			unsigned char key[16];
			unsigned short I;
			unsigned short pc;
			unsigned short stack[16];
			unsigned short sp;
			bool drawFlag;
		};
		debugInfo getDebugInfo();
		// The graphics in the chip8 are black and white and the screen has a total of 2048 pixels
		unsigned char gfx[64 * 32];
		unsigned char key[16];
		// Can we draw?
		bool drawFlag;
		// Can we beep?
		bool beepFlag;
};

#endif
