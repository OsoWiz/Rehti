#pragma once
#include <stdexcept>

enum class RehtiError : uint16_t
{
	SUBSYSTEM_ALREADY_INITIALIZED,
	SUBSYSTEM_NOT_INITIALIZED,
	RESOURCE_NOT_FOUND,
	MALFORMED_RESOURCE,
	INITIALIZATION_FAILURE,
	// perhaps specialize each exception type to its own class.
	GRAPHICS_INITIALIZATION_FAILED,
	GRAPHICS_OUT_OF_MEMORY,
	GRAPHICS_GENERIC_ERROR,

	UNKNOWN_ERROR
};

class RehtiException : public std::runtime_error
{
public:
	RehtiException(RehtiError errorCode, const std::string& additionalInformation = "");

};