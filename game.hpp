#pragma once
#include "32blit.hpp"
#include "animation-frame.hpp"
#include "position.hpp"
#include "track.hpp"
#include "actor.hpp"

class Game
{
public:

	int8_t aiOffset = 0;

	int8_t vibrationTimer = 0;

	uint8_t currentTrackId = 0;
	Track* currentTrack;

	std::vector<Actor*> cars;

	Actor* PlayerCar;

	uint8_t aiCount = 3;

	uint8_t place = 1;

	int32_t maxX = 0;

	bool raceStarted = false;

	std::vector<std::vector<AnimationFrame>> animations;

	Game(int32_t maxX)
	{
		this->maxX = maxX;
		// Load first track
		currentTrack = LoadTrack(this->currentTrackId);
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

		uint8_t gridPosition = 0;

		std::function<Point& (uint8_t currentCheckpoint)> getNextTargetCheckpoint = std::function([&](uint8_t currentCheckpoint) -> Point&
			{
				if (currentCheckpoint < this->currentTrack->checkPointLocations.size())
				{
					return this->currentTrack->checkPointLocations[currentCheckpoint];
				}

				return this->currentTrack->checkPointLocations[0];
			});

		PlayerCar = new Actor(currentTrack->startLocations[gridPosition], Rect(0, 0, this->currentTrack->vehicleSize.w / 8, this->currentTrack->vehicleSize.h / 8), this->currentTrack->vehicleSize, 0, gridPosition, getNextTargetCheckpoint, true);
		gridPosition++;
		PlayerCar->camera = Vec2(PlayerCar->x + (PlayerCar->size.w / 2), PlayerCar->y + (PlayerCar->size.h / 2));

		cars.emplace_back(PlayerCar);

		for (int i = 0; i < aiCount; ++i)
		{
			if (currentTrack->startLocations.size() >= gridPosition + 1)
			{
				cars.emplace_back(new Actor(currentTrack->startLocations[gridPosition], Rect(0, 0, this->currentTrack->vehicleSize.w / 8, this->currentTrack->vehicleSize.h / 8), this->currentTrack->vehicleSize, 0, gridPosition, getNextTargetCheckpoint));
				gridPosition++;
			}
		}

		std::vector<AnimationFrame> startLightOneFrames;

		blit::Point lightsStart = Point(maxX / 2 - 36, 0);

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

	void ChangeCurrentTrack();
};