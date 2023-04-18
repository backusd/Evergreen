#pragma once
#include "pch.h"
#include <Evergreen.h>


json ReadJSONFile(const std::string& filename);
void WriteJSONToFile(const json& data, const std::string& filename);