#pragma once

#include "Common.h"
#include "Point.h"

class Vector2
{
public:
	// Fields
	Number x;
	Number y;
	
public:
	// Constructors
	constexpr Vector2(void) = default;
	constexpr Vector2(int8_t x, int8_t y) : x(x), y(y) {}
	constexpr Vector2(int16_t x, int16_t y) : x(x), y(y) {}
	constexpr Vector2(Number x, Number y) : x(x), y(y) {}
	
	// Can't do a regular get magnitude without using float because
	// I don't have a fixed point sqrt
	constexpr NumberU getMagnitudeSquared(void) const
	{
		return fromSigned((x * x) + (y * y));
	}
	
	Vector2 & operator +=(Vector2 other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
	
	Vector2 & operator -=(Vector2 other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}
};

inline constexpr bool operator ==(Vector2 left, Vector2 right)
{
	return ((left.x == right.x) && (left.y == right.y));
}

inline constexpr bool operator !=(Vector2 left, Vector2 right)
{
	return ((left.x != right.x) || (left.y != right.y));
}

// Adding a vector to a vector creates a new vector
inline constexpr Vector2 operator +(Vector2 left, Vector2 right)
{
	return Vector2(left.x + right.x, left.y + right.y);
}

// Subtracting a vector from a vector creates a new vector
inline constexpr Vector2 operator -(Vector2 left, Vector2 right)
{
	return Vector2(left.x - right.x, left.y - right.y);
}

// Multiplying a vector by a factor scales the vector
inline constexpr Vector2 operator *(Vector2 vector, Number factor)
{
	return Vector2(vector.x * factor, vector.y * factor);
}

// Multiplying a vector by a factor scales the vector
inline constexpr Vector2 operator *(Vector2 vector, NumberU factor)
{
	return Vector2(vector.x * fromUnsigned(factor), vector.y * fromUnsigned(factor));
}

// Dividing a vector by a factor scales the vector
inline constexpr Vector2 operator /(Vector2 vector, Number factor)
{
	// Multiplying by the inverse might be cheaper
	return vector * (1 / factor);
}

// Dividing a vector by a factor scales the vector
inline constexpr Vector2 operator /(Vector2 vector, NumberU factor)
{
	// Multiplying by the inverse might be cheaper
	return vector * fromUnsigned(1 / factor);
}

//
// Vector & Point interaction
//

// Adding a vector to a point offsets the point
inline constexpr Point2 operator +(Point2 point, Vector2 offset)
{
	return Point2(point.x + offset.x, point.y + offset.y);
}

// Subtracting a vector from a point offsets the point
inline constexpr Point2 operator -(Point2 point, Vector2 offset)
{
	return Point2(point.x - offset.x, point.y - offset.y);
}

// Subtracting two points gets the vector between them
inline constexpr Vector2 operator -(Point2 firstPoint, Point2 secondPoint)
{
	return Vector2(firstPoint.x - secondPoint.x, firstPoint.y - secondPoint.y);
}

/*// Shorthand to get square distance between two points
inline constexpr NumberU distanceSquared(Point2 firstPoint, Point2 secondPoint)
{
	// Readable Version:
	// const auto vector = firstPoint - secondPoint;
	// return vector.getMagnitudeSquared();
	
	// Constexpr version:
	return (firstPoint - secondPoint).getMagnitudeSquared();
}*/