#pragma once
#include "unitydata.h"
#include "zbuffer.h"
#include <vector>
#include <string>

namespace Global
{
	extern UnityData unityData;
	extern ZBuffer zBuffer;
	extern std::vector<int> visibleMeshes;
	extern bool loggerSet;
	void reset();
}