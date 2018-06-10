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
void windowCycle( sf::RenderWindow* pWindow, Emulator::debugInfo* pDebugInfo, Emulator* pChip8 );

int const windowHeight = 640;
int const windowWidth = 320;

unsigned char toDraw[64 * 32];
sf::Clock deltaClock;
sf::Event event;

int main( int argc, char* argv[] ) 
{
	sf::RenderWindow window( sf::VideoMode( windowHeight, windowWidth ), "Chip 8 Emulator" );
	window.setFramerateLimit(60);

	if( DEBUG ) ImGui::SFML::Init( window );

	/**
	 * Let's turn on the Chip8
	 */
	Emulator chip8;
	chip8.initialize();
	
	Emulator::debugInfo* pDebugInfo;

	if( !chip8.loadGame( argv[1] ) ) {
		return -1;
	}
	
	while( window.isOpen() ) {
		windowCycle( &window, pDebugInfo, &chip8 );
	}

	return 0;
}

void windowCycle( sf::RenderWindow* pWindow, Emulator::debugInfo* pDebugInfo, Emulator* pChip8 ) {
	if( DEBUG ) pDebugInfo = pChip8->getDebugInfo();

	while( pWindow->pollEvent( event ) ) {
		if( DEBUG ) ImGui::SFML::ProcessEvent( event );

		if( event.type == sf::Event::Closed ){
			pWindow->close();
		}

		handleInput( pChip8, &event );
	}

	pChip8->emulateCycle();

	if( pChip8->canDraw() ) {
		std::copy( std::begin( pChip8->gfx ), std::end( pChip8->gfx ), std::begin( toDraw ) );
	}
		
	redraw( pWindow );

	if( DEBUG ) {
		/**
		 * ImGUI Debug Window Code
		 */
		ImGui::SFML::Update( *pWindow, deltaClock.restart() );
		ImGui::Begin( "Debug" );
		
		ImGui::Text( "Opcode: %x", pDebugInfo->opcode );
		ImGui::Text( "Index Register: %x", pDebugInfo->I );
		ImGui::Text( "Program Counter: %x", pDebugInfo->pc );
		ImGui::Text( "Stack Pointer: %x", pDebugInfo->sp );
		ImGui::Text( "Draw Flag: %d", pDebugInfo->drawFlag );

		ImGui::Button( "Stop" );
		ImGui::Button( "Start" );
		ImGui::Button( "Step" );
		
		ImGui::End();
		ImGui::SFML::Render( *pWindow );
	}
	
	pWindow->display();
}

/**
 * There has to be a better way to do the below
 */
void handleInput( Emulator* pChip8, sf::Event* pEvent ) 
{
	if( pEvent->type == sf::Event::KeyPressed ) {
		if( pEvent->key.code == sf::Keyboard::Num1 ) pChip8->key[0x1] = 1;
		else if( pEvent->key.code == sf::Keyboard::Num2 ) pChip8->key[0x2] = 1;
		else if( pEvent->key.code == sf::Keyboard::Num3 ) pChip8->key[0x3] = 1;
		else if( pEvent->key.code == sf::Keyboard::Num4 ) pChip8->key[0xC] = 1;
		
		else if( pEvent->key.code == sf::Keyboard::Q ) pChip8->key[0x4] = 1;
		else if( pEvent->key.code ==  sf::Keyboard::W ) pChip8->key[0x5] = 1;
		else if( pEvent->key.code ==  sf::Keyboard::E ) pChip8->key[0x6] = 1;
		else if( pEvent->key.code ==  sf::Keyboard::R ) pChip8->key[0xD] = 1;

		else if( pEvent->key.code == sf::Keyboard::A ) pChip8->key[0x7] = 1;
		else if( pEvent->key.code ==  sf::Keyboard::S ) pChip8->key[0x8] = 1;
		else if( pEvent->key.code == sf::Keyboard::D ) pChip8->key[0x9] = 1;
		else if( pEvent->key.code == sf::Keyboard::F ) pChip8->key[0xE] = 1;

		else if( pEvent->key.code == sf::Keyboard::Z ) pChip8->key[0xA] = 1;
		else if( pEvent->key.code == sf::Keyboard::X ) pChip8->key[0x0] = 1;
		else if( pEvent->key.code == sf::Keyboard::C ) pChip8->key[0xB] = 1;
		else if( pEvent->key.code == sf::Keyboard::V ) pChip8->key[0xF] = 1;
	}
	
	else if( pEvent->type == sf::Event::KeyReleased ) {
		if( pEvent->key.code == sf::Keyboard::Num1 ) pChip8->key[0x1] = 0;
		else if( pEvent->key.code == sf::Keyboard::Num2 ) pChip8->key[0x2] = 0;
		else if( pEvent->key.code == sf::Keyboard::Num3 ) pChip8->key[0x3] = 0;
		else if( pEvent->key.code == sf::Keyboard::Num4 ) pChip8->key[0xC] = 0;
		
		else if( pEvent->key.code == sf::Keyboard::Q ) pChip8->key[0x4] = 0;
		else if( pEvent->key.code ==  sf::Keyboard::W ) pChip8->key[0x5] = 0;
		else if( pEvent->key.code ==  sf::Keyboard::E ) pChip8->key[0x6] = 0;
		else if( pEvent->key.code ==  sf::Keyboard::R ) pChip8->key[0xD] = 0;

		else if( pEvent->key.code == sf::Keyboard::A ) pChip8->key[0x7] = 0;
		else if( pEvent->key.code ==  sf::Keyboard::S ) pChip8->key[0x8] = 0;
		else if( pEvent->key.code == sf::Keyboard::D ) pChip8->key[0x9] = 0;
		else if( pEvent->key.code == sf::Keyboard::F ) pChip8->key[0xE] = 0;

		else if( pEvent->key.code == sf::Keyboard::Z ) pChip8->key[0xA] = 0;
		else if( pEvent->key.code == sf::Keyboard::X ) pChip8->key[0x0] = 0;
		else if( pEvent->key.code == sf::Keyboard::C ) pChip8->key[0xB] = 0;
		else if( pEvent->key.code == sf::Keyboard::V ) pChip8->key[0xF] = 0;
	}

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



