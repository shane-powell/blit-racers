#pragma once
#include <32blit.hpp>
#include "tiledata.hpp"
#include "calculations.hpp"

class Actor {
public:
	virtual ~Actor() = default;
	blit::Size size;
	Rect spriteLocation;
	float x = 0.0f;
	float y = 0.0f;
	blit::Vec2 camera;
	float degrees = 270.0f;
	std::map<float, Rect> sprites;
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

	std::function<Point& (uint8_t currentCheckpoint)> getNextTargetCheckpoint;

	Actor() = default;

	Actor(float xIn, float yIn)
	{
		this->x = xIn;
		this->y = yIn;

		movement = Vec2(0, 1);
		this->GenerateSpriteMap(180);
	}

	Actor(Position gridPosition, Rect spriteLocation, Size size, uint8_t targetNode, uint8_t carNumber, std::function<Point& (uint8_t currentCheckpoint)> getNextTargetCheckpoint, bool isPlayer = false)
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

	static bool SortByPosition(const Actor* carA, const Actor* carB) {

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