#pragma once
#include <memory>

#include <Configuration.hpp>

class Rehti
{
public:

	static int initializeRehti(const Configuration& configuration);
	static int initializeRehti();

	static void cleanupRehti();

	Rehti(Rehti& other) = delete;
	Rehti& operator=(const Rehti&) = delete;


	void eventLoop();

private:
	class RehtiImpl;
	friend class RehtiImpl;
	Rehti();
	static std::unique_ptr<RehtiImpl> instance;
};