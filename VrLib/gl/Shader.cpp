#include <gl/glew.h>
#include "Shader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <VrLib/Log.h>

namespace vrlib
{
	namespace gl
	{

		void printShaderInfoLog(std::string fileName, GLuint obj)
		{
			int infologLength = 0;
			int charsWritten = 0;
			char *infoLog;

			glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

			if (infologLength > 0)
			{
				infoLog = (char *)malloc(infologLength);
				glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
				if (infoLog[0] != '\0')
				{
					printf("%s\n", fileName.c_str());
					printf("%s\n", infoLog);
					getchar();
				}
				free(infoLog);
			}
		}
		void printProgramInfoLog(std::string fileName, GLuint obj)
		{
			int infologLength = 0;
			int charsWritten = 0;
			char *infoLog;

			glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

			if (infologLength > 0)
			{
				infoLog = (char *)malloc(infologLength);
				glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
				if (infoLog[0] != '\0')
				{
					printf("%s\n", fileName.c_str());
					printf("%s\n", infoLog);
					getchar();
				}
				free(infoLog);
			}
		}


		ShaderProgram::ShaderProgram(std::string vertShader, std::string fragShader)
		{
			programId = glCreateProgram();
			addVertexShader(vertShader);
			addFragmentShader(fragShader);
		}

		ShaderProgram::~ShaderProgram(void)
		{
		}

		void ShaderProgram::bindAttributeLocation(std::string name, int position)
		{
			glBindAttribLocation(programId, position, name.c_str());
		}

		void ShaderProgram::bindFragLocation(std::string name, int position)
		{
			if (glBindFragDataLocation != NULL)
				glBindFragDataLocation(programId, position, name.c_str());
		}

		void ShaderProgram::link()
		{
			glLinkProgram(programId);
		}

		void ShaderProgram::use()
		{
			glUseProgram(programId);
		}

		void ShaderProgram::addVertexShader(std::string filename)
		{
			Shader* s = new Shader(filename, GL_VERTEX_SHADER);
			shaders.push_back(s);
			glAttachShader(programId, s->shaderId);
		}

		void ShaderProgram::addFragmentShader(std::string filename)
		{
			Shader* s = new Shader(filename, GL_FRAGMENT_SHADER);
			shaders.push_back(s);
			glAttachShader(programId, s->shaderId);
		}

		void ShaderProgram::setUniformMatrix4(const std::string &name, const glm::mat4 &matrix)
		{
			glUniformMatrix4fv(getUniformLocation(name), 1, 0, glm::value_ptr(matrix));
		}

		void ShaderProgram::setUniformMatrix3(const std::string &name, const glm::mat3 &matrix)
		{
			glUniformMatrix3fv(getUniformLocation(name), 1, 0, glm::value_ptr(matrix));
		}

		void ShaderProgram::setUniformInt(const std::string &name, int value)
		{
			glUniform1i(getUniformLocation(name), value);
		}

		void ShaderProgram::setUniformFloat(const std::string &name, float value)
		{
			glUniform1f(getUniformLocation(name), value);
		}

		void ShaderProgram::setUniformBool(const std::string &name, bool value)
		{
			glUniform1i(getUniformLocation(name), value ? 1 : 0);
		}


		void ShaderProgram::setUniformVec2(const std::string &name, const glm::vec2 &value)
		{
			glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
		}

		void ShaderProgram::setUniformVec3(const std::string &name, const glm::vec3 &value)
		{
			glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
		}

		void ShaderProgram::setUniformVec4(const std::string& name, const glm::vec4 &value)
		{
			glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
		}


		GLuint ShaderProgram::getUniformLocation(const std::string &name)
		{
			if (uniformLocations.find(name) == uniformLocations.end())
				uniformLocations[name] = glGetUniformLocation(programId, name.c_str());
			return uniformLocations[name];
		}

		bool ShaderProgram::hasShaders()
		{
			return GLEW_VERSION_1_5 == GL_TRUE;
		}

		ShaderProgram::Shader::Shader(std::string fileName, GLenum type)
		{
			std::string data = "";
			std::string line;
			std::ifstream pFile(fileName);
			while (!pFile.eof() && pFile.good())
			{
				std::getline(pFile, line);
				data += line + "\n";
			}

			shaderId = glCreateShader(type);
			const char* d2 = data.c_str();
			glShaderSource(shaderId, 1, &d2, NULL);
			glCompileShader(shaderId);
			printShaderInfoLog(fileName, shaderId);
		}


		void UntypedShader::addShader(std::string fileName, int shaderType)
		{
			std::string data = "";
			std::string line;
			std::ifstream pFile(fileName);
			while (!pFile.eof() && pFile.good())
			{
				std::getline(pFile, line);
				data += line + "\n";
			}

			GLuint shaderId = glCreateShader(shaderType);
			const char* d2 = data.c_str();
			glShaderSource(shaderId, 1, &d2, NULL);
			glCompileShader(shaderId);
			printShaderInfoLog(fileName, shaderId);

			glAttachShader(programId, shaderId);
		}

		UntypedShader::UntypedShader(std::string vertShader, std::string fragShader)
		{
			programId = glCreateProgram();
			addShader(vertShader, GL_VERTEX_SHADER);
			addShader(fragShader, GL_FRAGMENT_SHADER);
			printProgramInfoLog(vertShader + "/" + fragShader, programId);
		}

		void UntypedShader::bindAttributeLocation(std::string name, int position)
		{
			glBindAttribLocation(programId, position, name.c_str());
		}

		void UntypedShader::bindFragLocation(std::string name, int position)
		{
			glBindFragDataLocation(programId, position, name.c_str());
		}

		void UntypedShader::link()
		{
			glLinkProgram(programId);
		}

		void UntypedShader::use()
		{
			glUseProgram(programId);
		}

		void UntypedShader::registerUniform(int id, std::string value)
		{
			if ((int)uniformLocations.size() <= id)
				uniformLocations.resize(id + 1, -1);
			uniformLocations[id] = glGetUniformLocation(programId, value.c_str());
			if (uniformLocations[id] < 0)
				logger << "Error registering uniform " << value << Log::newline;
		}

		void UntypedShader::setUniform(int id, int value)
		{
			glUniform1i(uniformLocations[id], value);
		}

		void UntypedShader::setUniform(int id, float value)
		{
			glUniform1f(uniformLocations[id], value);
		}

		void UntypedShader::setUniform(int id, const glm::vec2 &value)
		{
			glUniform2fv(uniformLocations[id], 1, glm::value_ptr(value));
		}

		void UntypedShader::setUniform(int id, const glm::vec3 &value)
		{
			glUniform3fv(uniformLocations[id], 1, glm::value_ptr(value));
		}

		void UntypedShader::setUniform(int id, const glm::vec4 &value)
		{
			glUniform4fv(uniformLocations[id], 1, glm::value_ptr(value));
		}

		void UntypedShader::setUniform(int id, const glm::mat3 &value)
		{
			glUniformMatrix3fv(uniformLocations[id], 1, 0, glm::value_ptr(value));
		}
		void UntypedShader::setUniform(int id, const glm::mat4 &value)
		{
			glUniformMatrix4fv(uniformLocations[id], 1, 0, glm::value_ptr(value));
		}

	}
}
