#include <global.h>

namespace Global
{
	UnityData unityData;
	ZBuffer zBuffer;
	std::vector<int> visibleMeshes;
	bool loggerSet {false};

	void reset()
	{
		unityData = UnityData();
		zBuffer = ZBuffer();
		visibleMeshes.clear();
		loggerSet = false;
	}
}