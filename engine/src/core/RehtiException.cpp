#include "RehtiException.hpp"

std::string errorMessageFromCode(RehtiError errorCode)
{
	switch (errorCode)
	{
	case RehtiError::SUBSYSTEM_ALREADY_INITIALIZED:
		return "Core subsystem is already initialized";
	case RehtiError::SUBSYSTEM_NOT_INITIALIZED:
		return "Core subsystem is not initialized";
	case RehtiError::RESOURCE_NOT_FOUND:
		return "Requested resource was not found";
	case RehtiError::MALFORMED_RESOURCE:
		return "Resource is malformed or invalid";
	case RehtiError::INITIALIZATION_FAILURE:
		return "Failed to initialize an object/resource";
	case RehtiError::GRAPHICS_INITIALIZATION_FAILED:
		return "Failed to initialize graphics subsystem";
	case RehtiError::GRAPHICS_OUT_OF_MEMORY:
		return "Graphics subsystem ran out of memory";
	case RehtiError::GRAPHICS_GENERIC_ERROR:
		return "An error occurred in the graphics subsystem";
	default:
		return "An unknown error occurred";
	}
}

// The format is "RehtiException: <error message>[: additional information]"
RehtiException::RehtiException(RehtiError errorCode, const std::string& additionalInformation)
	: std::runtime_error("RehtiException: " 
		+ errorMessageFromCode(errorCode) 
		+ std::string((additionalInformation.empty()) ? additionalInformation : ": " + additionalInformation))
{
}
