#pragma once
#include <vector>
#include <mesh.h>
#include "zbuffer.h"
#include "camera.h"
#include "canvas.h"

class UnityData
{
public:
	
	void setCanvas(float width, float height)
	{
		canvas = Canvas(width, height);
	}

	void setBuffer(int width, int height)
	{
		zBuffer = ZBuffer(width, height);
	}

	void updateCamera(const float* cameraData)
	{
		const auto& fieldOfView = cameraData[0];
		const auto& nearClipPlane = cameraData[1];
		const auto& farClipPlane = cameraData[2];
		const auto& aspectRatio = cameraData[3];

		const auto& posX = cameraData[4];
		const auto& posY = cameraData[5];
		const auto& posZ = cameraData[6];

		const auto& quaternionX = cameraData[7];
		const auto& quaternionY = cameraData[8];
		const auto& quaternionZ = cameraData[9];
		const auto& quaternionW = cameraData[10];

		camera = Camera(fieldOfView, aspectRatio, nearClipPlane, farClipPlane);
		camera.setPos(posX, posY, posZ);
		camera.setRotation(quaternionX, quaternionY, quaternionZ, quaternionW);
		camera.calculateViewDirection();
		camera.generateFrustum();
	}

	void addMesh(int meshID, int* tris, float* verts, int triangleArraySize, int vertexArraySize)
	{
		meshes.emplace_back(Mesh(meshID, tris, verts, triangleArraySize, vertexArraySize));
	}

	void removeMesh(int meshID)
	{
		for (auto it = meshes.begin(); it != meshes.end(); ++it) 
		{
			if (it->ID == meshID)
			{
				meshes.erase(it);
				return;
			}
		}
	}

	const std::vector<Mesh>& getMeshes() const
	{
		return meshes;
	}

	const Camera& getCamera() const
	{
		return camera;
	}

	ZBuffer& getZBuffer()
	{
		return zBuffer;
	}

	const Canvas& getCanvas() const
	{
		return canvas;
	}


private:
	std::vector<Mesh> meshes;
	Camera camera;
	Canvas canvas{ 2.0f, 2.0f };
	ZBuffer zBuffer;
};