#pragma once
struct ScreenCoordinate
{
	float x;
	float y;
	ScreenCoordinate() = default;
	ScreenCoordinate(const float _x, const float _y) : x{ _x }, y{ _y }{};
};

struct TriangleScreenCoordinates
{
	ScreenCoordinate v1;
	ScreenCoordinate v2;
	ScreenCoordinate v3;
	bool isValid {false};

	bool isPointInside(const ScreenCoordinate& p) const
	{
		auto as_x = p.x - v1.x;
		auto as_y = p.y - v1.y;

		bool s_ab = (v2.x - v1.x) * as_y - (v2.y - v1.y) * as_x > 0;

		if ((v3.x - v1.x) * as_y - (v3.y - v1.y) * as_x > 0 == s_ab) return false;

		if ((v3.x - v2.x) * (p.y - v2.y) - (v3.y - v2.y) * (p.x - v2.x) > 0 != s_ab) return false;

		return true;
	}

};