#pragma once

#include <32blit.hpp>

double CalculateDistance(blit::Point& pointA, blit::Point pointB);

blit::Vec2 DegreesToVector(float degrees);

float calcAngleBetweenPoints(blit::Point a, blit::Point b);

bool IsRectIntersecting(blit::Rect rect1, blit::Rect rect2);