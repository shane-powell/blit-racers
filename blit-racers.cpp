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

float maxSpeed = 2.0f;
float acceleration = 0.01f;
float slowdown = 0.02f;
float rotationIncrement = 5;
float spriteRotationalSegmentSize = 15.0f;
float friction = 0.1f;

float lastXValue = 0.0;
float lastYValue = 0.0;

bool debugMode = true;

const uint8_t tileSize = 8;

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
	const uint8_t STEERINGDELAYMIN = 3;
	const uint8_t STEERINGDELAYMAX = 5;

	float prevX = 0.0f;
	float prevY = 0.0f;

	Vec2 movement;

	float speedMultiplier = 0.0;

	Actor() = default;

	Actor(float xIn, float yIn)
	{
		this->x = xIn;
		this->y = yIn;

		movement = Vec2(0, 1);
	}



	void GenerateSpriteMap(float angle)
	{
		int32_t x = 0;
		int32_t y = 0;

		for (int8_t i = 0; i < 24; i++)
		{
			sprites.emplace(angle, Rect(x, y, 3, 3));

			if (x < 12)
			{
				x += 3;
			}
			else
			{
				x = 0;
				y += 3;
			}

			if (angle == 360)
			{
				angle = 15;
				sprites.emplace(0, Rect(x, y, 3, 3));
			}
			else
			{
				angle += 15;
			}
		}
	}
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

GameState state = Play;
Surface* backGroundSurface;
Surface* menu0ss;
Surface* menu1ss;
Surface* gameOver;

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
		if (vibrationTimer > 0)
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

const uint32_t tilemap_width = 128;

const uint32_t tilemap_height = 128;



TileMap world(const_cast<uint8_t*>(map1), nullptr, Size(tilemap_width, tilemap_height), nullptr);
auto objectLayerStart = map1 + tilemap_height * tilemap_width;
TileMap objectsLayer(const_cast<uint8_t*>(objectLayerStart), nullptr, Size(tilemap_width, tilemap_height), nullptr);

struct TileData
{
	uint8_t id = 0;
	uint16_t index = 0;
	bool obstruction = false;
	uint16_t pixels_in_water = 0;
	bool in_water = false;
	float movement_modifier = 0;
	float life_modifier = 0;
};

uint16_t getTileFromPoint(const Point& point, uint8_t tile_size, uint8_t tile_map_width) {
	uint16_t horizontal_location = point.x / tile_size;

	uint16_t vertical_location = (point.y / tile_size) * tile_map_width;

	if ((point.y / tile_size) % tile_size > 0) {
		vertical_location += 1;
	}

	const uint16_t array_location = horizontal_location + vertical_location;

	return array_location;
}

TileData getLocalTileData(const blit::Point& Point_to_check, uint8_t tile_size, uint8_t tileMapWidth, uint8_t spriteWidth, uint8_t* mapLayer) {
	TileData tileData;

	for (auto y = 0; y < spriteWidth; y++) {
		for (auto x = 0; x < spriteWidth; x++) {
			const auto array_location = getTileFromPoint(Point(Point_to_check.x + x, Point_to_check.y + y),
				tile_size,
				tileMapWidth);
			const uint8_t tileScanned = *(mapLayer + array_location); //map1[array_location + tilemap_height * tilemap_width]; //map1[array_location];  //*(mapLayer + array_location); //mapLayer[array_location];

			tileData.id = tileScanned;
			tileData.index = array_location;

			switch (tileScanned) {
			//case 10:
			//case 11:
			//case 12:
			//case 13:
			//case 26:
			//case 27:
			//case 28:
			//case 29:
				
			case 36:
			case 37:
			case 38:
			case 39:
			case 52:
			case 53:
			case 54:
			case 55:
			case 68:
			case 69:
			case 70:
			case 71:
			case 84:
			case 85:
			case 86:
			case 87:

				
				tileData.obstruction = true;
				break;
			default:
				break;
			}
		}
	}
	return tileData;
}

Game game;

Actor car;

float worldY = 0; // Map height + 8 // 392
float worldX = 0;


// Line-interrupt callback for level->draw that applies our camera transformation
// This can be expanded to add effects like camera shake, wavy dream-like stuff, all the fun!
std::function<Mat3(uint8_t)> level_line_interrupt_callback = [](uint8_t y) -> Mat3 {
	return camera;
};

static double MapRange(float range1Min, float range1Max, float range1Value, float range2Min, float range2Max)
{
	const float slope = 1.0f * (range2Max - range2Min) / (range1Max - range1Min);
	const auto output = range2Min + slope * (range1Value - range1Min);

	return output;
}

void update_camera() {
	// Create a camera transform that centers around the car's position
	/*if (car.camera.x < car.x) {
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
	}*/

	car.camera.x = car.x;
	car.camera.y = car.y;


	camera = Mat3::identity();
	camera *= Mat3::translation(Vec2(car.x, car.y)); // offset to middle of world
	//camera *= Mat3::translation(Vec2(car.camera.x * 8.0f, car.camera.y * 8.0f)); // offset to middle of world
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
	objectsLayer.sprites = SpriteSheet::load(tile_sheet_1);

	screen.sprites = SpriteSheet::load(car1);

	car.size = Size(24, 24);
	car.spriteLocation = Rect(0, 0, 3, 3);

	car.GenerateSpriteMap(180);

	car.x = 270;
	car.y = 480;

	car.degrees = 180;

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
	objectsLayer.draw(&screen, Rect(0, 0, maxX, maxY), level_line_interrupt_callback);

}

void DrawGame()
{
	DrawWorld();

	//screen.sprite(car.spriteLocation, Point(car.x, car.y));

	auto sprite = car.degrees;

	float remainder = fmod(sprite, spriteRotationalSegmentSize);
	
	if(remainder > 0)
	{
		if(remainder < spriteRotationalSegmentSize / 2)
		{
			sprite -= remainder;
		}
		else
		{
			sprite += (spriteRotationalSegmentSize - remainder);
		}
	}
	
	screen.sprite(car.sprites[sprite], Point(maxX / 2 - (car.size.w / 2), maxY / 2 - (car.size.h / 2)));
	screen.pen = Pen(255, 0, 0);

	if(debugMode)
	{
		//screen.text("Vec X: " + std::to_string(car.movement.x), minimal_font, Point(0, 0));
		//screen.text("Vec Y: " + std::to_string(car.movement.y), minimal_font, Point(0, 10));
		//screen.text("V: " + std::to_string(car.speedMultiplier), minimal_font, Point(0, 20));


		//screen.text("d " + std::to_string(car.degrees), minimal_font, Point(0, 30));
		screen.text("X: " + std::to_string(car.x), minimal_font, Point(0, 40));
		screen.text("Y: " + std::to_string(car.y), minimal_font, Point(0, 50));
		auto tileData = getLocalTileData(Point(car.x, car.y), tileSize, tilemap_width, car.size.w, objectsLayer.tiles);
		screen.text("tile: " + std::to_string(tileData.id), minimal_font, Point(0, 60));
	}

	
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


void ApplyCarMovement(float radian, Vec2 newVector)
{
	car.movement.x = car.movement.x * friction;
	car.movement.y = car.movement.y * friction;

	car.movement.rotate(radian);
			
	car.movement.x += newVector.x;
	car.movement.y += newVector.y;
			
	car.x += car.movement.x;
	car.y += car.movement.y;

	newVector.rotate(radian * -1);
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

	if (car.inputDelay > 0)
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
		if (car.moveEnabled)
		{
			if (buttons & Button::A && car.speedMultiplier < maxSpeed)
			{
				car.speedMultiplier += acceleration;
				car.speedMultiplier = std::min(maxSpeed, car.speedMultiplier);  // NOLINT(clang-diagnostic-implicit-float-conversion)
			}
			else if (car.speedMultiplier > 0 && !(buttons & Button::A))
			{
				car.speedMultiplier -= slowdown;  // NOLINT(clang-diagnostic-implicit-float-conversion)
				car.speedMultiplier = std::max(0.0f, car.speedMultiplier);
			}

			if (car.inputDelay == 0)
			{
				if (buttons & Button::DPAD_RIGHT || joystick.x > 0) {
					car.inputDelay = round(MapRange(0, maxSpeed, car.speedMultiplier, car.STEERINGDELAYMIN, car.STEERINGDELAYMAX));

					if (car.degrees == 360.0f)
					{
						car.degrees = rotationIncrement;
					}
					else
					{
						car.degrees += rotationIncrement;
					}
				}
				else if (buttons & Button::DPAD_LEFT || joystick.x < 0) {
					car.inputDelay = round(MapRange(0, maxSpeed, car.speedMultiplier, car.STEERINGDELAYMIN, car.STEERINGDELAYMAX));

					if (car.degrees == 0.0f)
					{
						car.degrees = 360 - rotationIncrement;
					}
					else
					{
						car.degrees -= rotationIncrement;
					}
				}
			}

			float radian = (pi * car.degrees) / 180.00f;

			Vec2 newVector = Vec2(0, 1);
			newVector.rotate(radian);

			newVector.x = newVector.x * car.speedMultiplier;
			newVector.y = newVector.y * car.speedMultiplier;

			auto tileData = getLocalTileData(Point(floor(car.x), floor(car.y)), tileSize, tilemap_width, car.size.w, objectsLayer.tiles);

			if(!tileData.obstruction)
			ApplyCarMovement(radian, newVector);
			
			


		}


		update_camera();
		break;
	case GameOver:
		break;
	}

	lastButtons = blit::buttons;
}