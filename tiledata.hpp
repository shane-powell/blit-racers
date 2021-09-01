#pragma once

struct TileData
{
	uint8_t id = 0;
	uint16_t index = 0;
	uint16_t detectionCount = 1;
	bool water = false;
	bool fall = false;
	bool obstruction = false;
	blit::Point detectionLocation;

	constexpr TileData(uint8_t id, uint16_t index, blit::Point detectionLocation) : id(id), index(index), detectionLocation(detectionLocation) {}
	constexpr TileData(uint8_t id, bool water, bool obstruction, bool fall) : id(id), water(water), obstruction(obstruction), fall(fall) {}
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