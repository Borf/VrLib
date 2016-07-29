#pragma once

namespace vrlib
{
	namespace json { class Value; }
	namespace tien
	{
		class Node;
		class Renderer;
		class Scene;

		class Component
		{
		protected:
			Component() { node = nullptr; }
			virtual ~Component() {}
			Node* node;
			friend class Node;
		public:
			virtual void update(float elapsedTime, Scene& scene) {};
			virtual void postUpdate(Scene& scene) {};
			virtual json::Value toJson() const = 0;
		};
	}
}