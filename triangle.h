#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtx/normal.hpp>
#include "screencoordinate.h"
#include "triangleminmax.h"
#include "zbuffer.h"
#include "canvas.h"
#include "utility.h"

struct Triangle
{
	const glm::vec3& v1;
	const glm::vec3& v2;
	const glm::vec3& v3;
	const glm::vec3 normal;
	
	Triangle(const glm::vec3& _p1, const glm::vec3& _p2, const glm::vec3& _p3) : v1{ _p1 }, v2{ _p2 }, v3{ _p3 }, normal{glm::triangleNormal(v1, v2, v3) }	{}

	const glm::vec3& operator[](int x) const
	{
		if (x == 0) return v1;
		if (x == 1) return v2;
		if (x == 2) return v3;
	}
	/*
	glm::vec3& operator[](int x)
	{
		if (x == 0) return v1;
		if (x == 1) return v2;
		if (x == 2) return v3;
	}
	*/
	std::vector<glm::vec3> getEdges() const
	{
		std::vector<glm::vec3> edges;
		edges.push_back(v2 - v1);
		edges.push_back(v3 - v2);
		edges.push_back(v1 - v3);
		return edges;
	}

	TriangleScreenCoordinates getScreenCoordinatesOfVertices(const Camera& camera, const Canvas& canvas, ZBuffer& zBuffer) const
	{
		TriangleScreenCoordinates screenCoordinates {};
		if (computePixelCoordinates(v1, canvas, camera, zBuffer, screenCoordinates.v1) &&
			computePixelCoordinates(v2, canvas, camera, zBuffer, screenCoordinates.v2) &&
			computePixelCoordinates(v3, canvas, camera, zBuffer, screenCoordinates.v3)
			) screenCoordinates.isValid = true;
		return screenCoordinates;
	}

	std::vector<ScreenCoordinate> getValidScreenCoordinates(const TriangleScreenCoordinates& vertexScreenCoordinates, const ZBuffer& zbuffer) const
	{
		std::vector<ScreenCoordinate> validScreenCoordinates;
		const auto triMinMax = TriangleMinMax(vertexScreenCoordinates);

		if (triMinMax.minX > zbuffer.getWidth() - 1 || triMinMax.maxX < 0 || triMinMax.minY > zbuffer.getHeight() - 1 || triMinMax.maxY < 0) return validScreenCoordinates;

		auto imageWidth = zbuffer.getWidth();
		auto imageHeight = zbuffer.getHeight();
		uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(triMinMax.minX)));
		uint32_t x1 = std::min(int32_t(imageWidth) - 1, (int32_t)(std::floor(triMinMax.maxX)));
		uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(triMinMax.minY)));
		uint32_t y1 = std::min(int32_t(imageHeight) - 1, (int32_t)(std::floor(triMinMax.maxY)));

		for (uint32_t y = y0; y <= y1; ++y) {
			for (uint32_t x = x0; x <= x1; ++x) {
				ScreenCoordinate p(x + 0.5f, y + 0.5f);
				if (vertexScreenCoordinates.isPointInside(p))
				{
					validScreenCoordinates.push_back(p);
				}
			}
		}
		/*
		for (auto x = triMinMax.getMinX(); x <= triMinMax.getMaxX(); x++)
		{
			for (auto y = triMinMax.getMinY(); y <= triMinMax.getMaxY(); y++)
			{
				ScreenCoordinate p(x, y);
				if (vertexScreenCoordinates.isPointInside(p))
				{
					validScreenCoordinates.push_back(p);
				}
			}
		}
		*/
		return validScreenCoordinates;
	}

	void rasterizeTriangleToZBuffer(const Camera& camera, const Canvas& canvas, ZBuffer& zBuffer, const int meshID) const
	{
		const auto triangleScreenCoordinates = getScreenCoordinatesOfVertices(camera, canvas, zBuffer);
		if (!triangleScreenCoordinates.isValid) return;
		auto validScreenCoordinates = getValidScreenCoordinates(triangleScreenCoordinates, zBuffer);

		for (auto& screenPoint : validScreenCoordinates)
		{
			glm::vec3 point3D;
			if (barycentricPointTo3D(triangleScreenCoordinates, screenPoint, point3D))
			{
				auto val = getZFromBarycentricCoordinates(point3D, v1, v2, v3);
				val = -val;
				const auto x = static_cast<int>(screenPoint.x);
				const auto y = static_cast<int>(screenPoint.y);
				zBuffer.setIfCloser(x, y, val, meshID);
			}
		}
	}
};
