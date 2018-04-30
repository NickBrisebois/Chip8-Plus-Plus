#include <iostream>
#include <fstream>
#include <iomanip>
#include "Emulator.hpp"

Emulator::Emulator()
{
	std::cout << "Emulator has started" << std::endl;
}

void Emulator::initialize()
{
	pc = 0x200; // Program counter starts at 0x200
	opcode = 0; // Reset current opcode
	I = 0; // Reset index register
	sp = 0; // Reset stack pointer
	unsigned char fontset[80] = {
		0xf0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	clearScreen();

	// Clear stack
	for( int i = 0; i < 16; ++i ) {
		stack[i] = 0;
	}

	// Clear memory
	for( int i = 0; i < 4096; ++i ) {
		memory[i] = 0;
	}

	// Beginning of memory holds the font set 
	for( int i = 0; i < 80; ++i ) {
		memory[i] = fontset[i];
	}

}

bool Emulator::loadGame( std::string gamePath )
{
	// Load the game in binary read mode
	game.open( gamePath, std::ifstream::ate );

	if( !game.is_open() ) {
		std::cerr << "Game could not be loaded" << std::endl;
		return false;
	}

	std::ifstream::pos_type size = game.tellg();
	unsigned char* buffer = new unsigned char[size];
	game.seekg( 0, std::ifstream::beg );
	game.read( ( char* )buffer, size );
	game.close();

	for( int i = 0; i < size; ++i ){
		// Game memory is loaded in at address 0x200 (512 DEC)
		memory[i + 512] = buffer[i];
	}

	std::cout << gamePath << " successfully loaded into memory" << std::endl;
	return true;
}

void Emulator::emulateCycle()
{
	// One opcode is stored at pc + pc+1 so combine them by doing:
	opcode = memory[pc] << 8 | memory[pc + 1];
	handleOpcode( opcode );

	if( delay_timer > 0 ) {
		delay_timer--;
	}

	if( sound_timer > 0 ) {
		if( sound_timer == 1 ) {
			std::cout << "Beep" << std::endl;
		}
		sound_timer--;
	}
}

void Emulator::handleOpcode( short opcode ) 
{
	std::cout << std::hex << (opcode & 0xF000) << std::endl;
	switch( opcode & 0xF000 ) {
		case 0x0000: // Opcodes [0x00E && 0x00EE]
			switch( opcode & 0x000F ) {
				case 0x0000: // [0x00E0] Clears the screen
					clearScreen();
					break;
				case 0x000E: // [0x00EE] Returns from subroutine
					// RTS
					break;
				default:
					std::cout << "Unknown opcode [0x0000]: " << opcode << std::endl;
			}
		case 0xA000: // Opcode [0xANNN]
			setIndexReg( opcode & 0x0FFF );
			pc += 2;
			break;
		case 0x2000: // Opcode [0x2NNN]
			stack[sp] = pc;
			++sp;
			pc = opcode & 0x0FFF;
			break;
		case 0x0004: // Opcode [0x8XY4]
			addVYToVX( opcode );
			pc += 2;
			break;
		case 0x0033:
			storeBCDVX( opcode );
			pc += 2;
			break;
		case 0xD000:
			draw( opcode );
			pc += 2;
	}
}

void Emulator::setIndexReg( short addr ) 
{
	std::cout << "[i] " << addr << std::endl;
	I = addr;
}


void Emulator::clearScreen()
{
	std::cout << "Screen Cleared" << std::endl;
	for( int i = 0; i < 2048; ++i ){
		gfx[i] = 0;
	}
}

void Emulator::addVYToVX( short opcode )
{
	if( V[( opcode & 0x00F0 ) >> 4] > ( 0xFF - V[( opcode & 0x0F00 ) >> 8] ) ) {
		V[0xF] = 1;
	}else {
		V[0xF] = 0;
	}
	V[( opcode & 0x0F00 ) >> 8] += V[( opcode & 0x00F0 ) >> 4];
	std::cout << "V[" << ( ( opcode & 0x0F00 ) >> 8 ) << "]" << V[( ( opcode & 0x0F00 ) >> 8 )] << std::endl;
}

void Emulator::storeBCDVX( short opcode )
{
	memory[I] = V[( opcode & 0x0F00 ) >> 8] / 100;
	memory[I + 1] = ( V[( opcode & 0x0F00 ) >> 8] / 10 ) % 10;
	memory[I + 2] = ( V[( opcode & 0x0F00 ) >> 8] % 100 ) % 10;
}

void Emulator::draw( short opcode )
{
	unsigned short x = V[( opcode & 0x0F00 ) >> 8];
	unsigned short y = V[( opcode & 0x00F0 ) >> 4];
	unsigned short height = opcode & 0x000F;
	unsigned short pixel;

	V[0xF] = 0;
	for( int yline = 0; yline < height; yline++ ) {
		pixel = memory[I + yline];
		for( int xline = 0; xline < 8; xline++ ) {
			if( ( pixel & ( 0x80 >> xline ) ) != 0 ) {
				if( gfx[ ( x + xline + ( ( y + yline ) * 64 ) ) ] == 1 ) {
					V[0xF] = 1;	
				}
				gfx[x + xline + ( ( y + yline ) * 64 )] ^= 1;
			}
		}
	}

	drawFlag = true;
}
