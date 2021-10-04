#pragma once

#include <32blit.hpp>

namespace util {
	double CalculateDistance(blit::Point& pointA, blit::Point pointB);

	blit::Vec2 DegreesToVector(float degrees);

	float calcAngleBetweenPoints(blit::Point a, blit::Point b);

	bool IsRectIntersecting(blit::Rect rect1, blit::Rect rect2);

	std::string GetLapTimeString(uint32_t lapTime);

	double MapRange(float range1Min, float range1Max, float range1Value, float range2Min, float range2Max);

	float Rotate(float angle, float rotationDegrees);
}

