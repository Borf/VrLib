#pragma once

#include <string>

#include <VrLib/VrLib.h>
#include <glm/glm.hpp>

namespace vrlib
{
	class BinaryStream;

	class SerializableObject
	{
	public:
		virtual int getEstimatedSize() { return 1024; };

		virtual void writeObject(BinaryStream &writer) = 0;
		virtual void readObject(BinaryStream &reader) = 0;
	};

	class BinaryStream
	{
		unsigned int BUFSIZE;
		char* buffer;
		unsigned int readIndex;
		unsigned int writeIndex;


	public:
		BinaryStream(unsigned int bufsize = 1024);
		BinaryStream(std::string start, unsigned int bufsize = 1024);
		BinaryStream(char* buf, unsigned int len, unsigned int bufsize = 1024);
		~BinaryStream();


		std::string str();



		BinaryStream& operator << (int value);
		BinaryStream& operator >> (int &value);

		BinaryStream& operator << (unsigned int value);
		BinaryStream& operator >> (unsigned int &value);

		BinaryStream& operator << (float value);
		BinaryStream& operator >> (float &value);

		BinaryStream& operator << (double value);
		BinaryStream& operator >> (double &value);

		BinaryStream& operator << (char value);
		BinaryStream& operator >> (char &value);

		BinaryStream& operator << (unsigned char value);
		BinaryStream& operator >> (unsigned char &value);

		BinaryStream& operator << (bool value);
		BinaryStream& operator >> (bool &value);

		BinaryStream& operator << (std::string value);
		BinaryStream& operator >> (std::string &value);


		BinaryStream& operator << (SerializableObject &value);
		BinaryStream& operator >> (SerializableObject &value);

		BinaryStream& operator << (const glm::vec3 &value);
		BinaryStream& operator >> (glm::vec3 &value);

		BinaryStream& operator << (const glm::vec2 &value);
		BinaryStream& operator >> (glm::vec2 &value);

		BinaryStream& operator << (const glm::vec4 &value);
		BinaryStream& operator >> (glm::vec4 &value);

		BinaryStream& operator << (const glm::mat4 &value);
		BinaryStream& operator >> (glm::mat4 &value);

	};
}