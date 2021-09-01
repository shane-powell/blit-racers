#include "game.hpp"

void Game::ChangeCurrentTrack()
{
	delete this->currentTrack;
	this->currentTrack = LoadTrack(this->currentTrackId);
}