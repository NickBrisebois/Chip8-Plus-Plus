#include <SFML/Graphics.hpp>
#include <sys/time.h>
#include <thread>
#include <iostream>

#include "../libs/imgui/imgui.h"
#include "../libs/imgui-sfml/imgui-SFML.h"
#include "Emulator.hpp"

#define DEBUG true

void handleInput( Emulator* pChip8, sf::Event* pEvent );
void redraw( sf::RenderWindow* pWindow);

int const windowHeight = 640;
int const windowWidth = 320;

unsigned char toDraw[64 * 32];

int main( int argc, char* argv[] ) 
{
	sf::RenderWindow window( sf::VideoMode( windowHeight, windowWidth ), "Chip 8 Emulator" );
	window.setKeyRepeatEnabled( false );

	if( DEBUG ) ImGui::SFML::Init( window );

	/**
	 * Let's turn on the Chip8
	 */
	Emulator chip8;
	chip8.initialize();
	
	Emulator::debugInfo debugInfo;

	if( !chip8.loadGame( argv[1] ) ) {
		return -1;
	}

	sf::Clock deltaClock;
	while( window.isOpen() ) {
		sf::Event event;
		chip8.emulateCycle();

		if( chip8.canDraw() ) {
			std::copy( std::begin( chip8.gfx ), std::end( chip8.gfx ), std::begin( toDraw ) );
		}

		while( window.pollEvent( event ) ) {
			if( DEBUG ) ImGui::SFML::ProcessEvent( event );

			if( event.type == sf::Event::Closed ){
				window.close();
			}

			handleInput( &chip8, &event );
		}
			
		redraw( &window );

		if( DEBUG ) {
			/**
			 * ImGUI Debug Window Code
			 */
			debugInfo = chip8.getDebugInfo();
			ImGui::SFML::Update( window, deltaClock.restart() );
			ImGui::Begin( "Debug" );
			
			ImGui::Text( "Opcode: %x", debugInfo.opcode );
			ImGui::Text( "Index Register: %x", debugInfo.I );
			ImGui::Text( "Program Counter: %x", debugInfo.pc );
			ImGui::Text( "Stack Pointer: %x", debugInfo.sp );
			ImGui::Text( "Draw Flag: %d", debugInfo.drawFlag );
			
			ImGui::End();
			ImGui::SFML::Render( window );
		}
		
		sf::sleep( sf::microseconds(600) );
		
		window.display();
		
	}

	return 0;
}

void handleInput( Emulator* pChip8, sf::Event* pEvent ) 
{
	int keyVal = ( pEvent->type == sf::Event::KeyPressed ) ? 1 : 0;

	if( pEvent->key.code == sf::Keyboard::Num1 ) pChip8->key[0x1] = keyVal;
	else if( pEvent->key.code == sf::Keyboard::Num2 ) pChip8->key[0x2] = keyVal;
	else if( pEvent->key.code == sf::Keyboard::Num3 ) pChip8->key[0x3] = keyVal;
	else if( pEvent->key.code == sf::Keyboard::Num4 ) pChip8->key[0xC] = keyVal;
	
	else if( pEvent->key.code == sf::Keyboard::Q ) pChip8->key[0x4] = keyVal;
	else if( pEvent->key.code ==  sf::Keyboard::W ) pChip8->key[0x5] = keyVal;
	else if( pEvent->key.code ==  sf::Keyboard::E ) pChip8->key[0x6] = keyVal;
	else if( pEvent->key.code ==  sf::Keyboard::R ) pChip8->key[0xD] = keyVal;

	else if( pEvent->key.code == sf::Keyboard::A ) pChip8->key[0x7] = keyVal;
	else if( pEvent->key.code ==  sf::Keyboard::S ) pChip8->key[0x8] = keyVal;
	else if( pEvent->key.code == sf::Keyboard::D ) pChip8->key[0x9] = keyVal;
	else if( pEvent->key.code == sf::Keyboard::F ) pChip8->key[0xE] = keyVal;

	else if( pEvent->key.code == sf::Keyboard::Z ) pChip8->key[0xA] = keyVal;
	else if( pEvent->key.code == sf::Keyboard::X ) pChip8->key[0x0] = keyVal;
	else if( pEvent->key.code == sf::Keyboard::C ) pChip8->key[0xB] = keyVal;
	else if( pEvent->key.code == sf::Keyboard::V ) pChip8->key[0xF] = keyVal;
}

void redraw( sf::RenderWindow* pWindow )
{
	pWindow->clear();
	for( int y = 0; y < 32; y++ ) {
		for( int x = 0; x < 64; x++ ) {
			if( toDraw[( y * 64 ) + x] == 1 ) {
				sf::RectangleShape pixel( sf::Vector2f( 10, 10 ) );
				pixel.setPosition( x*10, y*10 );
				pixel.setFillColor( sf::Color( 255, 255, 255, 255 ) );
				pWindow->draw(pixel);
			}
		}
	}
}



