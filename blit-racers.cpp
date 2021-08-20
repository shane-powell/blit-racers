#include "blit-racers.hpp"
#include "assets.hpp"
#include <limits>
#include <algorithm>
#include <iostream>
#include <map>
#include <utility>
#include "animation-frame.hpp"
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
int8_t buttonBounceTimer = 0;
float debugAngle;

const uint8_t tileSize = 8;

int32_t millisElapsed = 0;
bool captureNodes = false;
std::vector<Point> nodes;

double CalculateDistance(blit::Point& pointA, blit::Point pointB)
{
	return std::sqrt(pow((pointB.x - pointA.x), 2) + pow((pointB.y - pointA.y), 2));
}

Vec2 DegreesToVector(float degrees)
{
	float radian = (pi * degrees) / 180.00f;

	Vec2 newVector = Vec2(0, 1);
	newVector.rotate(radian);

	return newVector;
}

float calcAngleBetweenPoints(Point a, Point b)
{
	// calculate angle as radian from car to target node
	float targetAngle = atan2(b.x - a.x, b.y - a.y);

	// get angle in degrees
	targetAngle = targetAngle * 180.00f / pi;

	targetAngle = targetAngle * -1;
	

	if (targetAngle < 0)
	{
		targetAngle += 360;
	}

	return targetAngle;
}

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
	//std::vector<Point> pointsChecked;
	//std::vector<uint8_t> idsFound;
	std::map<uint8_t, TileData> tilesScanned;
	Point collisionLocation;
};

class Position
{
public:
	Point location;
	float angle;

	Position(Point location, float angle)
	{
		this->location = location;
		this->angle = angle;
	}
};

//struct Node {
//	Point data;
//	struct Node* next;
//};

class Track
{
public:
	uint8_t checkpointCount = 0;
	const uint8_t* mapTiles{};
	const uint8_t* mapSpiteSheet{};
	Surface* carSpriteSheet{};
	uint32_t tileMapHeight = NULL;
	uint32_t tileMapWidth = NULL;
	TileMap *world{};
	TileMap *objectsLayer{};
	TileMap *checkpointLayer{};
	Surface *image;
	std::string title;

	std::vector<Position> startLocations;

	// todo split linked list for multiple paths
	std::vector<Point> nodes;

	std::vector<Point> checkPointLocations;

	uint8_t laps = 3;
	
	Track(const uint8_t checkpointCount,
		const uint8_t* mapTiles,
		const uint8_t* mapSpiteSheet,
		const uint32_t tileMapHeight,
		const uint32_t tileMapWidth,
		const uint8_t* worldLayerSheet,
		uint8_t* objectsLayerSheet,
		uint8_t* checkpointLayerSheet,
		std::vector<Position> startLocations,
		std::vector<Point> nodes,
		uint8_t* image,
		std::string title,
		const uint8_t* carSpriteSheet,
		std::vector<Point> checkPointLocations,
		uint8_t laps = 3) 
	{
		this->checkpointCount = checkpointCount;
		this->mapTiles = mapTiles;
		this->mapSpiteSheet = mapSpiteSheet;
		this->tileMapHeight = tileMapHeight;
		this->tileMapWidth = tileMapWidth;
		this->startLocations = std::move(startLocations);
		this->nodes = std::move(nodes);
		this->image = Surface::load(image);
		this->title = std::move(title);
		this->laps = laps;
		this->carSpriteSheet = Surface::load(carSpriteSheet);
		this->checkPointLocations = checkPointLocations;
		world = new TileMap(const_cast<uint8_t*>(map1), nullptr, Size(tileMapWidth, tileMapHeight), nullptr);

		auto objectLayerStart = map1 + tileMapHeight * tileMapWidth;
		objectsLayer = new TileMap(const_cast<uint8_t*>(objectLayerStart), nullptr, Size(tileMapWidth, tileMapHeight), nullptr);
		objectsLayer->empty_tile_id = 0;

		auto checkpointLayerStart = map1 + ((tileMapHeight * tileMapWidth) * 2);
		checkpointLayer = new TileMap(const_cast<uint8_t*>(checkpointLayerStart), nullptr, Size(tileMapWidth, tileMapHeight), nullptr);

		world->sprites = Surface::load(worldLayerSheet);
		objectsLayer->sprites = Surface::load(objectsLayerSheet);
		checkpointLayer->sprites = Surface::load(checkpointLayerSheet);
	}

	~Track()
	{
		delete[] world->sprites->data;
		delete[] world->sprites->palette;
		delete world->sprites;
		delete world;

		delete[] objectsLayer->sprites->data;
		delete[] objectsLayer->sprites->palette;
		delete objectsLayer->sprites;
		delete objectsLayer;

		delete[] checkpointLayer->sprites->data;
		delete[] checkpointLayer->sprites->palette;
		delete checkpointLayer->sprites;
		delete checkpointLayer;

		delete image->data;
		delete image->palette;
		delete image;

		delete carSpriteSheet->data;
		delete carSpriteSheet->palette;
		delete carSpriteSheet;
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

	bool isPlayer = false;

	uint8_t targetNode;

	uint8_t carNumber = 0;

	uint8_t position = 0;

	bool sorted = false;

	std::function<Point&(uint8_t currentCheckpoint)> getNextTargetCheckpoint;

	Actor() = default;

	Actor(float xIn, float yIn)
	{
		this->x = xIn;
		this->y = yIn;

		movement = Vec2(0, 1);
		this->GenerateSpriteMap(180);
	}

	Actor(Position gridPosition, Rect spriteLocation, Size size, uint8_t targetNode, uint8_t carNumber, std::function<Point&(uint8_t currentCheckpoint)> getNextTargetCheckpoint, bool isPlayer = false)
	{
		this->SetLocation(gridPosition);
		this->spriteLocation = spriteLocation;
		this->size = size;
		this->isPlayer = isPlayer;
		this->targetNode = targetNode;
		movement = Vec2(0, 1);
		this->GenerateSpriteMap(180);
		this->carNumber = carNumber;
		this->getNextTargetCheckpoint = getNextTargetCheckpoint;
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

	void SetLocation(Position gridPosition)
	{
		this->x = gridPosition.location.x;
		this->y = gridPosition.location.y;
		this->degrees = gridPosition.angle;
	}

	static bool SortByPosition (const Actor* carA, const Actor* carB) {

		//std::cout << std::to_string(carA->carNumber) + " VS " + std::to_string(carB->carNumber) + "\n";

		bool result = false;

		if (carA->completedLapTimes.size() > carB->completedLapTimes.size())
		{
			result = true;
			//std::cout << std::to_string(result) + "(lap count) \n";
			return result;
		}
		
		if (carA->completedLapTimes.size() == carB->completedLapTimes.size())
		{
			if (carA->currentCheckpoint > carB->currentCheckpoint)
			{
				result = true;
				//std::cout << std::to_string(result) + "(CheckPoint) \n";
				return result;
			}

			if (carA->currentCheckpoint == carB->currentCheckpoint)
			{
				auto distanceA = (int)CalculateDistance(carA->getNextTargetCheckpoint(carA->currentCheckpoint), Point(carA->x, carA->y));

				auto distanceB = (int)CalculateDistance(carB->getNextTargetCheckpoint(carB->currentCheckpoint), Point(carB->x, carB->y));

				if (distanceA < distanceB)
				{
					result = true;
					//std::cout << std::to_string(result) + "(Distance) \n";
					return result;
				}
				//else if (distanceA == distanceB)
				//{
				//	result = carA->carNumber < carB->carNumber;
				//	//std::cout << std::to_string(result) + "(Tiebreaker) \n";
				//	return result;
				//}
			}
		}		

		//std::cout << std::to_string(result) + "(Default) \n";
		return result;
		

		/*if (!car1->completedLapTimes.size() >= car2->completedLapTimes.size())
		{
			return false;
		}

		if (!car1->currentCheckpoint >= car2->currentCheckpoint)
		{
			return false;
		}

		if (CalculateDistance(car1->getNextTargetCheckpoint(car1->currentCheckpoint), Point(car1->x, car1->y)) <= CalculateDistance(car2->getNextTargetCheckpoint(car2->currentCheckpoint), Point(car2->x, car2->y)))
		{
			return true;
		}
		else
		{
			return false;
		}*/
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
	MainMenu,
	Play,
	GameOver,
	LevelSelect
};

enum TileScanType
{
	Collision,
	Checkpoint
};

GameState state = MainMenu;

Surface* lightSprites = Surface::load(lights);

const uint32_t tilemap_width = 128;

const uint32_t tilemap_height = 128;

class Game
{
public:

	int8_t aiOffset = 0;

	int8_t vibrationTimer = 0;

	Track* currentTrack;

	std::vector<Actor*> cars;
	
	Actor* PlayerCar;

	uint8_t aiCount = 3;

	uint8_t place = 1;

	bool raceStarted = false;

	std::vector<std::vector<AnimationFrame>> animations;

	Game()
	{
		// Load first track
		currentTrack = new Track(8, const_cast<uint8_t*>(map1), const_cast<uint8_t*>(spritesheet), 128, 128, const_cast<uint8_t*>(tile_sheet_1), const_cast<uint8_t*>(tile_sheet_1), const_cast<uint8_t*>(tile_sheet_1),
			{
				Position(Point(270,480),180),
				Position(Point(305,480),180),
				Position(Point(270,520),180),
				Position(Point(310,520),180)
			}
		,
			{
				Point(270, 440),
				Point(270, 412),
				Point(270, 346),
				Point(286, 297),
				Point(331,288),
				Point(405, 287),
				Point(486, 305),
				Point(555, 334),
				Point(636, 300),
				Point(699,283),
				Point(726, 319),
				Point(720, 394),
				Point(728, 477),
				Point(653,526),
				Point(589, 550),
				Point(498, 557),
				Point(399, 548),
				Point(302, 536),
				Point(257,501),
				Point(262, 469)
			}, const_cast<uint8_t*>(map1_preview), "Kitchen", const_cast<uint8_t*>(car1), {
				Point(280, 448),
				Point(288, 304),
				Point(504, 320),
				Point(656, 280),
				Point(288, 304),
				Point(728, 424),
				Point(672, 536),
				Point(536, 536),
				Point(368, 544)



			});

		uint8_t gridPosition = 0;

		std::function<Point&(uint8_t currentCheckpoint)> getNextTargetCheckpoint = std::function([&](uint8_t currentCheckpoint) -> Point&
			{
				if (currentCheckpoint < this->currentTrack->checkPointLocations.size())
				{
					return this->currentTrack->checkPointLocations[currentCheckpoint];
				}

				return this->currentTrack->checkPointLocations[0];
			});
		
		PlayerCar = new Actor(currentTrack->startLocations[gridPosition], Rect(0, 0, 3, 3), Size(24, 24), 0, gridPosition, getNextTargetCheckpoint, true);
		gridPosition++;
		PlayerCar->camera = Vec2(PlayerCar->x + (PlayerCar->size.w / 2), PlayerCar->y + (PlayerCar->size.h / 2));

		cars.emplace_back(PlayerCar);

		for (int i = 0; i < aiCount; ++i)
		{
			if(currentTrack->startLocations.size() >= gridPosition + 1)
			{
				cars.emplace_back(new Actor(currentTrack->startLocations[gridPosition], Rect(0, 0, 3, 3), Size(24, 24), 0, gridPosition, getNextTargetCheckpoint));
				gridPosition++;
			}
		}
	}

	~Game()
	{
		delete this->currentTrack;
		//delete this->PlayerCar;
		
		for (int i = 0; i < cars.size(); ++i)
		{
			delete this->cars[i];
		}
	}

	/*Game(int8_t noPlayers)
	{

	}*/

	/*Point getNextTargetCheckpoint(uint8_t currentCheckpoint)
	{
		if (currentCheckpoint < this->currentTrack->checkPointLocations.size())
		{
			return this->currentTrack->checkPointLocations[currentCheckpoint];
		}

		return this->currentTrack->checkPointLocations[0];
	}*/

	void initRace() {
		std::vector<AnimationFrame> startLightOneFrames;

		Point lightsStart = Point(maxX / 2 - 36, 0);

		startLightOneFrames.emplace_back(AnimationFrame(Rect(0, 0, 3, 3), lightsStart, 100));
		startLightOneFrames.emplace_back(AnimationFrame(Rect(3, 0, 3, 3), lightsStart, 300, std::function([&]() {raceStarted = true; })));
		startLightOneFrames.emplace_back(AnimationFrame(Rect(0, 3, 3, 3), lightsStart, 100));

		animations.emplace_back(startLightOneFrames);

		std::vector<AnimationFrame> startLightTwoFrames;

		startLightTwoFrames.emplace_back(AnimationFrame(Rect(0, 0, 3, 3), Point(lightsStart.x + 24, 0), 200));
		startLightTwoFrames.emplace_back(AnimationFrame(Rect(3, 0, 3, 3), Point(lightsStart.x + 24, 0), 200));
		startLightTwoFrames.emplace_back(AnimationFrame(Rect(0, 3, 3, 3), Point(lightsStart.x + 24, 0), 100));

		animations.emplace_back(startLightTwoFrames);

		std::vector<AnimationFrame> startLightThreeFrames;

		startLightThreeFrames.emplace_back(AnimationFrame(Rect(0, 0, 3, 3), Point(lightsStart.x + 48, 0), 300));
		startLightThreeFrames.emplace_back(AnimationFrame(Rect(3, 0, 3, 3), Point(lightsStart.x + 48, 0), 100));
		startLightThreeFrames.emplace_back(AnimationFrame(Rect(0, 3, 3, 3), Point(lightsStart.x + 48, 0), 100));

		animations.emplace_back(startLightThreeFrames);

	}

	void UpdateAnimations()
	{
		auto animation = animations.begin();

		while (animation != animations.end()) {
			if (animation->size() > 0)
			{
				if (animation->at(0).finished)
				{
					animation->erase(animation->begin());
				}
				else
				{
					animation->at(0).Animate();
				}
			}
			else
			{
				animation = animations.erase(animation);

				continue;
			}
			++animation;
		}

		if (animations.size() == 0)
		{
			//this->raceStarted = true;
		}
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

Game * game;

float worldY = 0; // Map height + 8 // 392
float worldX = 0;

Point worldToScreen(Point point, Vec2 camera)
{
	return Point(
		point.x - camera.x + screen.bounds.w / 2,
		point.y - camera.y + screen.bounds.h / 2);
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

			//tileScanData.pointsChecked.emplace_back(pointToCheck);

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

void update_camera(Actor* car) {

	car->camera.x = car->x;
	car->camera.y = car->y;

	camera = Mat3::identity();
	camera *= Mat3::translation(Vec2(car->x, car->y)); // offset to middle of world
	//camera *= Mat3::translation(Vec2(PlayerCar->camera.x * 8.0f, PlayerCar->camera.y * 8.0f)); // offset to middle of world
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
	maxX = screen.bounds.w;
}


void DrawMenu()
{
screen.pen = Pen(255, 255, 255, 255);


#ifdef DISPLAY_ST7789
	screen.text("Press Button A\nto start.", minimal_font, Point(maxX / 2, maxY / 2), true, center_h);
#else
	screen.text("Press Button A to start.", outline_font, Point(maxX / 2, maxY / 2), true, center_h);
#endif
}

void DrawLevelSelect()
{
	screen.pen = Pen(255, 255, 255, 255);
	screen.text("Select a track", outline_font, Point(maxX / 2, minY + 10), true, center_h);
	screen.sprites = game->currentTrack->image;
	screen.sprite(Rect(0, 0, 80 / 8, 60 / 8), Point(maxX / 4, maxY / 4));
	screen.text(game->currentTrack->title, outline_font, Point(maxX / 2, minY + 90), true, center_h);

}

void DrawWorld()
{
	game->currentTrack->world->draw(&screen, Rect(0, 0, maxX, maxY), level_line_interrupt_callback);
	game->currentTrack->objectsLayer->draw(&screen, Rect(0, 0, maxX, maxY), level_line_interrupt_callback);

	if(debugMode)
	{
		game->currentTrack->checkpointLayer->draw(&screen, Rect(0, 0, maxX, maxY), level_line_interrupt_callback);
	}

}

void DrawAnimations()
{
	screen.sprites = lightSprites;
	for (auto animation : game->animations)
	{
		if (animation.size() > 0)
		{
			screen.sprite(animation.at(0).spriteLocation, animation.at(0).drawLocation);
		}
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

#define NUM_PALETTES 4

Pen alternate_palettes[NUM_PALETTES][5] = {
	
	{
		Pen(106, 190, 48),
		Pen(153, 229, 80),
		Pen(0, 0, 0),
		Pen(255, 255, 255),
		Pen(203, 219, 252)
	},
	{
		Pen(12, 46, 171),
		Pen(84, 116, 156),
		Pen(0, 0, 0),
		Pen(237, 237, 0),
		Pen(128, 140, 84)
	},
	{
		Pen(255, 0, 0),
		Pen(148, 54, 61),
		Pen(0, 0, 0),
		Pen(242, 182, 61),
		Pen(247, 206, 123)
	},
	{
		Pen(0, 255, 0),
		Pen(85, 53, 148),
		Pen(0, 0, 0),
		Pen(242, 61, 226),
		Pen(247, 123, 236)
	}
};

void DrawCar(Actor* car)
{
	screen.sprites = game->currentTrack->carSpriteSheet;

	auto sprite = car->degrees;

	float remainder = fmod(sprite, spriteRotationalSegmentSize);

	if (remainder > 0)
	{
		if (remainder < spriteRotationalSegmentSize / 2)
		{
			sprite -= remainder;
		}
		else
		{
			sprite += (spriteRotationalSegmentSize - remainder);
		}
	}

	for (int x = 0; x < 5; x++) {
		screen.sprites->palette[1 + x] = alternate_palettes[car->carNumber][x];
	}

	if (car->isPlayer)
	{
		screen.sprite(car->sprites[sprite], Point(maxX / 2 - (car->size.w / 2), maxY / 2 - (car->size.h / 2)));
	}
	else
	{
		screen.sprite(car->sprites[sprite], worldToScreen(Point(car->x, car->y), game->PlayerCar->camera));
		//screen.text(std::to_string(car->position), outline_font, worldToScreen(Point(car->x + car->size.w, car->y), game->PlayerCar->camera));
	}

}

void DrawGame()
{
	DrawWorld();

	//screen.sprite(PlayerCar->spriteLocation, Point(PlayerCar->x, PlayerCar->y));

	for (auto car : game->cars)
	{
		DrawCar(car);
	}
	
	DrawAnimations();

	//screen.text(std::to_string(game->PlayerCar->position), minimal_font, Point(0, 0));

	

	if(debugMode)
	{
		const std::string curLapText = GetLapTimeString(game->PlayerCar->lapTime);

		auto backgroundSize = screen.measure_text(curLapText, minimal_font);

		backgroundSize.w = 45;

		screen.pen = Pen(0, 0, 0, 128);
		screen.rectangle(Rect(Point(0, 0), backgroundSize));

		screen.pen = Pen(255, 0, 0);
		screen.text(curLapText, minimal_font, Point(0, 0));
		uint32_t lapNumber = 1;

		for (auto completed_lap_time : game->PlayerCar->completedLapTimes)
		{
			auto lapTimeString = "Lap:" + std::to_string(lapNumber) + " " + GetLapTimeString(completed_lap_time);
			backgroundSize = screen.measure_text(lapTimeString, minimal_font);
			screen.pen = Pen(0, 0, 0, 128);
			screen.rectangle(Rect(Point(0, lapNumber * 10), backgroundSize));
			screen.pen = Pen(255, 0, 0);
			screen.text(lapTimeString, minimal_font, Point(0, lapNumber * 10));
			lapNumber++;
		}

		//screen.text("Vec X: " + std::to_string(game->PlayerCar->movement.x), minimal_font, Point(0, 0));
		//screen.text("Vec Y: " + std::to_string(game->PlayerCar->movement.y), minimal_font, Point(0, 10));
		//screen.text("V: " + std::to_string(PlayerCar->speedMultiplier), minimal_font, Point(0, 20));


		//screen.text("d " + std::to_string(game->cpuCars[0]->degrees), minimal_font, Point(0, 30));
		//screen.text("X: " + std::to_string(int(game->cpuCars[0]->x)), minimal_font, Point(0, 40));
		//screen.text("Y: " + std::to_string(int(game->cpuCars[0]->y)), minimal_font, Point(0, 50));
		//screen.text(std::to_string(game->currentTrack->nodes[game->PlayerCar->targetNode].x), minimal_font, Point(0, 60));
		//screen.text(std::to_string(game->currentTrack->nodes[game->PlayerCar->targetNode].y), minimal_font, Point(0, 70));
		//screen.text(std::to_string(game->currentTrack->nodes[game->cpuCars[0]->targetNode].x), minimal_font, Point(0, 60));
		//screen.text(std::to_string(game->currentTrack->nodes[game->cpuCars[0]->targetNode].y), minimal_font, Point(0, 70));
		//screen.text(std::to_string(debugAngle), minimal_font, Point(0, 90));

		for (auto car : game->cars)
		{
			if (!car->isPlayer)
			{
				screen.line(worldToScreen(Point(static_cast<int>(game->cars[0]->x + game->cars[0]->size.w / 2), static_cast<int>(game->cars[0]->y + game->cars[0]->size.h / 2)), game->PlayerCar->camera), worldToScreen(game->currentTrack->nodes[game->cars[0]->targetNode], game->PlayerCar->camera));
				
				screen.rectangle(Rect(worldToScreen(Point(car->x, car->y), game->PlayerCar->camera), car->size));

			}

			screen.pen = Pen(0, 255, 0, 50);

			screen.rectangle(Rect(worldToScreen(Point(car->x, car->y), game->PlayerCar->camera), car->size));
		}

		for (auto node : game->currentTrack->nodes)
		{
			screen.pixel(worldToScreen(node, game->PlayerCar->camera));
		}

		//std::string ids;

		/*int i = 0;
		
		for (auto tile : game->PlayerCar->currentTileData.tilesScanned)
		{
			i++;
			ids.append(std::to_string(tile.second.id) + " (" + std::to_string(tile.second.detectionCount) + ") ");
			if(i == 3)
			{
				ids.append("\n");
				i = 0;
			}
		}*/

		//screen.text("tiles: " + ids, minimal_font, Point(0, 80));

		/*for (auto pointChecked : game->PlayerCar->currentTileData.pointsChecked)
		{		
			screen.pen = Pen(0, 255, 0, 50);
			screen.pixel(worldToScreen(pointChecked, game->PlayerCar->camera));
		}*/

		screen.pen = Pen(255, 0, 0);

	}

	
}

void DrawGameOver()
{
	std::string placeSuffix;

	switch (game->PlayerCar->position)
	{
	case 1:
		placeSuffix = "st";
		break;
	case 2:
		placeSuffix = "nd";
		break;
	case 3:
		placeSuffix = "rd";
		break;
	case 4:
		placeSuffix = "th";
		break;
	}

	screen.pen = Pen(255, 255, 255, 255);
	screen.text("You came " + std::to_string(game->PlayerCar->position) + placeSuffix, outline_font, Point(maxX / 2, 10), true, center_h);

	uint8_t lapHeight = 40;
	uint8_t lapNumber = 1;
	for (auto lap : game->PlayerCar->completedLapTimes)
	{
		screen.text("Lap " + std::to_string(lapNumber) + "   " + GetLapTimeString(lap), outline_font, Point(maxX / 2, lapHeight), true, center_h);
		lapNumber++;
		lapHeight += 20;
	}
}

///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game-> time is the 
// amount if milliseconds elapsed since the start of your game
//
void render(uint32_t time) {
	screen.pen = Pen(0, 0, 0, 255);
	//screen.mask = nullptr;

	// clear the screen -- screen is a reference to the frame buffer and can be used to draw all things with the 32blit
	screen.clear();


	switch (state)
	{
	case MainMenu:
		DrawMenu();
		break;
	case LevelSelect:
		DrawLevelSelect();
		break;
	case Play:
		DrawGame();
		break;
	case GameOver:
		DrawGameOver();
		break;
	}
}

bool checkCarCollisions(Actor* car)
{
	auto collisionAdjustment = 0; //car->size.w / 4;
	auto collision = false;

	for (auto car2 : game->cars)
	{
		if (car2 != car)
		{
			if ((car2->x + collisionAdjustment <= car->x + (car->size.w - collisionAdjustment))
				&& (car2->x + collisionAdjustment >= car->x + collisionAdjustment)
				&& (car2->y + collisionAdjustment <= car->y + (car->size.h - collisionAdjustment))
				&& (car2->y + collisionAdjustment >= car->y + collisionAdjustment))
			{
				auto collisionVector = Point(car->x, car->y) - Point(car2->x, car2->y);
				car->x += collisionVector.x * 0.2;
				car->y += collisionVector.y * 0.2;
				collision = true;
				//car->speedMultiplier = 0;
			}
		}
	}

	return collision;
}

void ApplyCarMovement(float radian, Vec2 newVector, Actor* car)
{
	car->movement.x = car->movement.x * friction;
	car->movement.y = car->movement.y * friction;

	//car->movement.rotate(radian);
			
	car->movement.x += newVector.x;
	car->movement.y += newVector.y;
			
	auto collision = checkCarCollisions(car);

	if (!collision)
	{
		car->x += car->movement.x;
		car->y += car->movement.y;
	}
	

	newVector.rotate(radian * -1);
}

void updateCar(Actor* car)
{
	car->lapTime += 10;

	if (car->inputDelay > 0)
	{
		car->inputDelay--;
	}

	// Get scan data up front so AI can decide what to do
	car->currentTileData = getLocalTileData(Rect(car->x - (car->size.w / 2), car->y - (car->size.h / 2), car->size.w, car->size.h), tileSize, tilemap_width, game->currentTrack->objectsLayer->tiles, Collision);

	
		bool accelerate = false;
		bool left = false;
		bool right = false;

		if (car->moveEnabled)
		{
			if (car->isPlayer)
			{
				if (buttons & Button::A)
				{
					accelerate = true;
				}

				if (buttons & Button::DPAD_RIGHT || joystick.x > 0)
				{
					right = true;
				}
				else if (buttons & Button::DPAD_LEFT || joystick.x < 0)
				{
					left = true;
				}
			}
			else
			{
				auto currentTarget = game->currentTrack->nodes[car->targetNode];

				auto carCenter = Point(car->x, car->y);


				CalculateDistance(carCenter, currentTarget);

				float targetAngle = calcAngleBetweenPoints(carCenter, game->currentTrack->nodes[car->targetNode]);

				////targetAngle = targetAngle * 180 / pi;
				//targetAngle = targetAngle * -1;

				//
				//if(targetAngle < 0)
				//{
				//	targetAngle = targetAngle * -1 * 2;
				//}


				debugAngle = targetAngle;

				Vec2 oldVector = Vec2(0, 1);
				oldVector.rotate((pi * car->degrees) / 180.00f);
				Vec2 targetVector = Vec2(0, 1);
				targetVector.rotate((pi * targetAngle) / 180.00f);

				auto cross = targetVector.cross(oldVector);

				if (cross > 0)
				{
					left = true;
				}
				else if (cross < 0)
				{
					right = true;
				}

				auto angleAccuracy = 50;

				if (car->speedMultiplier < 1.4 || (car->degrees >= targetAngle - angleAccuracy && car->degrees <= targetAngle + angleAccuracy))
				{
					accelerate = true;
				}

				auto nodeAccuracy = 40;

				if (static_cast<int>(car->x) >= currentTarget.x - nodeAccuracy && static_cast<int>(car->x) <= currentTarget.x + nodeAccuracy && static_cast<int>(car->y) >= currentTarget.y - nodeAccuracy && static_cast<int>(car->y) <= currentTarget.y + nodeAccuracy)
				{
					if (car->targetNode < game->currentTrack->nodes.size() - 1)
					{
						car->targetNode++;
					}
					else
					{
						car->targetNode = 0;
					}
				}

			}
		}
		if (accelerate && car->speedMultiplier < maxSpeed)
		{
			car->speedMultiplier += acceleration;
			car->speedMultiplier = std::min(maxSpeed, car->speedMultiplier);  // NOLINT(clang-diagnostic-implicit-float-conversion)
		}
		else if (car->speedMultiplier > 0 && !accelerate)
		{
			car->speedMultiplier -= slowdown;  // NOLINT(clang-diagnostic-implicit-float-conversion)
			car->speedMultiplier = std::max(0.0f, car->speedMultiplier);
		}

		if (car->inputDelay == 0)
		{
			if (right) {
				car->inputDelay = round(MapRange(0, maxSpeed, car->speedMultiplier, car->STEERINGDELAYMIN, car->STEERINGDELAYMAX));

				if (car->degrees == 360.0f)
				{
					car->degrees = rotationIncrement;
				}
				else
				{
					car->degrees += rotationIncrement;
				}
			}
			else if (left) {
				car->inputDelay = round(MapRange(0, maxSpeed, car->speedMultiplier, car->STEERINGDELAYMIN, car->STEERINGDELAYMAX));

				if (car->degrees == 0.0f)
				{
					car->degrees = 360 - rotationIncrement;
				}
				else
				{
					car->degrees -= rotationIncrement;
				}
			}
		}

		float radian = (pi * car->degrees) / 180.00f;

		Vec2 newVector = DegreesToVector(car->degrees);
			Vec2(0, 1);
		
		newVector.x = newVector.x * car->speedMultiplier;
		newVector.y = newVector.y * car->speedMultiplier;

		if (car->currentTileData.obstruction)
		{
			auto collisionVector = Point(car->x, car->y) - car->currentTileData.collisionLocation;
			car->x += collisionVector.x;
			car->y += collisionVector.y;
		}

		ApplyCarMovement(radian, newVector, car);

		auto checkPointScan = getLocalTileData(Rect(car->x - (car->size.w / 2), car->y - (car->size.h / 2), car->size.w, car->size.h), tileSize, tilemap_width, game->currentTrack->checkpointLayer->tiles, Checkpoint);

		for (auto tiles_scanned : checkPointScan.tilesScanned)
		{
			if (tiles_scanned.second.id == car->currentCheckpoint + 1)
			{
				car->currentCheckpoint++;
			}
			else if (car->currentCheckpoint == game->currentTrack->checkpointCount && tiles_scanned.second.id == 1)
			{
				car->currentCheckpoint = 1;
				car->completedLapTimes.emplace_back(car->lapTime);
				car->lapTime = 0;
				captureNodes = false;

				if(car->completedLapTimes.size() >= game->currentTrack->laps)
				{
					car->moveEnabled = false;
					car->position = game->place;
					game->place++;
					if (car->isPlayer)
					{
						buttonBounceTimer = 50;
						state = GameOver;
					}	
				}
			}
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

	if (buttonBounceTimer > 0)
	{
		buttonBounceTimer--;
	}
	
	millisElapsed += 10;

	if(millisElapsed >= 500 && captureNodes)
	{
		millisElapsed = 0;
		nodes.emplace_back(Point(game->PlayerCar->x, game->PlayerCar->y));
	}

	switch (state)
	{
	case MainMenu:
		if (buttons & Button::A && buttonBounceTimer <= 0)
		{
			delete game;

			game = new Game();
			state = LevelSelect;
			buttonBounceTimer = 20;
		}
		break;
	case LevelSelect:
		if (buttons & Button::A && buttonBounceTimer <= 0)
		{
			game->initRace();
			state = Play;
		}
		break;
	case Play:
		if(buttons & Button::X && buttonBounceTimer <= 0)
		{
			debugMode = !debugMode;
			buttonBounceTimer = 20;
		}

		if (buttons & Button::Y && buttonBounceTimer <= 0)
		{
			state = MainMenu;
			buttonBounceTimer = 20;
		}

		if (game->raceStarted)
		{
			std::sort(game->cars.begin(), game->cars.end(), Actor::SortByPosition);

			uint8_t position = 1;

			for (auto car : game->cars)
			{
				car->position = position;
				updateCar(car);
				position++;
			}
		}

		game->UpdateAnimations();

		update_camera(game->PlayerCar);
		break;
	case GameOver:
		if (buttons & Button::A && buttonBounceTimer <= 0)
		{
			state = MainMenu;
			buttonBounceTimer = 20;
		}
		break;
	}

	lastButtons = blit::buttons;
}