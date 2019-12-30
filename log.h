#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <string>

void Log(const std::string& logString);
void Log(const float* floatArray, int size);