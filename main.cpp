#include <algorithm>
#include "main.h"
#include "camera.h"
#include "mesh.h"
#include "global.h"
#include "log.h"

#ifdef _WIN32
#define DllExport __declspec(dllexport)
#else
#define DllExport
#endif

extern "C"
{
	DllExport void reset()
	{
		Global::reset();
	}

	DllExport const char* getZbufferPath()
	{
		return Global::unityData.getZBuffer().getZBufferPath();
	}
	
	DllExport void processMesh(int meshID, int* tris, float* verts, int triangleArraySize, int vertexArraySize)
	{
		Global::unityData.addMesh(meshID, tris, verts, triangleArraySize, vertexArraySize);
	}
	
	DllExport void updateCamera(float* cameraSettings)
	{
		try {
			Global::unityData.updateCamera(cameraSettings);
		}
		catch (const std::exception & e)
		{
			Log("C++ exception thrown in updateCamera():");
			Log(e.what());
		}
	}
	
	DllExport void setBufferSize(int width, int height)
	{
		try {
			auto& zBuffer = Global::unityData.getZBuffer();
			zBuffer = ZBuffer(width, height);
			Log("buffer set to width:" + std::to_string(width) + " , height:" + std::to_string(height) );
		}
		catch (const std::exception & e)
		{
			Log("C++ exception thrown in setBufferSize():");
			Log(e.what()); 
		}
	}
	
	DllExport int* calculateVisibility()
	{
		try {
			const auto& meshes = Global::unityData.getMeshes();
			const auto& camera = Global::unityData.getCamera();
			const auto& canvas = Global::unityData.getCanvas();
			auto& zBuffer = Global::unityData.getZBuffer();
			zBuffer.clear();

			for (auto& mesh : meshes)
			{
				if(camera.isMeshInsideFrustum(mesh.boundsMin, mesh.boundsMax))
					mesh.rasterize(camera, canvas, zBuffer);
			}
			const auto visibleMeshIDs = zBuffer.getVisibleMeshList();

			const auto visibleMeshCount = visibleMeshIDs.size();
			const auto targetSize = visibleMeshCount + 1;
			if (Global::visibleMeshes.size() != targetSize) Global::visibleMeshes.resize(targetSize);
			if (visibleMeshCount != 0) {
				int index = 1;
				for (const auto& meshID : visibleMeshIDs)
				{
					Global::visibleMeshes[index] = meshID;
					index++;
				}
				std::sort(Global::visibleMeshes.begin() + 1, Global::visibleMeshes.end());
			}
			Global::visibleMeshes[0] = static_cast<int>(visibleMeshCount);
			
			return &Global::visibleMeshes[0];
		}
		catch (const std::exception & e)
		{
			Log("C++ exception thrown in main():");
			Log(e.what());
			Global::visibleMeshes.resize(1);
			Global::visibleMeshes[0] = 0;
			return &Global::visibleMeshes[0];
		}
	}

}