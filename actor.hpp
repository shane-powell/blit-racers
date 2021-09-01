#pragma once
#include <32blit.hpp>
#include "tiledata.hpp"
#include "util.hpp"
#include "track.hpp"

class Actor {
public:
	virtual ~Actor() = default;
	blit::Size size;
	blit::Rect spriteLocation;
	float x = 0.0f;
	float y = 0.0f;
	blit::Vec2 camera;
	float degrees = 270.0f;
	std::map<float, blit::Rect> sprites;
	uint8_t inputDelay = 0;
	bool moveEnabled = true;
	const uint8_t STEERINGDELAYMIN = 3;
	const uint8_t STEERINGDELAYMAX = 5;

	float prevX = 0.0f;
	float prevY = 0.0f;

	blit::Vec2 movement;

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

	std::function<blit::Point& (uint8_t currentCheckpoint)> getNextTargetCheckpoint;

	Actor() = default;

	Actor(float xIn, float yIn)
	{
		this->x = xIn;
		this->y = yIn;

		movement = blit::Vec2(0, 1);
		this->GenerateSpriteMap(180);
	}

	Actor(Position gridPosition, blit::Rect spriteLocation, blit::Size size, uint8_t targetNode, uint8_t carNumber, std::function<blit::Point& (uint8_t currentCheckpoint)> getNextTargetCheckpoint, bool isPlayer = false)
	{
		this->SetLocation(gridPosition);
		this->spriteLocation = spriteLocation;
		this->size = size;
		this->isPlayer = isPlayer;
		this->targetNode = targetNode;
		movement = blit::Vec2(0, 1);
		this->GenerateSpriteMap(180);
		this->carNumber = carNumber;
		this->getNextTargetCheckpoint = getNextTargetCheckpoint;
	}

	void GenerateSpriteMap(float angle);

	void ProcessTileData(Track* currentTrack);

	void SetLocation(Position gridPosition);

	static bool SortByPosition(const Actor* carA, const Actor* carB);
};