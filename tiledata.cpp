#include "tiledata.hpp"

uint16_t getTileFromPoint(const blit::Point& point, uint8_t tile_size, uint8_t tile_map_width) {
	uint16_t horizontal_location = (point.x / tile_size) - 1;

	uint16_t vertical_location = (point.y / tile_size) * tile_map_width;

	// Possible crap code
	if ((point.y / tile_size) % tile_size > 0) {
		vertical_location += 1;
	}

	const uint16_t array_location = horizontal_location + vertical_location;

	return array_location;
}

// todo replace point and sprite width/height with rect
TileScanData getLocalTileData(const blit::Rect& boundingBox, uint8_t tile_size, uint8_t tileMapWidth, uint8_t* mapLayer, TileScanType scanType) {
	TileScanData tileScanData;
	tileScanData.areaSize = boundingBox.w * boundingBox.h;

	for (auto y = 0; y < boundingBox.h; y++) {
		for (auto x = 0; x < boundingBox.w; x++) {
			auto pointToCheck = blit::Point(boundingBox.x + x, boundingBox.y + y);


			const auto array_location = getTileFromPoint(pointToCheck,
				tile_size,
				tileMapWidth);
			const uint8_t tileScanned = *(mapLayer + array_location);

			TileData tileData = TileData(tileScanned, array_location, pointToCheck);

			if (auto it{ tileScanData.tilesScanned.find(tileData.id) }; it != std::end(tileScanData.tilesScanned))
			{
				it->second.detectionCount += 1;
			}
			else
			{
				tileScanData.tilesScanned.emplace(tileData.id, tileData);
			}
		}
	}
	return tileScanData;
}