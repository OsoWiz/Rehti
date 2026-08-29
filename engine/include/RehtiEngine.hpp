#pragma once
#include <memory>
#include <typeinfo>

#include <flecs.h>

#include <Configuration.hpp>
#include <ResourceManager.hpp>
#include <RehtiException.hpp>
#include <RehtiGraphics.hpp>
#include <RehtiAudio.hpp>
#include <RehtiPhysics.hpp>
#include <RehtiInput.hpp>
#include <RehtiNetworking.hpp>

class RehtiEngine
{
public:
	static int initializeRehti(const Configuration& configuration);
	static int initializeRehti();
	static void cleanupRehti();

	template<class SubSystemType>
	static SubSystemType& getSubSystem();

	IEngineSubsystem* getSubSystemByName(const std::string& name);

	RehtiEngine(RehtiEngine& other) = delete;
	RehtiEngine& operator=(const RehtiEngine&) = delete;

	/**
	 * @brief Returns the world used by the engine and all of the subsystems. Rehti must be initialized before calling.
	 *  Using Rehti -defined component adds them automatically to related systems.
	 *  Using your own allows you to define application specific systems and components.
	 * @return the world used by the engine. 
	 */
	static flecs::world& getWorld();

	/**
	 * @brief Progresses the world by reading events and updating enabled subsystems.
	 */
	void progress();

private:
	class RehtiImpl;
	friend class RehtiImpl;
	RehtiEngine();
	static std::unique_ptr<RehtiImpl> instance;

	static IEngineSubsystem* getSubSystemByType(const std::type_info& type);
};

template<class SubSystemType>
inline SubSystemType& RehtiEngine::getSubSystem()
{
	IEngineSubsystem* subsystem = getSubSystemByType(typeid(SubSystemType));
	if (subsystem)
	{
		return *dynamic_cast<SubSystemType*>(subsystem);
	}
	else
	{
        throw RehtiException(RehtiError::SUBSYSTEM_NOT_INITIALIZED, "Subsystem not registered or the requested type is not a subsystem.");
	}
}
