
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.

const int resolutionX = 960;
const int resolutionY = 640;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid

// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};


void drawPlayer(RenderWindow& window, float& player_x, float& player_y, Sprite& playerSprite);
void moveBullet(float& bullet_y, bool& bullet_exists, Clock& bulletClock);
void drawBullet(RenderWindow& window, float& bullet_x, float& bullet_y, Sprite& bulletSprite);

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Write your functions declarations here. Some have been written for you. //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////


int main()
{
	srand(time(0));

	// Declaring RenderWindow.
	RenderWindow window(VideoMode(resolutionX, resolutionY), "Buzz Bombers", Style::Close | Style::Titlebar);

	// Used to position your window on every launch. Use according to your needs.
	window.setPosition(Vector2i(500, 200));

	// Initializing Background Music.
	Music bgMusic;
	bgMusic.openFromFile("Music/Forest.ogg");
	bgMusic.play();
	bgMusic.setVolume(50);


	// Initializing Player and Player Sprites.
	float player_x = (gameRows / 2) * boxPixelsX;
	float player_y = (gameColumns - 4) * boxPixelsY;

	Texture playerTexture;
	Sprite playerSprite;
	playerTexture.loadFromFile("Textures/spray.png");
	playerSprite.setTexture(playerTexture);
	//	playerSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

		// Initializing Bullet and Bullet Sprites
		// Data for bullet / Spray pellet

	float bullet_x = player_x;
	float bullet_y = player_y;
	bool bullet_exists = true;

	Clock bulletClock;
	Texture bulletTexture;
	Sprite bulletSprite;
	bulletTexture.loadFromFile("Textures/bullet.png");
	bulletSprite.setTexture(bulletTexture);
	bulletSprite.setScale(3, 3);
	bulletSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	// The ground on which player moves

	RectangleShape groundRectangle(Vector2f(960, 64));
	groundRectangle.setPosition(0, (gameColumns - 2) * boxPixelsY);
	groundRectangle.setFillColor(Color::Green);

	while (window.isOpen()) {

		Event e;
		while (window.pollEvent(e)) {
			if (e.type == Event::Closed) {
				return 0;
			}
		}

		///////////////////////////////////////////////////////////////
		//                                                           //
		// Call Your Functions Here. Some have been written for you. //
		// Be vary of the order you call them, SFML draws in order.  //
		//                                                           //
		///////////////////////////////////////////////////////////////


		if (bullet_exists == true)
		{
			moveBullet(bullet_y, bullet_exists, bulletClock);
			drawBullet(window, bullet_x, bullet_y, bulletSprite);
		}
		else
		{
			bullet_x = player_x;
			bullet_y = player_y;
		}

		drawPlayer(window, player_x, player_y, playerSprite);
		window.draw(groundRectangle);
		window.display();
		window.clear();
	}
}

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Write your functions definitions here. Some have been written for you. //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

void drawPlayer(RenderWindow& window, float& player_x, float& player_y, Sprite& playerSprite) {
	playerSprite.setPosition(player_x, player_y);
	window.draw(playerSprite);
}
void moveBullet(float& bullet_y, bool& bullet_exists, Clock& bulletClock) {
	if (bulletClock.getElapsedTime().asMilliseconds() < 20)
		return;

	bulletClock.restart();
	bullet_y -= 10;
	if (bullet_y < -32)
		bullet_exists = false;
}
void drawBullet(sf::RenderWindow& window, float& bullet_x, float& bullet_y, Sprite& bulletSprite) {
	bulletSprite.setPosition(bullet_x, bullet_y);
	window.draw(bulletSprite);
}

