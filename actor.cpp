#include "actor.hpp"
#include "animation.hpp"

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
	uint16_t fallCount = 0;
	uint16_t jumpCount = 0;

	for (auto const& tileType : this->currentTileData.tilesScanned) {

		for (auto const& trackTile : currentTrack->activeTiles)
		{
			if (tileType.first > 0 && tileType.first == trackTile.id)
			{
				if (trackTile.obstruction)
				{
					this->currentTileData.obstruction = true;
					this->currentTileData.collisionLocation = tileType.second.detectionLocation;
				}

				if (trackTile.fall)
				{
					fallCount += tileType.second.detectionCount;
				}
				if (trackTile.jump)
				{
					jumpCount += tileType.second.detectionCount;
				}
				break;
			}
		}

		
	}

	auto detectionTrigger = this->currentTileData.areaSize * 0.75;

	if (fallCount > detectionTrigger && moveEnabled)
	{
		this->moveEnabled = false;

		animationQueue.push(new Animation(blit::Rect(this->GetPosition(), this->size), this->GetPosition(), 300, this->degrees, this->scale, 5, Vec2(-0.006, -0.006), std::function([&]() {
			Respawn();
			})));
	}
	else if (jumpCount > detectionTrigger && moveEnabled)
	{
		this->moveEnabled = false;
		this->lockSpeed = true;
		
		animationQueue.push(new Animation(blit::Rect(this->GetPosition(), this->size), this->GetPosition(), 50, this->degrees, this->scale, 0, Vec2(0.01, 0.01)));
		animationQueue.push(new Animation(blit::Rect(this->GetPosition(), this->size), this->GetPosition(), 50, this->degrees, animationQueue.front()->finalScale, 0, Vec2(-0.01, -0.01), std::function([&]() {
			this->moveEnabled = true;
			this->lockSpeed = false;
			//delete animation;
			})));
	}

	if (animationQueue.empty() && effects.empty() && vehicleType == util::Vehicle::boat)
	{
		// todo effect rotation
		//effects.emplace_back(new Animation(blit::Rect(blit::Point(0, 9), blit::Size(2,1)), this->GetPosition(), 2,2));
	}
}

blit::Point Actor::GetPosition()
{
	return blit::Point(this->x, this->y);
}

void Actor::Respawn()
{
	auto checkPoint = this->getNextTargetCheckpoint(this->currentCheckpoint);

	this->x = checkPoint.location.x;
	this->y = checkPoint.location.y;

	this->degrees = checkPoint.angle;

	this->moveEnabled = true;
}

void Actor::Rotate(float rotation)
{
	if (rotation > 0)
	{

		if (this->degrees == 360.0f)
		{
			this->degrees = rotation;
		}
		else
		{
			this->degrees += rotation;
		}
	}
	else if (rotation < 0)
	{

		if (this->degrees == 0.0f)
		{
			this->degrees = 360 + rotation;
		}
		else
		{
			this->degrees += rotation;
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
			auto distanceA = (int)util::CalculateDistance(carA->getNextTargetCheckpoint(carA->currentCheckpoint).location, blit::Point(carA->x, carA->y));

			auto distanceB = (int)util::CalculateDistance(carB->getNextTargetCheckpoint(carB->currentCheckpoint).location, blit::Point(carB->x, carB->y));

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

void Actor::Animate()
{
	if (!animationQueue.empty())
	{
		if (animationQueue.front()->finished)
		{
			auto animationToRemove = animationQueue.front();
			animationQueue.pop();
			delete animationToRemove;

			this->Animate();
		}
		else
		{
			animationQueue.front()->Animate();
		}
	}

	if (!this->effects.empty())
	{
		if (this->effects[0]->finished)
		{
			auto animationToRemove = effects[0];
			effects.clear();
			delete animationToRemove;
		}
		else
		{
			this->effects[0]->Animate();
		}
	}
}
