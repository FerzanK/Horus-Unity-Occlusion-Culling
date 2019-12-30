#pragma once
#include <glm/vec2.hpp>


struct Canvas
{
	glm::vec2 normalize(const glm::vec2& pScreen) const
	{
		glm::vec2 pNDC;
		pNDC.x = (pScreen.x + canvasWidth * 0.5f) / canvasWidth;
		pNDC.y = (pScreen.y + canvasHeight * 0.5f) / canvasHeight;
		return pNDC;
	}

	bool isPointInsideCanvas(glm::vec2& pScreen) const
	{
		if (std::abs(pScreen.x) > canvasWidth || std::abs(pScreen.y) > canvasHeight) return false;
		return true;
	}

	float getWidth() const
	{
		return canvasWidth;
	}

	float getHeight() const
	{
		return canvasHeight;
	}

	Canvas(const float width, const float height) : canvasWidth(width), canvasHeight(height) {};
	Canvas() = default;

	private:
		float canvasWidth;
		float canvasHeight;

};