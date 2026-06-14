#include "Logger.hpp"

#include "Configuration.hpp"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <iostream>
#include <unordered_map>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

namespace
{
	logging::trivial::severity_level parseLevel(const std::string& value)
	{
		if (value == "trace") return logging::trivial::trace;
		if (value == "debug") return logging::trivial::debug;
		if (value == "info") return logging::trivial::info;
		if (value == "warning") return logging::trivial::warning;
		if (value == "error") return logging::trivial::error;
		if (value == "fatal") return logging::trivial::fatal;
		return logging::trivial::info;
	}
}

Logger::Level Logger::defaultLevel = Logger::Level::Info;

Logger& Logger::instance()
{
	static Logger logger;
	return logger;
}

void Logger::initialize(const Configuration& configuration)
{
	if (initialized)
	{
		return;
	}

	bool logToStdout = configuration.getOrElse<bool>("logging_to_stdout", true);
	std::string minLevelSetting = configuration.getOrElse<std::string>("logging_level", "info");
	logging::trivial::severity_level minLevel = parseLevel(minLevelSetting);

	if (logToStdout)
	{
		logging::add_console_log(
			std::clog,
			keywords::format = "[%TimeStamp%] <%Severity%>: %Message%"
		);
	}

	logging::core::get()->set_filter(logging::trivial::severity >= minLevel);
	logging::add_common_attributes();
	initialized = true;
}

void Logger::shutdown()
{
	if (!initialized)
	{
		return;
	}

	logging::core::get()->remove_all_sinks();
	initialized = false;
}

bool Logger::isInitialized() const
{
	return initialized;
}

void Logger::log(Level level, const std::string& message)
{
	if (!initialized)
	{
		Configuration defaults(std::unordered_map<std::string, std::string>{});
		initialize(defaults);
	}

	switch (level)
	{
		case Level::Trace:
			BOOST_LOG_TRIVIAL(trace) << message;
			break;
		case Level::Debug:
			BOOST_LOG_TRIVIAL(debug) << message;
			break;
		case Level::Info:
			BOOST_LOG_TRIVIAL(info) << message;
			break;
		case Level::Warning:
			BOOST_LOG_TRIVIAL(warning) << message;
			break;
		case Level::Error:
            BOOST_LOG_TRIVIAL(error) << message;
			break;
		case Level::Fatal:
			BOOST_LOG_TRIVIAL(fatal) << message;
			break;
	}
}

void Logger::log(const std::string& message)
{
	instance().log(defaultLevel, message);
}

void Logger::trace(const std::string& message)
{
	instance().log(Level::Trace, message);
}

void Logger::debug(const std::string& message)
{
	instance().log(Level::Debug, message);
}

void Logger::info(const std::string& message)
{
	instance().log(Level::Info, message);
}

void Logger::warning(const std::string& message)
{
	instance().log(Level::Warning, message);
}

void Logger::error(const std::string& message)
{
	instance().log(Level::Error, message);
}

void Logger::fatal(const std::string& message)
{
	instance().log(Level::Fatal, message);
}

Logger& Logger::operator<<(const std::string& message)
{
	streamBuffer << message;
	return *this;
}

Logger& Logger::operator<<(std::ostream& (*manip)(std::ostream&))
{
	manip(streamBuffer);

	using StreamManipulator = std::ostream& (*)(std::ostream&);
	if (manip == static_cast<StreamManipulator>(std::endl<char, std::char_traits<char>>) ||
		manip == static_cast<StreamManipulator>(std::flush<char, std::char_traits<char>>) ||
		manip == static_cast<StreamManipulator>(std::ends<char, std::char_traits<char>>))
	{
		flushStreamBuffer();
	}

	return *this;
}

void Logger::flushStreamBuffer()
{
	const std::string message = streamBuffer.str();
	if (!message.empty())
	{
		log(defaultLevel, message);
	}

	streamBuffer.str({});
	streamBuffer.clear();
}
