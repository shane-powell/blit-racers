#include "util.hpp"

namespace util {

	double CalculateDistance(blit::Point& pointA, blit::Point pointB)
	{
		return std::sqrt(pow((pointB.x - pointA.x), 2) + pow((pointB.y - pointA.y), 2));
	}

	blit::Vec2 DegreesToVector(float degrees)
	{
		float radian = (blit::pi * degrees) / 180.00f;

		blit::Vec2 newVector = blit::Vec2(0, 1);
		newVector.rotate(radian);

		return newVector;
	}

	float calcAngleBetweenPoints(blit::Point a, blit::Point b)
	{
		// calculate angle as radian from car to target node
		float targetAngle = atan2(b.x - a.x, b.y - a.y);

		// get angle in degrees
		targetAngle = targetAngle * 180.00f / blit::pi;

		targetAngle = targetAngle * -1;


		if (targetAngle < 0)
		{
			targetAngle += 360;
		}

		return targetAngle;
	}

	bool IsRectIntersecting(blit::Rect rect1, blit::Rect rect2) {
		if (rect1.x + rect1.w > rect2.x &&
			rect1.x < rect2.x + rect2.w &&
			rect1.y + rect1.h > rect2.y &&
			rect1.y < rect2.y + rect2.h) {
			return true;
		}
		return false;
	}

	std::string GetLapTimeString(uint32_t lapTime)
	{
		const uint32_t minutes = lapTime / 60000;
		const uint32_t seconds = (lapTime % 60000) / 1000;
		const uint32_t milliseconds = lapTime % 1000;

		std::string secondString = std::to_string(seconds);

		secondString.insert(secondString.begin(), 2 - secondString.size(), '0');

		std::string millisecondString = std::to_string(milliseconds);

		millisecondString.insert(millisecondString.begin(), 3 - millisecondString.size(), '0');


		return std::to_string(minutes) + ":" + secondString + ":" + millisecondString;
	}

	double MapRange(float range1Min, float range1Max, float range1Value, float range2Min, float range2Max)
	{
		const float slope = 1.0f * (range2Max - range2Min) / (range1Max - range1Min);
		const auto output = range2Min + slope * (range1Value - range1Min);

		return output;
	}
}