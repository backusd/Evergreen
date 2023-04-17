#include "JSONHelper.h"
#include <fstream>



json LoadJSONFile(const std::string& filename)
{
	json data;
	std::ifstream file;
	file.open(filename);
	if (file.is_open())
	{
		std::string fileData;
		std::ostringstream oss;
		oss << file.rdbuf();
		fileData = oss.str();
		file.close();

		// This can throw and somewhere up the call tree there needs to be a catch for json::parse_error
		try
		{
			data = json::parse(fileData);
		}
		catch (json::parse_error& e)
		{
			EG_ERROR("Caught json::parse_error when trying to parse file '{}'\nException Message: {}", filename, e.what());
		}
	}
	else
	{
		EG_ERROR("Failed to open file '{}'", filename);
	}

	return data;
}