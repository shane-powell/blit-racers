#pragma once
#include <32blit.hpp>

class Position
{
public:
	blit::Point location;
	float angle;

	Position(blit::Point location, float angle)
	{
		this->location = location;
		this->angle = angle;
	}
};