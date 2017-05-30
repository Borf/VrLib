#pragma once

#include <string>
#include <functional>
#include <vector>
#include <VrLib/json.hpp>
#include <glm/glm.hpp>

class TienEdit;

namespace vrlib
{
	namespace tien
	{
		class Node;
		class Renderer;
		class Scene;

		namespace components
		{
			class Light;
		}

		class EditorBuilder
		{
		public:
			class TextComponent { public: virtual std::string getText() const = 0; virtual void setText(const std::string &text) = 0; };
			class BoolComponent { public: virtual bool getValue() const = 0; virtual void setValue(bool newValue) = 0; };
			class ColorComponent { public: virtual glm::vec4 getColor() const = 0; virtual void setColor(const glm::vec4 &color) = 0; };
			class FloatComponent { public: virtual float getValue() const = 0; virtual void setValue(float value) = 0; };

			virtual TextComponent* addTitle(const std::string &name) = 0;

			virtual void beginGroup(const std::string &name, bool vertical = true) = 0;
			virtual void endGroup() = 0;
			virtual TextComponent* addTextBox(const std::string &value, std::function<void(const std::string &)> onChange) = 0;
			virtual TextComponent* addTextureBox(const std::string &value, std::function<void(const std::string &)> onChange) = 0;
			virtual TextComponent* addModelBox(const std::string &value, std::function<void(const std::string &)> onChange) = 0;
			virtual TextComponent* addPrefabBox(const std::string &value, std::function<void(const std::string &)> onChange) = 0;
			virtual ColorComponent* addColorBox(const glm::vec4 &value, std::function<void(const glm::vec4 &)> onChange) = 0;

			virtual FloatComponent* addFloatBox(float value, float min, float max, std::function<void(float)> onChange) = 0;

			virtual void addCheckbox(bool value, std::function<void(bool)> onChange) = 0;
			virtual void addButton(const std::string &value, std::function<void()> onClick) = 0;
			virtual void addSmallButton(const std::string &value, std::function<void()> onClick) = 0;
			virtual TextComponent* addComboBox(const std::string &value, const std::vector<std::string> &values, std::function<void(const std::string &)> onClick) = 0;
			virtual TextComponent* addLabel(const std::string &value) = 0;
			virtual void addDivider() = 0;
			virtual void updateComponentsPanel() = 0;
			enum BrowseType
			{
				Model,
				Texture,
				Node,
				Prefab
			};


			virtual std::string toString(float value) const;
		};

		class Component
		{
		protected:
			Component() { node = nullptr; }
			virtual ~Component() {};
			Node* node;
			friend class Node;
			friend class Renderer;
			friend class components::Light;
		public:
			virtual void update(float elapsedTime, Scene& scene) {};
			virtual void postUpdate(Scene& scene) {};
			virtual json toJson(json &meshes) const;
			virtual void updateNodePointer(Node* oldNode, Node* newNode) {}

			virtual void buildEditor(EditorBuilder* builder, bool folded);
		};
	}
}