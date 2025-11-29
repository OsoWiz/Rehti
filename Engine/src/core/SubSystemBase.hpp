#pragma once

template<class DerivedSystem>
class SubSystemBase
{
	static DerivedSystem* getInstance()
	{
		return DerivedSystem::getInstance();
	}

};

