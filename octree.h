#pragma once
#include "mesh.h"
#include "glm/vec3.hpp"
#include <vector>
#include <memory>


struct BoundingBox
{
	glm::vec3 min;
	glm::vec3 max;

	BoundingBox() : min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min()) {}

	void add(const glm::vec3& vertex)
	{
		addMin(vertex);
		addMax(vertex);
	}

private:
	void addMin(const glm::vec3& vertex)
	{
		assignLower(min.x, vertex.x);
		assignLower(min.y, vertex.y);
		assignLower(min.z, vertex.z);
	}

	void addMax(const glm::vec3& vertex)
	{
		assignHigher(min.x, vertex.x);
		assignHigher(min.y, vertex.y);
		assignHigher(min.z, vertex.z);
	}

	void assignLower(float& v1, const float& v2)
	{
		if (v1 > v2) v1 = v2;
	}

	void assignHigher(float& v1, const float& v2)
	{
		if (v1 < v2) v1 = v2;
	}
};


class Node
{
public:

	glm::vec3 min;
	glm::vec3 max;
	std::unique_ptr<Node[]> children;

	Node() = default;
	Node(const BoundingBox& boundingBox) : min(boundingBox.min), max(boundingBox.max) {}
	
	Node(const glm::vec3& parentPoint, const glm::vec3& parentCentoid, const glm::vec3& parentHalfDimension)
	{
		auto centoid = (parentPoint + parentCentoid) / 2.0f;
		auto halfDimension = parentHalfDimension / 2.0f;
		min = centoid - halfDimension;
		max = centoid + halfDimension;
	}

	void addTriangle(const Triangle& triangle)
	{

	}

	void divide()
	{
		auto centoid = getCentoid();
		auto halfDimension = getHalfDimension();
		glm::vec3 V1(min.x, min.y, max.z);
		glm::vec3 V2(min.x, max.y, min.z);
		glm::vec3 V3(min.x, max.y, max.z);
		glm::vec3 V4(max.x, min.y, min.z);
		glm::vec3 V5(max.x, min.y, max.z);
		glm::vec3 V6(max.x, max.y, min.z);
		children = std::make_unique<Node[]>(8);
		children[0] = Node(min, centoid, halfDimension);
		children[1] = Node(V1, centoid, halfDimension);
		children[2] = Node(V2, centoid, halfDimension);
		children[3] = Node(V3, centoid, halfDimension);
		children[4] = Node(V4, centoid, halfDimension);
		children[5] = Node(V5, centoid, halfDimension);
		children[6] = Node(V6, centoid, halfDimension);
		children[7] = Node(max, centoid, halfDimension);
	}

	void generateMesh(std::vector<Mesh>& meshes)
	{
		if (hasChildren())
		{
			for (int i = 0; i < 8; i++)
			{
				children[i].generateMesh(meshes);
			}
		}
		else {
			Mesh newMesh;
			newMesh.addVertex(min.x, min.y, min.z);
			newMesh.addVertex(max.x, min.y, min.z);
			newMesh.addVertex(max.x, min.y, max.z);
			newMesh.addVertex(min.x, min.y, max.z);
			newMesh.addVertex(min.x, max.y, min.z);
			newMesh.addVertex(max.x, max.y, min.z);
			newMesh.addVertex(max.x, max.y, max.z);
			newMesh.addVertex(min.x, max.y, max.z);
			newMesh.triangleIndices = { 1,1,1,2,2,1,3,3,1,4,4,1,5,5,2,6,6,2,2,7,2,1,8,2,6,6,3,7,9,3,3,3,3,2,7,3,7,9,4,8,10,4,4,4,4,3,3,4,8,10,5,5,11,5,1,12,5,4,4,5,5,13,6,8,10,6,7,9,6,6,14,6 };
			meshes.push_back(newMesh);
		}
	}

private:

	glm::vec3 getCentoid()
	{
		return (min + max) / 2.0f;
	}

	glm::vec3 getHalfDimension()
	{
		return (max - min) / 2.0f;
	}

	bool isLeaf()
	{
		return children == nullptr;
	}

	bool hasChildren()
	{
		return children != nullptr;
	}

};


struct Octree
{

	BoundingBox bounds;
	int depth = 0;

	Octree(const Mesh& mesh, int _depth): depth(_depth)
	{
		bounds = generateBoundingBox(mesh);
	}


	void generateMesh(const Mesh& mesh)
	{
		bounds = generateBoundingBox(mesh);
	}


	BoundingBox generateBoundingBox(const Mesh& mesh)
	{
		auto boundingBox = BoundingBox();
		for (auto& v : mesh.vertices)
		{
			boundingBox.add(v);
		}

		return boundingBox;
	}
	


};

typedef struct Interval {
	float min;
	float max;
} Interval;


Interval GetInterval(const Triangle& triangle, const glm::vec3& axis) {
	Interval result;

	result.min = glm::dot(axis, triangle.v1);
	result.max = result.min;
	for (int i = 1; i < 3; ++i) {
		float value = glm::dot(axis, triangle[i]);
		result.min = fminf(result.min, value);
		result.max = fmaxf(result.max, value);
	}

	return result;
}

Interval GetInterval(const Node& aabb, const glm::vec3& axis) {
	glm::vec3 i = aabb.min;
	glm::vec3 a = aabb.max;

	glm::vec3 vertex[8] = {
		glm::vec3(i.x, a.y, a.z),
		glm::vec3(i.x, a.y, i.z),
		glm::vec3(i.x, i.y, a.z),
		glm::vec3(i.x, i.y, i.z),
		glm::vec3(a.x, a.y, a.z),
		glm::vec3(a.x, a.y, i.z),
		glm::vec3(a.x, i.y, a.z),
		glm::vec3(a.x, i.y, i.z)
	};

	Interval result;
	result.min = result.max = glm::dot(axis, vertex[0]);

	for (int i = 1; i < 8; ++i) {
		float projection = glm::dot(axis, vertex[i]);
		result.min = (projection < result.min) ? projection : result.min;
		result.max = (projection > result.max) ? projection : result.max;
	}

	return result;
}


bool OverlapOnAxis(const Node& aabb, const Triangle& triangle, const glm::vec3& axis) 
{ 
	Interval a = GetInterval(aabb, axis);    
	Interval b = GetInterval(triangle, axis);    
	return ((b.min <= a.max) && (a.min <= b.max)); 
}

bool TriangleAABB(const Triangle& t, const Node& a) {
	// Compute the edge vectors of the triangle  (ABC)
	auto triangleEdges = t.getEdges();
	auto& f0 = triangleEdges[0];
	auto& f1 = triangleEdges[1];
	auto& f2 = triangleEdges[2];

	// Compute the face normals of the AABB
	glm::vec3 u0(1.0f, 0.0f, 0.0f);
	glm::vec3 u1(0.0f, 1.0f, 0.0f);
	glm::vec3 u2(0.0f, 0.0f, 1.0f);

	glm::vec3 test[13] = {
		// 3 Normals of AABB
		u0, // AABB Axis 1
		u1, // AABB Axis 2
		u2, // AABB Axis 3
		// 1 Normal of the Triangle
		glm::cross(f0, f1),
		// 9 Axis, cross products of all edges
		glm::cross(u0, f0),
		glm::cross(u0, f1),
		glm::cross(u0, f2),
		glm::cross(u1, f0),
		glm::cross(u1, f1),
		glm::cross(u1, f2),
		glm::cross(u2, f0),
		glm::cross(u2, f1),
		glm::cross(u2, f2)
	};

	for (int i = 0; i < 13; ++i) {
		if (!OverlapOnAxis(a, t, test[i])) {
			return false; // Seperating axis found
		}
	}

	return true; // Seperating axis not found
}