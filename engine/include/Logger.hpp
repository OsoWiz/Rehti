#pragma once

#include <iostream>
#include <sstream>
#include <string>

class Configuration;

class Logger
{
public:
	enum class Level
	{
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	static Logger& instance();

	void initialize(const Configuration& configuration);
	void shutdown();
	bool isInitialized() const;

	void log(Level level, const std::string& message);
	void log(const std::string& message);

	static void trace(const std::string& message);
	static void debug(const std::string& message);
	static void info(const std::string& message);
	static void warning(const std::string& message);
    static void error(const std::string& message);
	static void fatal(const std::string& message);

	Logger& operator<<(const std::string& message);

	template<typename T>
	Logger& operator<<(const T& value)
	{
		streamBuffer << value;
		return *this;
	}

	Logger& operator<<(std::ostream& (*manip)(std::ostream&));

private:
	void flushStreamBuffer();

	Logger() = default;
	bool initialized = false;
	std::ostringstream streamBuffer;
	static Level defaultLevel;
};
