#pragma once
#include <limits>
#include <unordered_set>
#include "bitmap_image.hpp"
#include "log.h"
#define NOMINMAX


struct ZBufferData
{
	bool isAssigned {false};
	float value { (std::numeric_limits<float>::max)() };
	int meshID {-1};

	void setValue(const float val, const int _meshID)
	{
		value = val;
		meshID = _meshID;
		isAssigned = true;
	}

	void clear()
	{
		isAssigned = false;
		value = std::numeric_limits<float>::max();
	}
};

struct ZBuffer
{
	ZBuffer() = default;
	ZBuffer(const int width_, const int height_) :	data{ std::vector<ZBufferData>(static_cast<size_t>(width_) * static_cast<size_t>(height_)) },
												width{ width_ },
												height{ height_ }
												{}


	int getWidth() const
	{
		return width;
	}

	int getHeight() const
	{
		return height;
	}

	void clear()
	{
		for (auto& i : data)
		{
			i.clear();
		}
	}
	
	void setValue(const int x, const int y, const float val, const int meshID)
	{
		const auto arrayPos = getArrayPos(x,y);
		setValue(arrayPos, val, meshID);
	}

	float getValue(const int x, const int y)
	{
		const auto arrayPos = getArrayPos(x, y);
		return data[arrayPos].value;
	}

	void setIfCloser(const int x, const int y, const float val, const int meshID)
	{
		const auto arrayPos = getArrayPos(x, y);
		if (arrayPos >= data.size())
		{
			Log("setIfCloser called with wrong coordinates, x:" + std::to_string(x) + ", y:" + std::to_string(y) + ", arrayPos:" + std::to_string(arrayPos));
			return;
		}
		if (data[arrayPos].value > val) setValue(arrayPos, val, meshID);
	}

	bitmap_image convertToImage()
	{
		bitmap_image image(width, height);
		
		auto maxZ = (std::numeric_limits<float>::min)(); // #define NOMINMAX doesn't seem to be working, windows define macros for min(), max(), this is a workaround
		auto minZ = (std::numeric_limits<float>::max)();

		for(size_t i=0; i < data.size(); i++)
		{
			if (isAssignedVal(i))
			{
				const auto& currentValue = data[i].value;
				if (minZ > currentValue) minZ = currentValue;
				if (maxZ < currentValue) maxZ = currentValue;
			}
		}

		const auto dist = maxZ - minZ;

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				const auto pos = getArrayPos(x, y);
				if (isUnassignedVal(pos))  continue;
				auto currentVal = getValue(x, y);
				currentVal = (currentVal - minZ) / dist; // normalize [0, 1]
				const auto intensity = static_cast<unsigned char>(std::ceil(255 * (1 - currentVal)));
				rgb_t color;
				color.red = intensity;
    			color.blue = intensity;
				color.red = intensity;
				image.set_pixel(x, y, color);
			}
		}
		return image;
	}

	void writeBufferToFile(const std::string& filename)
	{
		auto image = convertToImage();
		image.vertical_flip();
		image.convert_to_grayscale();
		const std::string drive = "Z:\\";
		const std::string finalPath = drive + filename;
		bufferFilePath = finalPath;
		image.save_image(finalPath);
	}

	std::unordered_set<int> getVisibleMeshList()
	{
		std::unordered_set<int> visibleMeshIDs;
		static int bufferVal = 0;
		const std::string fileName = "zBuffer_" + std::to_string(bufferVal) + ".bmp";
		writeBufferToFile(fileName);
		bufferVal++;
		for(auto& d : data)
		{
			if (d.isAssigned) visibleMeshIDs.insert(d.meshID);
		}
		return visibleMeshIDs;
	}

	const char* getZBufferPath() const
	{
		return  bufferFilePath.c_str();
	}

private:
	int getArrayPos(const int x, const int y) const
	{
		return x + y * width;
	}

	bool isAssignedVal(const int pos) const
	{
		return data[pos].isAssigned;
	}
	
	bool isUnassignedVal(const int val) const
	{
		return !isAssignedVal(val);
	}

	void setValue(const int pos, const float val, const int meshID)
	{
		data[pos].setValue(val, meshID);
	}

	std::vector<ZBufferData> data;
	int width {};
	int height {};
	std::string bufferFilePath;
};

