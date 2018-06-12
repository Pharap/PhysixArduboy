#pragma once

#include "Common.h"

class Size2
{
public:
	// Fields
	NumberU width;
	NumberU height;
	
public:
	// Constructors
	constexpr Size2(void) = default;
	constexpr Size2(NumberU width, NumberU height) : width(width), height(height) {}
};

inline constexpr bool operator ==(Size2 left, Size2 right)
{
	return ((left.width == right.width) && (left.height == right.height));
}

inline constexpr bool operator !=(Size2 left, Size2 right)
{
	return ((left.width != right.width) || (left.height != right.height));
}