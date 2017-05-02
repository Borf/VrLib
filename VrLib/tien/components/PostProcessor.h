#pragma once

#include "../Component.h"
#include <VrLib/gl/shader.h>
#include <list>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class PostProcessor : public Component
			{
				class ShaderParameterBase
				{
				public:
					int uniformIndex;
					std::string name;
					virtual void set(vrlib::gl::UntypedShader* shader) const = 0;
					virtual void buildEditor(EditorBuilder * builder) const = 0;
				};

				template <class T> 
				class ShaderParameter : public ShaderParameterBase
				{
				public:
					T* value;
					void set(vrlib::gl::UntypedShader* shader) const override
					{
						shader->setUniform(uniformIndex, *value);
					}
					virtual void buildEditor(EditorBuilder * builder) const { throw "oops"; };
				};
			protected:
				vrlib::gl::UntypedShader* shader;
				std::string name;
				std::list<ShaderParameterBase*> parameters;


				PostProcessor(const std::string &name) { this->name = name; }

				template <class T>
				void loadShader(const std::string &vert, const std::string &frag)
				{
					shader = new vrlib::gl::UntypedShader(vert, frag);
					shader->bindAttributeLocation("a_position", 0);
					shader->link();
					shader->bindFragLocation("fragColor", 0);
					shader->registerUniform(20, "s_texture");//TODO: don't hardcode 20/21
					shader->registerUniform(21, "s_depth");
					shader->use();
					shader->setUniform(20, 0);
					shader->setUniform(21, 1);
				}
				
				template<class T, class Q>
				void addShaderParameter(T t, const std::string &name, Q &value)
				{
					shader->registerUniform((int)t, name);
					ShaderParameter<Q>* p = new ShaderParameter<Q>();
					p->name = name;
					p->uniformIndex = (int)t;
					p->value = &value;
					parameters.push_back(p);
				}

			public:
				int passes = 1;
				virtual void pass(int index) {};
				virtual void runPass(int index);
				virtual void buildEditor(EditorBuilder * builder, bool folded);

			};

			template<>
			void PostProcessor::ShaderParameter<float>::buildEditor(EditorBuilder* builder) const
			{
				builder->addFloatBox(*value, 0, 10, [this](float newValue) { *value = newValue; });
			}

		}
	}
}