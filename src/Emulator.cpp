#include <iostream>
#include <fstream>
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
	// Beginning of memory holds the font set 
	for( int i = 0; i < 80; ++i ) {
		memory[i] = fontset[i];
	}
}

void Emulator::loadGame( std::string gamePath )
{
	// Load the game in binary read mode
	game.open( gamePath, std::ifstream::binary );

	if( !game.is_open() ) {
		throw std::runtime_error( "Game could not be loaded ");
	}

	// Get the data from the game by getting its internal filebuf object
	std::filebuf* pBuff = game.rdbuf();
	std::size_t size = pBuff->pubseekoff( 0, game.end, game.in );
	char* buffer = new char[size];
	pBuff->sgetn( buffer, size );
	game.close();

	for( int i = 0; i < size; ++i ){
		// Game memory is loaded in at address 0x200 (512 DEC)
		memory[ i + 512 ] = buffer[i];
	}

	std::cout << gamePath << " successfully loaded into memory" << std::endl;
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

void Emulator::handleOpcode( char opcode ) 
{
	switch( opcode & 0xF000 ) {
		case 0xA000:
			I = opcode & 0x0FFF;
			pc += 2;
			break;
	}
}

void Emulator::setIndexReg( int addr ) 
{

}


