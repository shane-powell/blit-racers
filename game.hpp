#pragma once
#include "32blit.hpp"
#include "animation.hpp"
#include "position.hpp"
#include "track.hpp"
#include "actor.hpp"

enum Difficulty {
	Easy,
	Medium,
	Hard
};

class Game
{
public:

	int8_t aiOffset = 0;

	int8_t vibrationTimer = 0;

	uint8_t currentTrackId = 0;
	Track* currentTrack;

	std::vector<Actor*> cars;
	std::vector<Actor*> carPositions;

	Actor* PlayerCar;

	Actor* ActiveCar;
	uint8_t activeCarId = 0;

	uint8_t aiCount = 3;

	uint8_t place = 1;

	int32_t maxX = 0;

	bool raceStarted = false;

	std::vector<std::vector<Animation>> animations;

	Difficulty difficulty = Difficulty::Medium;

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

	void initRace();

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