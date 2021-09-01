#include "blit-racers.hpp"
#include "assets.hpp"
#include <limits>
#include <algorithm>
#include <iostream>
#include <map>
#include <utility>
#include "animation-frame.hpp"
#include "position.hpp"
#include "track.hpp"
#include "actor.hpp"
#include "game.hpp"
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

Surface* titleSprite = Surface::load(title);


const uint32_t tilemap_width = 128;

const uint32_t tilemap_height = 128;



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

// todo replace point and sprite width/height with rect
TileScanData getLocalTileData(const Rect& boundingBox, uint8_t tile_size, uint8_t tileMapWidth, uint8_t* mapLayer, TileScanType scanType) {
	TileScanData tileScanData;
	//TileScanData tileScanData(boundingBox.w * boundingBox.h);
	tileScanData.areaSize = boundingBox.w * boundingBox.h;

	for (auto y = 0; y < boundingBox.h; y++) {
		for (auto x = 0; x < boundingBox.w; x++) {
			auto pointToCheck = Point(boundingBox.x + x, boundingBox.y + y);

			//tileScanData.pointsChecked.emplace_back(pointToCheck);

			const auto array_location = getTileFromPoint(pointToCheck,
				tile_size,
				tileMapWidth);
			const uint8_t tileScanned = *(mapLayer + array_location); //map1[array_location + tilemap_height * tilemap_width]; //map1[array_location];  //*(mapLayer + array_location); //mapLayer[array_location];

			TileData tileData = TileData(tileScanned, array_location, pointToCheck);

			if (auto it{ tileScanData.tilesScanned.find(tileData.id) }; it != std::end(tileScanData.tilesScanned))
			{
				it->second.detectionCount += 1;
			}
			else
			{
				
				tileScanData.tilesScanned.emplace(tileData.id, tileData);
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

	car->camera.x = car->x + (car->size.w / 2);
	car->camera.y = car->y + (car->size.h / 2);

	camera = Mat3::identity();
	camera *= Mat3::translation(Vec2(car->x, car->y)); // offset to middle of world
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
screen.sprites = titleSprite;


#ifdef DISPLAY_ST7789
	screen.sprite(Rect(0, 0, 120 / 8, 120 / 8), Point(0, 0));
	screen.text("Press Button A\nto start.", minimal_font, Point(maxX / 2, maxY - 20), true, center_h);
#else
	screen.sprite(Rect(0, 0, 120 / 8, 120 / 8), Point(20, 0));
	screen.text("Press Button A to start.", outline_font, Point(maxX / 2, maxY - 10), true, center_h);
#endif
}

void DrawLevelSelect()
{
	screen.pen = Pen(255, 255, 255, 255);
	screen.text("Select a track", outline_font, Point(maxX / 2, minY + 10), true, center_h);
	screen.sprites = game->currentTrack->image;

#ifdef DISPLAY_ST7789
	screen.sprite(Rect(0, 0, 80 / 8, 60 / 8), Point(20, maxY / 4));
#else
	screen.sprite(Rect(0, 0, 80 / 8, 60 / 8), Point(maxX / 4, maxY / 4));
#endif

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


		screen.sprite(car->sprites[sprite], worldToScreen(Point(car->x, car->y), game->PlayerCar->camera));
		//screen.text(std::to_string(car->position), outline_font, worldToScreen(Point(car->x + car->size.w, car->y), game->PlayerCar->camera));

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

			screen.pen = blit::Pen(0, 255, 0, 50);

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
	auto collisionAdjustment = car->size.w / 4;
	auto collision = false;

	float newX = car->x + car->movement.x;
	float newY = car->y + car->movement.y;

	for (auto car2 : game->cars)
	{
		if (car2 != car)
		{
			if (IsRectIntersecting(blit::Rect(blit::Point(car->x, car->y), car->size), blit::Rect(blit::Point(car2->x, car2->y), car2->size)))
			{
				auto collisionVector = blit::Point(newX, newY) - blit::Point(car2->x, car2->y);
				car->x += (collisionVector.x * 0.2);
				car->y += (collisionVector.y * 0.2);
				collision = true;
			}
		}
	}

	return collision;
}

void ApplyCarMovement(float radian, blit::Vec2 newVector, Actor* car)
{
	car->movement.x = car->movement.x * friction;
	car->movement.y = car->movement.y * friction;

	//todo This needs to be removed at somepoint as I was accidently applying the rotation twice but removing makes ai worse.
	car->movement.rotate(radian);
			
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
	car->ProcessTileData(game->currentTrack);
	
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
			car->speedMultiplier = car->speedMultiplier / 2;
			auto collisionVector = Point(car->x, car->y) - car->currentTileData.collisionLocation;
			//collisionVector *= car->speedMultiplier;
			//car->x += collisionVector.x;
			//car->y += collisionVector.y;
			newVector.x += collisionVector.x * car->speedMultiplier;
			newVector.y += collisionVector.y * car->speedMultiplier;
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

			game = new Game(maxX);
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
		else if ((buttons & Button::DPAD_LEFT && buttonBounceTimer <= 0) || joystick.x > 0)
		{
			buttonBounceTimer = 20;
			if (game->currentTrackId >= TrackCount - 1)
			{
				game->currentTrackId--;
			}
			else
			{
				game->currentTrackId = TrackCount - 1;
			}

			game->ChangeCurrentTrack();
		}
		else if ((buttons & Button::DPAD_RIGHT && buttonBounceTimer <= 0) || joystick.x < 0)
		{
			buttonBounceTimer = 20;
			if (game->currentTrackId < TrackCount - 1)
			{
				game->currentTrackId++;
			}
			else
			{
				game->currentTrackId = 0;
			}

			game->ChangeCurrentTrack();
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
