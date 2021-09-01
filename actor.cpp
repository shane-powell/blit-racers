#include "actor.hpp"

void Actor::GenerateSpriteMap(float angle)
{
	int32_t x = 0;
	int32_t y = 0;

	for (int8_t i = 0; i < 24; i++)
	{
		sprites.emplace(angle, blit::Rect(x, y, this->size.w / 8, this->size.h / 8));

		if (x < this->size.w / 8 * 5)
		{
			x += this->size.w / 8;
		}
		else
		{
			x = 0;
			y += this->size.h / 8;
		}

		if (angle == 360)
		{
			angle = 15;
			sprites.emplace(0, blit::Rect(x, y, this->size.w / 8, this->size.h / 8));
		}
		else
		{
			angle += 15;
		}
	}
}

void Actor::ProcessTileData(Track* currentTrack)
{
	for (auto const& tileType : this->currentTileData.tilesScanned) {

		uint16_t fallCount = 0;

		for (auto const& trackTile : currentTrack->activeTiles)
		{
			if (tileType.first == trackTile.id)
			{
				if (trackTile.obstruction)
				{
					this->currentTileData.obstruction = true;
					this->currentTileData.collisionLocation = tileType.second.detectionLocation;
				}

				if (trackTile.fall)
				{
					fallCount += trackTile.detectionCount;
				}
			}
		}

		if (fallCount > this->currentTileData.areaSize * 0.75)
		{
			
		}
	}
}

void Actor::SetLocation(Position gridPosition)
{
	this->x = gridPosition.location.x;
	this->y = gridPosition.location.y;
	this->degrees = gridPosition.angle;
}

bool Actor::SortByPosition(const Actor* carA, const Actor* carB) {

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
			auto distanceA = (int)CalculateDistance(carA->getNextTargetCheckpoint(carA->currentCheckpoint), blit::Point(carA->x, carA->y));

			auto distanceB = (int)CalculateDistance(carB->getNextTargetCheckpoint(carB->currentCheckpoint), blit::Point(carB->x, carB->y));

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