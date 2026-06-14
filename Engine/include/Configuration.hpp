#pragma once
#include <optional>
#include <string>
#include <unordered_map>

class Configuration
{
public:
	static Configuration fromFile(const char* path);

	Configuration(std::unordered_map<std::string, std::string> settings)
		: settings(settings) {
	}

	template<typename T>
	std::optional<T> getSetting(const std::string& key) const
	{
		const auto it = settings.find(key);
		if (it == settings.end())
			return std::nullopt;

		const std::string& value = it->second;
		if (value.empty())
			return std::nullopt;

		if constexpr (std::is_same_v<T, int>)
			return std::stoi(value);
		else if constexpr (std::is_same_v<T, unsigned int>)
			return std::stoul(value);
		else if constexpr (std::is_same_v<T, float>)
			return std::stof(value);
		else if constexpr (std::is_same_v<T, double>)
			return std::stod(value);
		else if constexpr (std::is_same_v<T, bool>)
			return value == "true" || value == "1";
		else if constexpr (std::is_same_v<T, std::string>)
			return value;
		else
			static_assert(!sizeof(T*), "Unsupported type");
	}

	template<typename T>
	T getOrElse(const std::string& key, T alternative) const
	{
		auto val = getSetting<T>(key);
		if (val.has_value())
			return val.value();
		return alternative;
	}

private:
	Configuration();
	std::unordered_map<std::string, std::string> settings;
};