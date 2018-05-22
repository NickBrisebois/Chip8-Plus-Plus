#include <SFML/Graphics.hpp>
#include <sys/time.h>
#include <thread>
#include <iostream>

#include "Emulator.hpp"

void handleInput( Emulator* pChip8, sf::RenderWindow* pWindow, sf::Event* pEvent );

int const windowHeight = 640;
int const windowWidth = 320;

int main( int argc, char* argv[] ) 
{
	sf::RenderWindow window( sf::VideoMode( windowHeight, windowWidth ), "Chip 8 Emulator" );
	window.setKeyRepeatEnabled( false );

	Emulator chip8;
	chip8.initialize();

	if( !chip8.loadGame( argv[1] ) ) {
		return -1;
	}

	while( window.isOpen() ) {
		sf::Event event;
		chip8.emulateCycle();

		while( window.pollEvent( event ) ) {
			if( event.type == sf::Event::Closed ){
				window.close();
			}
			handleInput( &chip8, &window, &event );
		}

		if( chip8.canDraw() ) {
			window.clear();
			for( int y = 0; y < 32; y++ ) {
				for( int x = 0; x < 64; x++ ) {
					if( chip8.gfx[( y * 64) + x] == 1 ) {
						sf::RectangleShape pixel( sf::Vector2f( 10, 10 ) );
						pixel.setPosition( x*10, y*10 );
						pixel.setFillColor( sf::Color( 255, 255, 255, 255 ) );
						window.draw(pixel);
					}
				}
			}
			std::this_thread::sleep_for( std::chrono::milliseconds( 60 ) );
		}
		window.display();
	}

	return 0;
}

void handleInput( Emulator* pChip8, sf::RenderWindow* pWindow, sf::Event* pEvent ) 
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
	
	else if( pEvent->type == sf::Event::KeyPressed ) {
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
