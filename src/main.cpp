#include <SFML/Graphics.hpp>
#include <thread>

#include "Emulator.hpp"

void handleInput();

int const windowHeight = 640;
int const windowWidth = 320;

int main( int argc, char* argv[] ) 
{
	sf::RenderWindow window( sf::VideoMode( windowHeight, windowWidth ), "Chip 8 Emulator" );
	
	Emulator* pChip8 = new Emulator();
	if( !pChip8->loadGame( "./breakout.ch8" ) ) {
		return -1;
	}


	while( window.isOpen() ) {
		sf::Event event;

		pChip8->emulateCycle();

		while( window.pollEvent( event ) ) {
			if( event.type == sf::Event::Closed ){
				window.close();
			}
		}

		// For debug
		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

		window.clear();
		window.display();
	}

	return 0;
}

void handleInput() 
{

}
