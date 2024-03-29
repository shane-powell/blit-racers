#pragma once
#include <32blit.hpp>
#include "position.hpp"
#include "assets.hpp"
#include "tiledata.hpp"
#include "util.hpp"

const uint8_t TrackCount = 2;

class Track
{
public:
	uint8_t checkpointCount = 0;
	const uint8_t* mapTiles{};
	blit::Surface* carSpriteSheet{};
	uint32_t tileMapHeight = NULL;
	uint32_t tileMapWidth = NULL;
	blit::TileMap* world{};
	blit::TileMap* objectsLayer{};
	blit::TileMap* checkpointLayer{};
	blit::Surface* image;
	std::string title;
	blit::Size vehicleSize;

	std::vector<Position> startLocations;

	// todo split linked list for multiple paths
	std::vector<blit::Point> nodes;

	std::vector<Position> checkPointLocations;

	std::vector<TileData> activeTiles;

	uint8_t laps = 3;

	util::Vehicle vehicleType;

	Track(const uint8_t checkpointCount,
		const uint8_t* mapTiles,
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
		std::vector<Position> checkPointLocations,
		blit::Size vehicleSize,
		std::vector<TileData> activeTiles,
		util::Vehicle vehicleType,
		uint8_t laps = 3)
	{
		this->checkpointCount = checkpointCount;
		this->mapTiles = mapTiles;
		this->tileMapHeight = tileMapHeight;
		this->tileMapWidth = tileMapWidth;
		this->startLocations = std::move(startLocations);
		this->nodes = std::move(nodes);
		this->image = blit::Surface::load(image);
		this->title = std::move(title);
		this->laps = laps;
		this->carSpriteSheet = blit::Surface::load(carSpriteSheet);
		this->checkPointLocations = checkPointLocations;
		this->vehicleSize = vehicleSize;
		this->activeTiles = activeTiles;
		this->vehicleType = vehicleType;

		world = new blit::TileMap(const_cast<uint8_t*>(mapTiles), nullptr, blit::Size(tileMapWidth, tileMapHeight), nullptr);

		auto objectLayerStart = mapTiles + tileMapHeight * tileMapWidth;
		objectsLayer = new blit::TileMap(const_cast<uint8_t*>(objectLayerStart), nullptr, blit::Size(tileMapWidth, tileMapHeight), nullptr);
		objectsLayer->empty_tile_id = 0;

		auto checkpointLayerStart = mapTiles + ((tileMapHeight * tileMapWidth) * 2);
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
