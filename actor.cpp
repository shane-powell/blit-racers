#include "actor.hpp"

void Actor::ProcessTileData(Track* currentTrack)
{
	for (auto const& tileType : this->currentTileData.tilesScanned) {

		uint16_t fallCount = 0;

		for (auto const& trackTile : currentTrack->activeTiles)
		{
			if (tileType.first == trackTile.id)
			{
				if (trackTile.obstruction)
				{
					this->currentTileData.obstruction = true;
					this->currentTileData.collisionLocation = tileType.second.detectionLocation;
				}

				if (trackTile.fall)
				{
					fallCount += trackTile.detectionCount;
				}
			}
		}

		if (fallCount > this->currentTileData.areaSize * 0.75)
		{
			
		}
	}
}