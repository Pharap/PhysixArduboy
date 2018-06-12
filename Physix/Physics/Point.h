#pragma once

#include "Common.h"

class Vector2;

class Point2
{
public:
	// Fields
	Number x;
	Number y;
	
public:
	// Constructors
	constexpr Point2(void) = default;
	constexpr Point2(int8_t x, int8_t y) : x(x), y(y) {}
	constexpr Point2(int16_t x, int16_t y) : x(x), y(y) {}
	constexpr Point2(Number x, Number y) : x(x), y(y) {}
	
	Point2 & operator +=(Vector2 other);
	Point2 & operator -=(Vector2 other);
};

inline constexpr bool operator ==(Point2 left, Point2 right)
{
	return ((left.x == right.x) && (left.y == right.y));
}

inline constexpr bool operator !=(Point2 left, Point2 right)
{
	return ((left.x != right.x) || (left.y != right.y));
}

// Shorthand to get square distance between two points
inline constexpr NumberU distanceSquared(Point2 firstPoint, Point2 secondPoint)
{
	// Constexpr version:
	return fromSigned(square(firstPoint.x - secondPoint.x) + square(firstPoint.y - secondPoint.y));
}