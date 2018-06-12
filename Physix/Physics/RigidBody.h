#pragma once

#include "Common.h"
#include "Point.h"
#include "Vector.h"

class RigidBody
{
public:
	// Fields
	Point2 position = Point2(0, 0);
	Vector2 velocity = Vector2(0, 0);
	Number mass = 1.0;

public:
	// Constructors
	constexpr RigidBody(void) = default;
	constexpr RigidBody(Point2 position) : position(position), velocity(), mass(1.0) {}
	constexpr RigidBody(Point2 position, Number mass) : position(position), velocity(), mass(mass) {}
	
	constexpr Number getX(void) const
	{
		return this->position.x;
	}
	
	constexpr Number getY(void) const
	{
		return this->position.y;
	}
	
	void applyForce(Vector2 force)
	{
		this->velocity += (force / mass);
	}
};