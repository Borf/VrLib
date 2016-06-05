#pragma once

namespace vrlib
{
	namespace tien
	{
		class Node;
		class Renderer;

		class Component
		{
		protected:
			Component() { node = nullptr; }
			virtual ~Component() {}
			Node* node;
			friend class Node;
		public:
			virtual void update(float elapsedTime) {};
		};
	}
}