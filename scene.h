#pragma once
#include <vector>
#include <camera.h>
#include <zbuffer.h>
#include <canvas.h>
#include <mesh.h>

class Scene
{
	Camera& camera;
	ZBuffer& zbuffer;
	Canvas& canvas;
	std::vector<Mesh>& meshes;

	Scene(Camera& _camera, ZBuffer& _zbuffer, Canvas& _canvas, std::vector<Mesh>& _meshes) : camera(_camera), zbuffer(zbuffer), canvas(_canvas), meshes(_meshes) {}

	void render() const
	{
		for(auto& mesh : meshes)
		{
			mesh.rasterize(camera, canvas, zbuffer);
		}
	}
};