#pragma once

double CalculateDistance(blit::Point& pointA, blit::Point pointB)
{
	return std::sqrt(pow((pointB.x - pointA.x), 2) + pow((pointB.y - pointA.y), 2));
}

blit::Vec2 DegreesToVector(float degrees)
{
	float radian = (pi * degrees) / 180.00f;

	blit::Vec2 newVector = Vec2(0, 1);
	newVector.rotate(radian);

	return newVector;
}

float calcAngleBetweenPoints(blit::Point a, blit::Point b)
{
	// calculate angle as radian from car to target node
	float targetAngle = atan2(b.x - a.x, b.y - a.y);

	// get angle in degrees
	targetAngle = targetAngle * 180.00f / pi;

	targetAngle = targetAngle * -1;


	if (targetAngle < 0)
	{
		targetAngle += 360;
	}

	return targetAngle;
}