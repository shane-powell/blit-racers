#include "game.hpp"

void Game::ChangeCurrentTrack()
{
	delete this->currentTrack;
	this->currentTrack = LoadTrack(this->currentTrackId);
}

void Game::initRace() {

	uint8_t gridPosition = 0;

	std::function<Position& (uint8_t currentCheckpoint)> getNextTargetCheckpoint = std::function([&](uint8_t currentCheckpoint) -> Position&
		{
			if (currentCheckpoint < this->currentTrack->checkPointLocations.size())
			{
				return this->currentTrack->checkPointLocations[currentCheckpoint];
			}

			return this->currentTrack->checkPointLocations[0];
		});

	PlayerCar = new Actor(currentTrack->startLocations[gridPosition], Rect(0, 0, this->currentTrack->vehicleSize.w / 8, this->currentTrack->vehicleSize.h / 8), this->currentTrack->vehicleSize, 0, gridPosition, getNextTargetCheckpoint, this->currentTrack->vehicleType, true);
	ActiveCar = PlayerCar;
	gridPosition++;
	PlayerCar->camera = Vec2(PlayerCar->x + (PlayerCar->size.w / 2), PlayerCar->y + (PlayerCar->size.h / 2));

	cars.emplace_back(PlayerCar);
	carPositions.emplace_back(PlayerCar);

	for (int i = 0; i < aiCount; ++i)
	{
		if (currentTrack->startLocations.size() >= gridPosition + 1)
		{
			auto aiCar = new Actor(currentTrack->startLocations[gridPosition], Rect(0, 0, this->currentTrack->vehicleSize.w / 8, this->currentTrack->vehicleSize.h / 8), this->currentTrack->vehicleSize, 0, gridPosition, getNextTargetCheckpoint, this->currentTrack->vehicleType);

			switch (difficulty)
			{
			case Difficulty::Easy:
				aiCar->maxSpeed *= 0.5;
				break;
			case Difficulty::Medium:
				aiCar->maxSpeed *= 0.8;
				break;
			}

			cars.emplace_back(aiCar);
			carPositions.emplace_back(aiCar);
			gridPosition++;
		}
	}

	std::vector<Animation> startLightOneFrames;

	blit::Point lightsStart = Point(maxX / 2 - 36, 0);

	startLightOneFrames.emplace_back(Animation(Rect(0, 0, 3, 3), lightsStart, 100));
	startLightOneFrames.emplace_back(Animation(Rect(3, 0, 3, 3), lightsStart, 300, std::function([&]() {raceStarted = true; })));
	startLightOneFrames.emplace_back(Animation(Rect(0, 3, 3, 3), lightsStart, 100));

	animations.emplace_back(startLightOneFrames);

	std::vector<Animation> startLightTwoFrames;

	startLightTwoFrames.emplace_back(Animation(Rect(0, 0, 3, 3), Point(lightsStart.x + 24, 0), 200));
	startLightTwoFrames.emplace_back(Animation(Rect(3, 0, 3, 3), Point(lightsStart.x + 24, 0), 200));
	startLightTwoFrames.emplace_back(Animation(Rect(0, 3, 3, 3), Point(lightsStart.x + 24, 0), 100));

	animations.emplace_back(startLightTwoFrames);

	std::vector<Animation> startLightThreeFrames;

	startLightThreeFrames.emplace_back(Animation(Rect(0, 0, 3, 3), Point(lightsStart.x + 48, 0), 300));
	startLightThreeFrames.emplace_back(Animation(Rect(3, 0, 3, 3), Point(lightsStart.x + 48, 0), 100));
	startLightThreeFrames.emplace_back(Animation(Rect(0, 3, 3, 3), Point(lightsStart.x + 48, 0), 100));

	animations.emplace_back(startLightThreeFrames);

}