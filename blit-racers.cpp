#include "blit-racers.hpp"
#include "assets.hpp"
#include <limits>
#include <algorithm>
#include <map>
using namespace blit;

int32_t maxX = 160;
int32_t minX = 0;
int32_t maxY = 120;
int32_t minY = 0;

int32_t rowHeight = 40;

Mat3 camera;

class Actor {
public:
	virtual ~Actor() = default;
	//Point loc = Point(16, 16);
	Size size;
	Rect spriteLocation;
	float x = 0.0f;
	float y = 0.0f;
	Vec2 camera;
	float degrees = 270.0f;
	std::map<float, Rect> sprites;
	uint8_t inputDelay = 0;
	bool moveEnabled = true;
	const uint8_t steeringDelay = 10;

	float speedMultiplier = 0.0f;
	
	/*float xSpeed = 0.0f;
	float ySpeed = 0.0f;*/

	Actor() = default;

	Actor(float xIn, float yIn)
	{
		this->x = xIn;
		this->y = yIn;
	}

	/*virtual Size GetSize()
	{
		return this->size;
	}

	virtual Rect GetSpriteLocation()
	{
		return this->spriteLocation;
	}

	Point GetLocation()
	{
		return this->loc;
	}*/
};

enum PlayerState
{
	Alive,
	Splat,
	Splash,
	Eagle
};

enum GameState {
	Menu,
	Play,
	GameOver
};

GameState state = Menu;
SpriteSheet* backGroundSurface;
SpriteSheet* menu0ss;
SpriteSheet* menu1ss;
SpriteSheet* gameOver;

class Game
{
public:

	int8_t aiOffset = 0;

	int8_t vibrationTimer = 0;

	Game()
	{

	}

	Game(int8_t noPlayers)
	{

	}

	void Update()
	{
		if(vibrationTimer > 0)
		{
			blit::vibration = 1;
			vibrationTimer--;
		}
		else
		{
			blit::vibration = 0;
		}
	}


};

Game game;

Actor car;

const uint32_t tilemap_width = 32;

const uint32_t tilemap_height = 64;

float worldY = 0; // Map height + 8 // 392
float worldX = 0;

TileMap world(const_cast<uint8_t*>(map1), nullptr, Size(tilemap_width, tilemap_height), nullptr);

// Line-interrupt callback for level->draw that applies our camera transformation
// This can be expanded to add effects like camera shake, wavy dream-like stuff, all the fun!
std::function<Mat3(uint8_t)> level_line_interrupt_callback = [](uint8_t y) -> Mat3 {
	return camera;
};

void update_camera() {
	// Create a camera transform that centers around the car's position
	if (car.camera.x < car.x) {
		car.camera.x += 0.1f;
	}
	if (car.camera.x > car.x) {
		car.camera.x -= 0.1f;
	}
	if (car.camera.y < car.y) {
		car.camera.y += 0.1f;
	}
	if (car.camera.y > car.y) {
		car.camera.y -= 0.1f;
	}


	camera = Mat3::identity();
	//camera *= Mat3::translation(Vec2(car.x, car.y)); // offset to middle of world
	camera *= Mat3::translation(Vec2(car.camera.x * 8.0f, car.camera.y * 8.0f)); // offset to middle of world
	camera *= Mat3::translation(Vec2(-maxX / 2, -maxY / 2)); // transform to centre of framebuffer
}

///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init() {
	set_screen_mode(ScreenMode::lores);

	world.sprites = SpriteSheet::load(tile_sheet_1);
	screen.sprites = SpriteSheet::load(car1);

	car.size = Size(24, 24);
	car.spriteLocation = Rect(0, 0, 3, 3);

	car.sprites.emplace(270, Rect(0, 0, 3, 3));
	car.sprites.emplace(285, Rect(0, 0, 3, 3));
	car.sprites.emplace(300, Rect(3, 0, 3, 3));
	car.sprites.emplace(315, Rect(6, 0, 3, 3));
	car.sprites.emplace(330, Rect(9, 0, 3, 3));
	car.sprites.emplace(345, Rect(12, 0, 3, 3));
	car.sprites.emplace(0, Rect(3, 3, 3, 3));
	car.sprites.emplace(360, Rect(3, 3, 3, 3));

	car.sprites.emplace(15, Rect(6, 3, 3, 3));
	car.sprites.emplace(30, Rect(9, 3, 3, 3));
	car.sprites.emplace(45, Rect(12, 3, 3, 3));
	car.sprites.emplace(60, Rect(0, 6, 3, 3));
	car.sprites.emplace(75, Rect(3, 6, 3, 3));
	car.sprites.emplace(90, Rect(6, 6, 3, 3));

	car.sprites.emplace(105, Rect(9, 6, 3, 3));
	car.sprites.emplace(120, Rect(12, 6, 3, 3));
	car.sprites.emplace(135, Rect(0, 9, 3, 3));
	car.sprites.emplace(150, Rect(3, 9, 3, 3));
	car.sprites.emplace(165, Rect(6, 9, 3, 3));
	car.sprites.emplace(180, Rect(9, 9, 3, 3));

	car.sprites.emplace(195, Rect(12, 9, 3, 3));
	car.sprites.emplace(210, Rect(0, 12, 3, 3));
	car.sprites.emplace(225, Rect(3, 12, 3, 3));
	car.sprites.emplace(240, Rect(6, 12, 3, 3));
	car.sprites.emplace(255, Rect(9, 12, 3, 3));



	car.x = 30;
	car.y = 30;

	car.camera = Vec2(car.x + (car.size.w / 2), car.y + (car.size.h / 2));

}

void DrawMenu()
{
	
}



void DrawWorld()
{
	/*const Vec2 wo(worldX, worldY);

	world.transform =
		Mat3::identity() *
		Mat3::translation(wo);*/
		
	world.draw(&screen, Rect(0, 0, maxX, maxY), level_line_interrupt_callback);

}

void DrawGame()
{
	DrawWorld();

	//screen.sprite(car.spriteLocation, Point(car.x, car.y));
	screen.sprite(car.sprites[car.degrees], Point(maxX / 2 - (car.size.w / 2), maxY / 2 - (car.size.h / 2)));
	screen.pen = Pen(255, 0, 0);
	screen.text("X: " + std::to_string((int)car.x), fat_font, Point(0, 0));
	screen.text("Y: " + std::to_string((int)car.y), fat_font, Point(0, 10));
}

void DrawGameOver()
{
}

///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game. time is the 
// amount if milliseconds elapsed since the start of your game
//
void render(uint32_t time) {
	screen.pen = Pen(0, 0, 0, 255);
	//screen.mask = nullptr;
	
	// clear the screen -- screen is a reference to the frame buffer and can be used to draw all things with the 32blit
	screen.clear();
	

	switch (state)
	{
	case Menu:
		DrawMenu();
		break;
	case Play:
		DrawGame();
		break;
	case GameOver:
		DrawGameOver();
		break;
	}
}



///////////////////////////////////////////////////////////////////////////
//
// update(time)
//
// This is called to update your game state. time is the 
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time) {
	
	static uint16_t lastButtons = 0;
	uint16_t changed = blit::buttons ^ lastButtons;
	uint16_t pressed = changed & blit::buttons;
	uint16_t released = changed & ~blit::buttons;

	if(car.inputDelay > 0)
	{
		car.inputDelay--;
	}

	switch (state)
	{
	case Menu:
		if (buttons & Button::A)
		{
			state = Play;
		}
		break;
	case Play:
		if(car.moveEnabled)
		{
			if (buttons & Button::A)
			{
				car.speedMultiplier += 0.1;
			}
			else if(car.speedMultiplier > 0)
			{
				car.speedMultiplier -= 0.1;
			}
			if(car.inputDelay == 0)
			{
				if (buttons & Button::DPAD_RIGHT || joystick.x > 0) {
					car.inputDelay = car.steeringDelay;
					if (car.degrees == 360)
					{
						car.degrees = 15;
					}
					else
					{
						car.degrees += 15;
					}
				}
				else if (buttons & Button::DPAD_LEFT || joystick.x < 0) {
					car.inputDelay = car.steeringDelay;
					if (car.degrees == 0)
					{
						car.degrees = 360 - 15;
					}
					else
					{
						car.degrees -= 15;
					}
				}
			}

			/*Vec2 movement(0, 1);
			movement.rotate(angle);
			pos += movement;*/
			float radian = (pi * car.degrees) / 180.00f;

			auto x = cos(radian);
			auto y = sin(radian);

			const auto xIn = (x / 16) * car.speedMultiplier;
			const auto yIn = (y / 16) * car.speedMultiplier;

			//need to map screenspace to location

			car.y += yIn;

			car.x += xIn;

			//worldX += xIn;
			//worldX = (car.x + (car.size.w / 2)) - (maxX / 2);
			//worldY += yIn;
			//worldY = (car.y + (car.size.h / 2)) - (maxY / 2);
		}
		

		update_camera();
		break;
	//case GameOver:
		//break;
	}

	lastButtons = blit::buttons;
}