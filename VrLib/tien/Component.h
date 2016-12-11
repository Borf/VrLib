#pragma once

#include <string>
#include <functional>
#include <vector>

namespace vrlib
{
	namespace json { class Value; }
	namespace tien
	{
		class Node;
		class Renderer;
		class Scene;

		class EditorBuilder
		{
		public:
			class TextBox { public: virtual std::string getText() const = 0; virtual void setText(const std::string &text) = 0; };

			virtual void addTitle(const std::string &name) = 0;

			virtual void beginGroup(const std::string &name, bool vertical = true) = 0;
			virtual void endGroup() = 0;
			virtual TextBox* addTextBox(const std::string &value, std::function<void(const std::string &)> onChange) = 0;
			virtual void addCheckbox(bool value, std::function<void(bool)> onChange) = 0;
			virtual void addButton(const std::string &value, std::function<void()> onClick) = 0;
			virtual void addComboBox(const std::string &value, const std::vector<std::string> &values, std::function<void(const std::string &)> onClick) = 0;


			enum BrowseType
			{
				Model,
				Node,
				Prefab
			};
			virtual void addBrowseButton(BrowseType type, std::function<void(const std::string &)> onClick) = 0;
		};

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
			virtual json::Value toJson(json::Value &meshes) const = 0;

			virtual void buildEditor(EditorBuilder* builder);
		};
	}
}