#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sys/time.h>
#include <iostream>

#include "../libs/imgui/imgui.h"
#include "../libs/imgui-sfml/imgui-SFML.h"
#include "Emulator.hpp"

#define DEBUG true

void handleKeyDown( Emulator* pChip8, sf::Event* pEvent );
void handleKeyUp( Emulator* pChip8, sf::Event* pEvent );
void redraw( sf::RenderWindow* pWindow);
void windowCycle( sf::RenderWindow* pWindow, Emulator* pChip8, sf::Sound* pBeepNoise );
void debugWindow( sf::RenderWindow* pWindow, Emulator::debugInfo* pDebugInfo, bool* pGameToggled, Emulator* pChip8, sf::Clock* pDeltaClock, sf::Sound* pBeepNoise );

int const windowHeight = 640;
int const windowWidth = 320;

unsigned char toDraw[64 * 32];
sf::Clock deltaClock;
sf::Event event;

int main( int argc, char* argv[] ) 
{
	sf::RenderWindow window( sf::VideoMode( windowHeight, windowWidth ), "Chip 8 Emulator" );
	window.setFramerateLimit(60);

	sf::SoundBuffer soundBuffer;
	sf::Sound beepNoise;
	if( !soundBuffer.loadFromFile( "./sound/beep-02.wav" ) ) {
		return -1;
	}		
	beepNoise.setBuffer( soundBuffer );

	// If debug enabled, initialize the debug window
	if( DEBUG ) ImGui::SFML::Init( window );

	/**
	 * Let's turn on the Chip8
	 */
	Emulator chip8;
	chip8.initialize();
	
	if( !chip8.loadGame( argv[1] ) ) {
		return -1;
	}
	
	Emulator::debugInfo debugInfo;
	bool gameToggled = true;
	
	while( window.isOpen() ) {
		while( window.pollEvent( event ) ) {
			if( DEBUG ) ImGui::SFML::ProcessEvent( event );

			if( event.type == sf::Event::Closed ){
				window.close();
			}
			
			if( gameToggled ) {
				handleKeyDown( &chip8, &event );
				handleKeyUp( &chip8, &event );
			}
		}

		if( gameToggled ) 
			windowCycle( &window, &chip8, &beepNoise );
		
		if( DEBUG ) {
			debugInfo = chip8.getDebugInfo();
			debugWindow( &window, &debugInfo, &gameToggled, &chip8, &deltaClock, &beepNoise );
		}
		window.display();
	}

	return 0;
}

void windowCycle( sf::RenderWindow* pWindow, Emulator* pChip8, sf::Sound* pBeepNoise ) {
	pChip8->emulateCycle();

	if( pChip8->canDraw() ) {
		std::copy( std::begin( pChip8->gfx ), std::end( pChip8->gfx ), std::begin( toDraw ) );
	}
	
	if( pChip8->canBeep() ) {
		pBeepNoise->play();
	}
	
	redraw( pWindow );
}

/**
 * There has to be a better way to do the below
 */
void handleKeyDown( Emulator* pChip8, sf::Event* pEvent ) 
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

}

void handleKeyUp( Emulator* pChip8, sf::Event* pEvent ) 
{
	if( pEvent->type == sf::Event::KeyReleased ) {
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

void debugWindow( sf::RenderWindow* pWindow, Emulator::debugInfo* pDebugInfo, bool* pGameToggled, Emulator* pChip8, sf::Clock* pDeltaClock, sf::Sound* pBeepNoise )
{
	/**
	 * ImGUI Debug Window Code
	 */
	ImGui::SFML::Update( *pWindow, pDeltaClock->restart() );
	ImGui::Begin( "Debug" );
	
	ImGui::Text( "Prev Opcode: %x", pDebugInfo->prevOpcode );
	ImGui::Text( "Next Opcode: %x", pDebugInfo->nextOpcode );
	ImGui::Text( "Index Reg: %x", pDebugInfo->I );
	ImGui::Text( "Prog Counter: %x", pDebugInfo->pc );
	ImGui::Text( "Stack Pointer: %x", pDebugInfo->sp );
	ImGui::Text( "Draw Flag: %d", pDebugInfo->drawFlag );
	if( ImGui::Button( "Play/Pause" ) )
		*pGameToggled = !*pGameToggled;
	if( ImGui::Button( "Step" ) )
		windowCycle( pWindow, pChip8, pBeepNoise );
	ImGui::End();

	ImGui::Begin( "Stack" );
	for( int i = 0; i < 16; i++ ) {
		ImGui::Text( "%x: %x", i, pDebugInfo->stack[i] );
	}
	ImGui::End();

	ImGui::Begin( "Registers" );
	for( int i = 0; i < 16; i++ ) {
		ImGui::Text( "%x: %x", i, pDebugInfo->V[i] );
	}
	ImGui::End();

	ImGui::Begin( "Pressed Keys" );
	for( int i = 0; i < 16; i++ ) {
		ImGui::Text( "%x: %d", i, pDebugInfo->key[i] );
	}
	ImGui::End();

	ImGui::SFML::Render( *pWindow );
}

