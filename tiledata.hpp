#pragma once
#include "32blit.hpp"

enum TileScanType
{
	Collision,
	CheckpointScan
};

struct TileData
{
	uint8_t id = 0;
	uint16_t index = 0;
	uint16_t detectionCount = 1;
	bool water = false;
	bool fall = false;
	bool obstruction = false;
	bool jump = false;
	blit::Point detectionLocation;

	constexpr TileData(uint8_t id, uint16_t index, blit::Point detectionLocation) : id(id), index(index), detectionLocation(detectionLocation) {}
	constexpr TileData(uint8_t id, bool water, bool obstruction, bool fall) : id(id), water(water), obstruction(obstruction), fall(fall) {}

	constexpr TileData(uint8_t id, bool water, bool obstruction, bool fall, bool jump) : id(id), water(water), obstruction(obstruction), fall(fall), jump(jump) {}
};

struct TileScanData
{
	uint16_t areaSize = 0;
	bool obstruction = false;
	std::map<uint8_t, TileData> tilesScanned;
	blit::Point collisionLocation;

	TileScanData() = default;
	//constexpr TileScanData(uint16_t areaSize) : areaSize(areaSize) {};
};

uint16_t getTileFromPoint(const blit::Point& point, uint8_t tile_size, uint8_t tile_map_width);

TileScanData getLocalTileData(const blit::Rect& boundingBox, uint8_t tile_size, uint8_t tileMapWidth, uint8_t* mapLayer, TileScanType scanType);