#include <SFML/Graphics.hpp>

#include "Emulator.hpp"

void handleInput();

int const windowHeight = 640;
int const windowWidth = 320;

int main( int argc, char* argv[] ) 
{
	sf::RenderWindow window( sf::VideoMode( windowHeight, windowWidth ), "Chip 8 Emulator" );
	
	Emulator* pChip8 = new Emulator();
	pChip8->loadGame( "./breakout.ch8" );

	while( window.isOpen() ) {
		sf::Event event;

		pChip8->emulateCycle();

		while( window.pollEvent( event ) ) {
			if( event.type == sf::Event::Closed ){
				window.close();
			}
		}

		window.clear();
		window.display();
	}

	return 0;
}

void handleInput() 
{

}
