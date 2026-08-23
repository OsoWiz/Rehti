#pragma once

namespace Rehti
{
	namespace Units
	{

		class Speed
		{
		public:
			enum class Unit
			{
				MetersPerSecond,
				KilometersPerHour,
				MilesPerHour,
				FeetPerSecond
			};

		};

		class Distance
		{
		public:
			enum class Unit
			{
				Meters,
				Kilometers,
				Miles,
				Feet
			};
		};




	}
}