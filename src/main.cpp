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
	pChip8->initialize();
	if( !pChip8->loadGame( argv[1] ) ) {
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

		if( pChip8->drawFlag ) {
			pChip8->drawFlag = false;
			window.clear();
			for( int y = 0; y < 32; y++ ) {
				for( int x = 0; x < 64; x++ ) {
					if( pChip8->gfx[( y * 64) + x] == 1 ) {
						sf::RectangleShape pixel( sf::Vector2f( 10, 10 ) );
						pixel.setPosition( x*10, y*10 );
						pixel.setFillColor( sf::Color( 255, 255, 255, 255 ) );
						window.draw(pixel);
					}
				}
			}
		}

		// For debug
	//	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

		window.display();
	}

	return 0;
}

void handleInput() 
{

}
