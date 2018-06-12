#pragma once

#include <stdint.h>
#include <FixedPoints.h>

using Number = SFixed<12, 3>;
using NumberU = UFixed<13, 3>;

constexpr inline NumberU fromSigned(Number value)
{
	return NumberU::fromInternal(value.getInternal());
}

constexpr inline Number fromUnsigned(NumberU value)
{
	return Number::fromInternal(value.getInternal());
}

template< typename T >
constexpr auto square(T value) -> decltype(value * value)
{
	return value * value;
}

template< typename T, size_t size >
constexpr size_t arrayLength(T (&)[size])
{
	return size;
}

template< typename T >
constexpr size_t arrayLength(T (&)[0])
{
	return 0;
}