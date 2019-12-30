#pragma once
#include <vector>
#include "triangle.h"
#include "utility.h"
#include <limits>
#include "camera.h"

struct Mesh
{
	int ID;
	std::vector<glm::vec3> vertices;
	std::vector<int> triangleIndices;
	std::vector<Triangle> triangles;
	glm::vec3 boundsMin {std::numeric_limits<float>::max()};
	glm::vec3 boundsMax { std::numeric_limits<float>::min() };

	Mesh() = default;
	Mesh(int meshID, int* tris, float* verts, int triangleArraySize, int vertexArraySize): ID(meshID), triangleIndices(tris, tris + triangleArraySize)
	{
		vertices.reserve(vertexArraySize / 3);
		for (int i = 0; i < vertexArraySize; i++)
		{
			const auto& x = verts[i];
			const auto& y = verts[++i];
			const auto& z = verts[++i];
			if (boundsMin.x > x) boundsMin.x = x;
			if (boundsMax.x < x) boundsMax.x = x;
			if (boundsMin.y > y) boundsMin.y = y;
			if (boundsMax.y < y) boundsMax.y = y;
			if (boundsMin.z > z) boundsMin.z = z;
			if (boundsMax.z < z) boundsMax.z = z;
			vertices.emplace_back(glm::vec3(x, y, z));
		}

		triangles.reserve(triangleArraySize / 3);
		for (int i = 0; i < triangleArraySize; i++)
		{
			const auto& t1 = tris[i];   const auto& v1 = vertices[t1];
			const auto& t2 = tris[++i]; const auto& v2 = vertices[t2];
			const auto& t3 = tris[++i]; const auto& v3 = vertices[t3];
			triangles.emplace_back(Triangle(v1, v2, v3));
		}
	}
	
	void addVertex(const float v1, const float v2, const float v3)
	{
		vertices.emplace_back(glm::vec3(v1, v2, v3));
	}

	glm::vec3 getCenter() const
	{
		auto sum = glm::vec3(0.0f, 0.0f, 0.0f);
		for (auto& v : vertices) sum += v;
		const float count = 1.0f / vertices.size();
		return sum * count;
	}

	bool isBackFaced(const glm::vec3& camDirectionVec, const Triangle& tri) const
	{
		//const auto dirVec = glm::normalize(directionVec - tri.normal);
		const auto camToTriNormalVec = camDirectionVec - tri.v1;
		return 0.0f > glm::dot(camToTriNormalVec, tri.normal);
		/*
		const auto dirVec = glm::normalize(cameraPos - tri.v1);
		const auto dotProduct = -glm::dot(tri.normal, dirVec);
		return 0.0f > dotProduct;
		*/
		
	}

	void rasterize(const Camera& camera, const Canvas& canvas, ZBuffer& zBuffer) const
	{
		for (const auto& t : triangles)
		{
			const auto& viewDir = camera.getViewDir();
			if (isBackFaced(viewDir, t)) continue;
			t.rasterizeTriangleToZBuffer(camera, canvas, zBuffer, ID);
		}
	}

};
