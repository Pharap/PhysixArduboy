//
//  Copyright (C) 2018-2021 Pharap (@Pharap)
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#pragma once

#include "Common.h"
#include "Point.h"
#include "Size.h"

class Circle
{
public:
	// Fields
	Point2 position;
	NumberU radius;

public:
	// Constructors
	constexpr Circle() = default;

	constexpr Circle(Point2 position) :
		position { position },
		radius { 1 }
	{
	}

	constexpr Circle(Point2 position, NumberU radius) :
		position { position },
		radius { radius }
	{
	}

	constexpr Circle(Number x, Number y) :
		position { x, y },
		radius { 1 }
	{
	}

	constexpr Circle(Number x, Number y, NumberU radius) :
		position { x, y },
		radius { radius }
	{
	}

	constexpr Number getX() const
	{
		return this->position.x;
	}

	constexpr Number getY() const
	{
		return this->position.y;
	}

	constexpr Size2 getSize() const
	{
		return Size2(this->radius, this->radius);
	}

	constexpr NumberU getWidth() const
	{
		return this->radius;
	}

	constexpr NumberU getHeight() const
	{
		return this->radius;
	}

	constexpr NumberU getDiameter() const
	{
		return (this->radius * 2);
	}

	constexpr NumberU getRadiusSquared() const
	{
		return (this->radius * this->radius);
	}

	// Returns true if the point intersects the circle
	constexpr bool intersects(Point2 point) const
	{
		return (distanceSquared(this->position, point) <= this->getRadiusSquared());
	}

	// Returns true if the point lies within the circle
	constexpr bool contains(Point2 point) const
	{
		return (distanceSquared(this->position, point) < this->getRadiusSquared());
	}
};

// Returns true if the circles intersect each other
constexpr inline bool intersects(Circle first, Circle second)
{
	return (distanceSquared(first.position, second.position) <= square(first.radius + second.radius));
}