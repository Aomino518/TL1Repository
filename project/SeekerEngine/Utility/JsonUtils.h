#pragma once
#include <string>
#include "Vector3.h"
#include <nlohmann/json.hpp>

namespace JsonUtils
{
	std::string ReadFilePath(const std::string& path);
	void WriteTextFile(const std::string& path, const std::string& text);
};

