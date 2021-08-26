#pragma once

struct TileData
{
	uint8_t id = 0;
	uint16_t index = 0;
	uint16_t detectionCount = 1;
};

struct TileScanData
{
	uint8_t id = 0;
	uint16_t index = 0;
	bool obstruction = false;
	uint16_t pixels_in_water = 0;
	bool in_water = false;
	float movement_modifier = 0;
	float life_modifier = 0;
	//std::vector<Point> pointsChecked;
	//std::vector<uint8_t> idsFound;
	std::map<uint8_t, TileData> tilesScanned;
	blit::Point collisionLocation;
};