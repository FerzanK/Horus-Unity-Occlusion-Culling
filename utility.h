#pragma once
#include <glm/vec3.hpp>
#include "canvas.h"
#include "camera.h"
#include "zbuffer.h"
#include "screencoordinate.h"


inline bool computePixelCoordinates(const glm::vec3& pWorld, const Canvas& canvas, const Camera& camera, const ZBuffer& zBuffer, ScreenCoordinate& pRaster)
{
	const auto pNew = glm::vec4(pWorld, 1.0f);

	//Camera matrix
	const auto& projectionMatrix = camera.getProjectionMatrix();
	const auto& viewMatrix = camera.getViewMatrix();
	const auto& rotationMatrix = camera.getRotationMatrix();

	const auto trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0001f));
	const auto modelSpace = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	// Coordinates of the point on the canvas. Use perspective projection.
	const auto cameraSpace = projectionMatrix * viewMatrix * rotationMatrix * pNew;
	glm::vec2 pScreen(cameraSpace.x, cameraSpace.y);
	pScreen = pScreen / -cameraSpace.z;

	// If the x- or y-coordinate absolute value is greater than the canvas width 
	// or height respectively, the point is not visible
	if (!canvas.isPointInsideCanvas(pScreen)) return false;

	// Normalize. Coordinates will be in the range [0,1]
	const auto pNDC = canvas.normalize(pScreen);

	// Finally convert to pixel coordinates. Don't forget to invert the y coordinate
	pRaster.x = std::floor(zBuffer.getWidth() * (pNDC.x));
	pRaster.y = std::floor(zBuffer.getHeight() * (1 - pNDC.y));

	return true;
}


// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
inline glm::vec3 Barycentric(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	const auto v0 = b - a, v1 = c - a, v2 = p - a;
	const auto d00 = glm::dot(v0, v0);
	const auto d01 = glm::dot(v0, v1);
	const auto d11 = glm::dot(v1, v1);
	const auto d20 = glm::dot(v2, v0);
	const auto d21 = glm::dot(v2, v1);
	const auto denom = d00 * d11 - d01 * d01;
	const auto v = (d11 * d20 - d01 * d21) / denom;
	const auto w = (d00 * d21 - d01 * d20) / denom;
	glm::vec3 barycentricCoordinates(v, w, 1 - v - w);
	return barycentricCoordinates;
}

inline bool isPointInsideTriangle(const glm::vec3& p, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	const auto result = Barycentric(p, v1, v2, v3);
	return result.x >= 0.0f && result.y >= 0.0f && result.z >= 0.0f;
}

inline float getZFromBarycentricCoordinates(const glm::vec3& b, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	float z = v1.z * b.x;
	z += v2.z * b.y;
	z += v3.z * b.z;
	return z;
}

inline bool barycentricPointTo3D(const ScreenCoordinate v1, const ScreenCoordinate v2, const ScreenCoordinate v3, const ScreenCoordinate P, glm::vec3& result) {
	auto u = glm::cross(glm::vec3(v3.x - v1.x, v2.x - v1.x, v1.x - P.x), glm::vec3(v3.y - v1.y, v2.y - v1.y, v1.y - P.y));
	/* `pts` and `P` has integer value as coordinates
	   so `abs(u[2])` < 1 means `u[2]` is 0, that means
	   triangle is degenerate, in this case return something with negative coordinates */
	if (std::abs(u.z) < 1) return false;
	result = glm::vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return true;
}

inline bool barycentricPointTo3D(const TriangleScreenCoordinates& triangleScreenCoordinates, const ScreenCoordinate p, glm::vec3& result) {
	return barycentricPointTo3D(triangleScreenCoordinates.v1, triangleScreenCoordinates.v2, triangleScreenCoordinates.v3, p, result);
}
