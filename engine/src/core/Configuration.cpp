#include "Configuration.hpp"
#include <fstream>
Configuration::Configuration()
{
}

Configuration Configuration::fromFile(const char* path)
{
	Configuration config = Configuration();
	std::ifstream file(path);
	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);
		auto delimPos = line.find('=');
		if (delimPos == std::string::npos)
			continue;
		std::string key = line.substr(0, delimPos);
		std::string value = line.substr(delimPos + 1);
		config.settings[key] = value;
	}

	return config;
}


