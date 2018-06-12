#include "Physics/Point.h"
#include "Physics/Vector.h"

Point2 & Point2::operator+=(Vector2 other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}

Point2 & Point2::operator-=(Vector2 other)
{
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}