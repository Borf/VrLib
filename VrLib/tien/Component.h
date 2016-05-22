#pragma once

namespace vrlib
{
	namespace tien
	{
		class Component
		{
		protected:
			Component() {}
			virtual ~Component() {}

		public:
			virtual void update(float elapsedTime) {};
		};
	}
}