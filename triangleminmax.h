#pragma once
#include "screencoordinate.h"

struct TriangleMinMax
{
	float minX, minY, maxX, maxY;

	TriangleMinMax(const TriangleScreenCoordinates& triangleScreenCoordinates)
	{
		computeMinX(triangleScreenCoordinates.v1, triangleScreenCoordinates.v2, triangleScreenCoordinates.v3);
		computeMinY(triangleScreenCoordinates.v1, triangleScreenCoordinates.v2, triangleScreenCoordinates.v3);
		computeMaxX(triangleScreenCoordinates.v1, triangleScreenCoordinates.v2, triangleScreenCoordinates.v3);
		computeMaxY(triangleScreenCoordinates.v1, triangleScreenCoordinates.v2, triangleScreenCoordinates.v3);
	}

	float getMinX() const
	{
		return minX;
	}

	float getMinY() const
	{
		return minY;
	}

	float getMaxX() const
	{
		return maxX;
	}

	float getMaxY() const
	{
		return maxY;
	}

private:
	void computeMinX(const ScreenCoordinate& v1, const ScreenCoordinate& v2, const ScreenCoordinate& v3)
	{
		minX = v1.x;
		if (minX > v2.x) minX = v2.x;
		if (minX > v3.x) minX = v3.x;
	}

	void computeMinY(const ScreenCoordinate& v1, const ScreenCoordinate& v2, const ScreenCoordinate& v3)
	{
		minY = v1.y;
		if (minY > v2.y) minY = v2.y;
		if (minY > v3.y) minY = v3.y;
	}

	void computeMaxX(const ScreenCoordinate& v1, const ScreenCoordinate& v2, const ScreenCoordinate& v3)
	{
		maxX = v1.x;
		if (maxX < v2.x) maxX = v2.x;
		if (maxX < v3.x) maxX = v3.x;
	}

	void computeMaxY(const ScreenCoordinate& v1, const ScreenCoordinate& v2, const ScreenCoordinate& v3)
	{
		maxY = v1.y;
		if (maxY < v2.y) maxY = v2.y;
		if (maxY < v3.y) maxY = v3.y;
	}
};
