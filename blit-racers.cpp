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

bool debugMode = false;
int8_t debugTimer = 0;

const uint8_t tileSize = 8;

struct TileData
{
	uint8_t id = 0;
	uint16_t index = 0;
	uint16_t detectionCount = 1;
};

struct TileScanData
{
	uint8_t id = 0;
	uint16_t index = 0;
	bool obstruction = false;
	uint16_t pixels_in_water = 0;
	bool in_water = false;
	float movement_modifier = 0;
	float life_modifier = 0;
	std::vector<Point> pointsChecked;
	//std::vector<uint8_t> idsFound;
	std::map<uint8_t, TileData> tilesScanned;
	Point collisionLocation;
};

class Track
{
public:
	uint8_t checkpointCount = 0;
	uint8_t * mapTiles;
	uint8_t * mapSpiteSheet;
	uint32_t tileMapHeight = NULL;
	uint32_t tileMapWidth = NULL;
	TileMap world;
	TileMap objectsLayer;
	TileMap checkpointLayer;

	Track() = default;
	
	Track(uint8_t checkpointCount, uint8_t * mapTiles, uint8_t * mapSpiteSheet, uint32_t tileMapHeight, uint32_t tileMapWidth, uint8_t* worldLayerSheet, uint8_t* objectsLayerSheet, uint8_t* checkpointLayerSheet)
	{
		this->checkpointCount = checkpointCount;
		this->mapTiles = mapTiles;
		this->mapSpiteSheet = mapSpiteSheet;
		this->tileMapHeight = tileMapHeight;
		this->tileMapWidth = tileMapWidth;

		world = TileMap(const_cast<uint8_t*>(map1), nullptr, Size(tileMapWidth, tileMapHeight), nullptr);

		auto objectLayerStart = map1 + tileMapHeight * tileMapWidth;
		objectsLayer = TileMap(const_cast<uint8_t*>(objectLayerStart), nullptr, Size(tileMapWidth, tileMapHeight), nullptr);

		auto checkpointLayerStart = map1 + ((tileMapHeight * tileMapWidth) * 2);
		checkpointLayer = TileMap(const_cast<uint8_t*>(checkpointLayerStart), nullptr, Size(tileMapWidth, tileMapHeight), nullptr);

		world.sprites = Surface::load(worldLayerSheet);
		objectsLayer.sprites = Surface::load(objectsLayerSheet);
		checkpointLayer.sprites = Surface::load(checkpointLayerSheet);
	}
};

class Actor {
public:
	virtual ~Actor() = default;
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

	TileScanData currentTileData;

	uint8_t currentCheckpoint = 1;

	uint32_t lapTime = 0;

	std::vector<uint32_t> completedLapTimes;

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

enum TileScanType
{
	Collision,
	Checkpoint
};

GameState state = Play;
Surface* backGroundSurface;
Surface* menu0ss;
Surface* menu1ss;
Surface* gameOver;

const uint32_t tilemap_width = 128;

const uint32_t tilemap_height = 128;

class Game
{
public:

	int8_t aiOffset = 0;

	int8_t vibrationTimer = 0;

	Track currentTrack;

	Game()
	{
		// Load first track
		currentTrack = Track(8, const_cast<uint8_t*>(map1), const_cast<uint8_t*>(spritesheet), 128, 128, const_cast<uint8_t*>(tile_sheet_1), const_cast<uint8_t*>(tile_sheet_1), const_cast<uint8_t*>(tile_sheet_1));
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

uint16_t getTileFromPoint(const Point& point, uint8_t tile_size, uint8_t tile_map_width) {
	uint16_t horizontal_location = (point.x / tile_size) - 1;

	uint16_t vertical_location = (point.y / tile_size) * tile_map_width;

	// Possible crap code
	if ((point.y / tile_size) % tile_size > 0) {
		vertical_location += 1;
	}

	const uint16_t array_location = horizontal_location + vertical_location;

	return array_location;
}

Game game;

Actor car;

float worldY = 0; // Map height + 8 // 392
float worldX = 0;

Point worldToScreen(Point point)
{
	return Point(
		point.x - car.camera.x + screen.bounds.w / 2,
		point.y - car.camera.y + screen.bounds.h / 2);
}

void ProcessCollisionScan(TileScanData& tileScanData, Point pointToCheck, const uint8_t tileScanned)
{
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
		tileScanData.obstruction = true;
		tileScanData.collisionLocation = pointToCheck;
		break;
	default:
		break;
	}
}

// todo replace point and sprite width/height with rect
TileScanData getLocalTileData(const Rect& boundingBox, uint8_t tile_size, uint8_t tileMapWidth, uint8_t* mapLayer, TileScanType scanType) {
	TileScanData tileScanData;

	for (auto y = 0; y < boundingBox.h; y++) {
		for (auto x = 0; x < boundingBox.w; x++) {
			auto pointToCheck = Point(boundingBox.x + x, boundingBox.y + y);

			tileScanData.pointsChecked.emplace_back(pointToCheck);

			const auto array_location = getTileFromPoint(pointToCheck,
				tile_size,
				tileMapWidth);
			const uint8_t tileScanned = *(mapLayer + array_location); //map1[array_location + tilemap_height * tilemap_width]; //map1[array_location];  //*(mapLayer + array_location); //mapLayer[array_location];

			TileData tileData;
			tileData.id = tileScanned;
			tileData.index = array_location;
			
			tileScanData.id = tileScanned;
			tileScanData.index = array_location;

			if (auto it{ tileScanData.tilesScanned.find(tileScanData.id) }; it != std::end(tileScanData.tilesScanned))
			{
				it->second.detectionCount += 1;
			}
			else
			{
				
				tileScanData.tilesScanned.emplace(tileData.id, tileData);
			}

			switch (scanType)
			{
			case Collision:
				ProcessCollisionScan(tileScanData, pointToCheck, tileScanned);
				break;
			case Checkpoint:
				//ProcessCheckpointScan(tileScanData, pointToCheck, tileScanned);
				break;
			}
			
		}
	}
	return tileScanData;
}


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

	screen.sprites = Surface::load(car1);

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

	game.currentTrack.world.draw(&screen, Rect(0, 0, maxX, maxY), level_line_interrupt_callback);
	game.currentTrack.objectsLayer.draw(&screen, Rect(0, 0, maxX, maxY), level_line_interrupt_callback);

	if(debugMode)
	{
		game.currentTrack.checkpointLayer.draw(&screen, Rect(0, 0, maxX, maxY), level_line_interrupt_callback);
	}

}

std::string GetLapTimeString(uint32_t lapTime)
{
	const uint32_t minutes = lapTime / 60000;
	const uint32_t seconds = (lapTime % 60000) / 1000;
	const uint32_t milliseconds = lapTime % 1000;

	std::string secondString = std::to_string(seconds);

	secondString.insert(secondString.begin(), 2 - secondString.size(), '0');

	std::string millisecondString = std::to_string(milliseconds);

	millisecondString.insert(millisecondString.begin(), 3 - millisecondString.size(), '0');


	 return std::to_string(minutes) + ":" + secondString + ":" + millisecondString;
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

	const std::string curLapText = GetLapTimeString(car.lapTime);

	auto backgroundSize = screen.measure_text(curLapText, minimal_font);

	backgroundSize.w = 40;
	
	screen.pen = Pen(0, 0, 0, 128);
	screen.rectangle(Rect(Point(0,0), backgroundSize));

	screen.pen = Pen(255, 0, 0);
	screen.text(curLapText, minimal_font, Point(0, 0));
	uint32_t lapNumber = 1;
	
	
	for (auto completed_lap_time : car.completedLapTimes)
	{	
		auto lapTimeString = "Lap:" + std::to_string(lapNumber) + " " + GetLapTimeString(completed_lap_time);
		backgroundSize = screen.measure_text(lapTimeString, minimal_font);
		screen.pen = Pen(0, 0, 0, 128);
		screen.rectangle(Rect(Point(0, lapNumber * 10), backgroundSize));
		screen.pen = Pen(255, 0, 0);
		screen.text( lapTimeString, minimal_font, Point(0, lapNumber * 10));
		lapNumber ++;
	}

	if(debugMode)
	{
		//screen.text("Vec X: " + std::to_string(car.movement.x), minimal_font, Point(0, 0));
		//screen.text("Vec Y: " + std::to_string(car.movement.y), minimal_font, Point(0, 10));
		//screen.text("V: " + std::to_string(car.speedMultiplier), minimal_font, Point(0, 20));


		//screen.text("d " + std::to_string(car.degrees), minimal_font, Point(0, 30));
		screen.text("X: " + std::to_string(int(car.x)), minimal_font, Point(0, 40));
		screen.text("Y: " + std::to_string(int(car.y)), minimal_font, Point(0, 50));

		std::string ids;

		int i = 0;
		
		for (auto tile : car.currentTileData.tilesScanned)
		{
			i++;
			ids.append(std::to_string(tile.second.id) + " (" + std::to_string(tile.second.detectionCount) + ") ");
			if(i == 3)
			{
				ids.append("\n");
				i = 0;
			}
		}

		screen.text("tiles: " + ids, minimal_font, Point(0, 80));

		for (auto pointChecked : car.currentTileData.pointsChecked)
		{		
			screen.pen = Pen(0, 255, 0, 50);
			screen.pixel(worldToScreen(pointChecked));
		}

		screen.pen = Pen(255, 0, 0);

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

	car.lapTime += 10;
	
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

		if(debugTimer > 0)
		{
			debugTimer--;
		}

		if(buttons & Button::X && debugTimer <= 0)
		{
			debugMode = !debugMode;
			debugTimer = 20;
		}

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

			car.currentTileData = getLocalTileData(Rect(car.x - (car.size.w / 2), car.y - (car.size.h / 2), car.size.w, car.size.h), tileSize, tilemap_width, game.currentTrack.objectsLayer.tiles, Collision);

			if(car.currentTileData.obstruction)
			{
				auto collisionVector =  Point(car.x, car.y) - car.currentTileData.collisionLocation;
				car.x += collisionVector.x;
				car.y += collisionVector.y;
			}
			
			ApplyCarMovement(radian, newVector);
			
			auto checkPointScan = getLocalTileData(Rect(car.x - (car.size.w / 2), car.y - (car.size.h / 2), car.size.w, car.size.h), tileSize, tilemap_width, game.currentTrack.checkpointLayer.tiles, Checkpoint);

			for (auto tiles_scanned : checkPointScan.tilesScanned)
			{
				if(tiles_scanned.second.id == car.currentCheckpoint + 1)
				{
					car.currentCheckpoint++;
				}
				else if(car.currentCheckpoint == game.currentTrack.checkpointCount && tiles_scanned.second.id == 1)
				{
					car.currentCheckpoint = 1;
					car.completedLapTimes.emplace_back(car.lapTime);
					car.lapTime = 0;
				}
			}
		}


		update_camera();
		break;
	case GameOver:
		break;
	}

	lastButtons = blit::buttons;
}