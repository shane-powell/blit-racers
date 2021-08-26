#pragma once
#include <32blit.hpp>
#include "position.hpp"
#include "assets.hpp"

const uint8_t TrackCount = 2;

class Track
{
public:
	uint8_t checkpointCount = 0;
	const uint8_t* mapTiles{};
	const uint8_t* mapSpiteSheet{};
	blit::Surface* carSpriteSheet{};
	uint32_t tileMapHeight = NULL;
	uint32_t tileMapWidth = NULL;
	blit::TileMap* world{};
	blit::TileMap* objectsLayer{};
	blit::TileMap* checkpointLayer{};
	blit::Surface* image;
	std::string title;

	std::vector<Position> startLocations;

	// todo split linked list for multiple paths
	std::vector<blit::Point> nodes;

	std::vector<blit::Point> checkPointLocations;

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
		std::vector<blit::Point> nodes,
		uint8_t* image,
		std::string title,
		const uint8_t* carSpriteSheet,
		std::vector<blit::Point> checkPointLocations,
		uint8_t laps = 3)
	{
		this->checkpointCount = checkpointCount;
		this->mapTiles = mapTiles;
		this->mapSpiteSheet = mapSpiteSheet;
		this->tileMapHeight = tileMapHeight;
		this->tileMapWidth = tileMapWidth;
		this->startLocations = std::move(startLocations);
		this->nodes = std::move(nodes);
		this->image = blit::Surface::load(image);
		this->title = std::move(title);
		this->laps = laps;
		this->carSpriteSheet = blit::Surface::load(carSpriteSheet);
		this->checkPointLocations = checkPointLocations;
		world = new blit::TileMap(const_cast<uint8_t*>(map1), nullptr, blit::Size(tileMapWidth, tileMapHeight), nullptr);

		auto objectLayerStart = map1 + tileMapHeight * tileMapWidth;
		objectsLayer = new blit::TileMap(const_cast<uint8_t*>(objectLayerStart), nullptr, blit::Size(tileMapWidth, tileMapHeight), nullptr);
		objectsLayer->empty_tile_id = 0;

		auto checkpointLayerStart = map1 + ((tileMapHeight * tileMapWidth) * 2);
		checkpointLayer = new blit::TileMap(const_cast<uint8_t*>(checkpointLayerStart), nullptr, blit::Size(tileMapWidth, tileMapHeight), nullptr);

		world->sprites = blit::Surface::load(worldLayerSheet);
		objectsLayer->sprites = blit::Surface::load(objectsLayerSheet);
		checkpointLayer->sprites = blit::Surface::load(checkpointLayerSheet);
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

Track* LoadTrack(uint8_t trackId);
