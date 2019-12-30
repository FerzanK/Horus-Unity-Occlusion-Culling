#include "log.h"

auto my_logger = spdlog::basic_logger_st("Log", "log.txt");

void Log(const std::string& logString)
{
	my_logger->info(logString);
	my_logger->flush();
}

void Log(const float* floatArray, int size)
{
	for(int i = 0; i < size; i++)
	{
		Log(std::to_string(floatArray[i]));
	}
}