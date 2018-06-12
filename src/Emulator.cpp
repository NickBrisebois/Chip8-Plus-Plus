#include <iostream>
#include <cstring>
#include "Emulator.hpp"

Emulator::Emulator()
{
	std::cout << "Emulator has started" << std::endl;
	srand( time(NULL) );
}

void Emulator::initialize()
{ 
	pc = 0x200; // Program counter starts at 0x200 
	opcode = 0; // Reset current opcode
	I = 0; // Reset index register
	sp = 0; // Reset stack pointer
	sound_timer = 0;
	delay_timer = 0;
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

	// Clear registers
	for( int i = 0; i < 16; ++i ) {
		V[i] = 0;
	}

	for( int i = 0; i < 16; ++i ) {
		key[i] = 0;
	}

	// Beginning of memory holds the font set 
	for( int i = 0; i < 80; ++i ) {
		memory[i] = fontset[i];
	}
	
	drawFlag = true;
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
	game.seekg( 0, std::ios::beg );
	game.read( ( char* )buffer, size );
	game.close();

	for( int i = 0; i < size; ++i ){
		// Game memory is loaded in at address 0x200 (512 DEC)
		memory[i + 512] = buffer[i];
	}
	
	delete [] buffer;
	
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

void Emulator::handleOpcode( unsigned short opcode ) 
{
	// Opcode represented as ABCD
	unsigned short b = ( opcode & 0x0F00 ) >> 8;
	unsigned short c = ( opcode & 0x00F0 ) >> 4;
	unsigned short d = ( opcode & 0x000F );
	unsigned short bcd = ( opcode & 0x0FFF );
	unsigned short cd = ( opcode & 0x00FF );

	switch( opcode & 0xF000 ) {
		case 0x0000: // Opcodes [0x00E & 0x00EE]
			switch( d ) {
				case 0x0000: // CLS
					clearScreen();
					pc += 2;
					break;
				case 0x000E: // RET
					--sp;
					pc = stack[sp];
					break;
			}
			break;
		case 0x1000:
			pc = bcd;
			break;
		case 0x2000: // RET
			stack[sp] = pc;
			sp++;
			pc = bcd;
			break;
		case 0x3000: // SE Vx, byte
			if( V[b] == cd ) {
				pc += 2;
			}
			pc += 2;
			break;
		case 0x4000: // SNE Vx, byte
			if( V[b] != cd ) {
				pc += 2;
			}
			pc += 2;
			break;
		case 0x5000: // SE Vx, Vy
			if( V[b] == V[c] ) {
				pc += 2;
			}
			pc += 2;
			break;
		case 0x6000: // LD Vx, byte
			V[b] = cd;
			pc += 2;
			break;
		case 0x7000: // ADD Vx, byte
			V[b] += cd;
			pc += 2;
			break;
		case 0x8000:
			switch ( opcode & 0x000F ) {
				case 0x0000: // LD Vx, Vy
					V[b] = V[c];
					pc += 2;
					break;
				case 0x0001: // OR Vx, Vy
					V[b] |= V[c];
					pc += 2;
					break;
				case 0x0002: // AND Vx, Vy
					V[b] &= V[c];
					pc += 2;
					break;
				case 0x0003: // XOR Vx, Vy
					V[b] ^= V[c];
					pc += 2;
					break;
				case 0x0004: // ADD Vx, Vy
					V[0xF] = ( ( V[b] + V[c] ) > 0xFF ) ? 1 : 0;
					V[b] += V[c];
					pc += 2;
					break;
				case 0x0005: // SUB Vx, Vy
					V[0xF] = ( V[c] > V[b] ) ? 0 : 1;
					V[b] -= V[c];
					pc += 2;
					break;
				case 0x0006: // SHR Vx {, Vy}
					V[0xF] = V[b] & 0x1;
					V[b] >>= 1;
					pc += 2;
					break;
				case 0x0007: // SUBN Vx, Vy
					if( V[b] > V[c] ) {
						V[0xF] = 0;
					} else {
						V[0xF] = 1;
					}
					V[b] = V[c] - V[b];
					pc += 2;
					break;
				case 0x000E: // SHL Vx {, Vy}
					V[0xf] = V[b] >> 7;
					V[b] <<= 1;
					pc += 2;
					break;
			}
			break;
		case 0x9000: // SNE Vx, Vy
			if( V[b] != V[c] ) {
				pc += 2;
			}
			pc += 2;
			break;
		case 0xA000: // LD I, addr
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		case 0xB000: // JP V0, addr
			pc = bcd + V[0];
			break;
		case 0xC000: // RND Vx, byte
			V[b] = ( rand() % 256 ) & cd;
			pc += 2;
			break;
		case 0xD000: // DRW Vx, Vy, nibble
			draw( opcode );
			pc += 2;
			break;
		case 0xE000:
			switch( opcode & 0x00FF ) {
				case 0x009E: // Ex9E:
					if( key[V[b]] != 0 )
						pc += 2;
					break;
				case 0x00A1:
					if( key[V[b]] == 0 )
						pc += 2;
					break;
			}
			break;
		case 0xF000:
			switch( opcode & 0x00FF ) {
				case 0x0007: // LD Vx, DT
					V[b] = delay_timer;
					pc += 2;
					break;
				case 0x000A:
					{
						bool keyPress = false;

						for( int i = 0; i < 16; ++i ) {
							if( key[i] != 0 ) {
								V[( opcode & 0x0F00 ) >> 8] = i;
								keyPress = true;
							}
						}

						if( !keyPress )
							return;

						pc += 2;
					}
					break;
				case 0x0015: // LD DT, Vx
					delay_timer = V[b];
					pc += 2;
					break;
				case 0x0018: // LD ST, Vx
					sound_timer = V[b];
					pc += 2;
					break;
				case 0x001E: // ADD I, Vx
					V[0xF] = ( I + V[b] > 0xFFF ) ? 1 : 0;
					I += V[b];
					pc += 2;
					break;
				case 0x0029: // LD F, Vx
					I = V[b] * 0x5; 
					pc += 2;
					break;
				case 0x0033: // LD B, Vx
					storeBCDVX( opcode );
					pc += 2;
					break;
				case 0x0055: // LD [I], Vx
					storeRegisters( b );
					I = b + 1;
					pc += 2;
					break;
				case 0x0065: // LD Vx, [I]
					loadRegisters( b );
					I = b + 1;
					pc += 2;
					break;
			}
			break;
	}
}

void Emulator::clearScreen()
{
	for( int i = 0; i < 2048; ++i ){
		gfx[i] = 0;
	}
}

void Emulator::storeBCDVX( unsigned short opcode )
{
	memory[I] = V[( opcode & 0x0F00 ) >> 8] / 100;
	memory[I + 1] = ( V[( opcode & 0x0F00 ) >> 8] / 10 ) % 10;
	memory[I + 2] = ( V[( opcode & 0x0F00 ) >> 8] % 100 ) % 10;
}

void Emulator::draw( unsigned short opcode )
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

void Emulator::storeRegisters( unsigned short b )
{
	for( int i = 0; i <= b; ++i ) {
		memory[I + i] = V[i];
	}
}

void Emulator::loadRegisters( unsigned short b )
{
	for( int i = 0; i <= b; ++i ) {
		V[i] = memory[I + i];
	}
}

bool Emulator::canDraw()
{
	if( drawFlag ) {
		drawFlag = false;
		return true;
	}
	return false;
}

Emulator::debugInfo Emulator::getDebugInfo()
{
	debugInfo info;
	info.prevOpcode = opcode;
	info.nextOpcode = memory[pc] << 8 | memory[pc + 1];
	memcpy( info.memory, memory, sizeof( memory ) );
	memcpy( info.V, V, sizeof( V ) );
	memcpy( info.key, key, sizeof( key ) );
	info.I = I;
	info.pc = pc;
	memcpy( info.stack, stack, sizeof( stack ) );
	info.sp = sp;
	info.drawFlag = drawFlag;
	return info;
}
