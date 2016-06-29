#include "BinaryStream.h"
#include <VrLib/Log.h>
#include <glm/glm.hpp>
#include <string.h>

namespace vrlib
{
	BinaryStream::BinaryStream(unsigned int bufsize)
	{
		this->BUFSIZE = bufsize;
		buffer = new char[BUFSIZE];
		readIndex = 0;
		writeIndex = 0;
	}

	BinaryStream::BinaryStream(std::string startValue, unsigned int bufsize)
	{
		this->BUFSIZE = glm::max(bufsize, startValue.size());
		readIndex = 0;
		buffer = new char[BUFSIZE];
		memcpy(buffer, startValue.c_str(), startValue.length());
		writeIndex = startValue.length();
	}

	BinaryStream::BinaryStream(char* startBuffer, unsigned int len, unsigned int bufsize)
	{
		this->BUFSIZE = glm::max(bufsize, len);
		buffer = new char[BUFSIZE];
		if (len > BUFSIZE)
			logger << "Error: data too large for binary stream" << Log::newline;
		readIndex = 0;
		memcpy(buffer, startBuffer, len);
		writeIndex = len;
	}


	BinaryStream::~BinaryStream()
	{
		delete[] buffer;
	}




	std::string BinaryStream::str()
	{
		return std::string(buffer + readIndex, writeIndex - readIndex);
	}

	BinaryStream& BinaryStream::operator<<(int value)
	{
		memcpy(buffer + writeIndex, (char*)&value, 4);
		writeIndex += 4;
		if (writeIndex > BUFSIZE)
			logger << "Error: too much data in buffer" << Log::newline;
		return *this;
	}

	BinaryStream& BinaryStream::operator<<(float value)
	{
		memcpy(buffer + writeIndex, (char*)&value, 4);
		writeIndex += 4;
		if (writeIndex > BUFSIZE)
			logger << "Error: too much data in buffer" << Log::newline;
		return *this;
	}

	BinaryStream& BinaryStream::operator<<(double value)
	{
		memcpy(buffer + writeIndex, (char*)&value, 8);
		writeIndex += 8;
		if (writeIndex > BUFSIZE)
			logger << "Error: too much data in buffer" << Log::newline;
		return *this;
	}

	BinaryStream& BinaryStream::operator<<(bool value)
	{
		memcpy(buffer + writeIndex, value ? "1" : "0", 1);
		writeIndex += 1;
		if (writeIndex > BUFSIZE)
			logger << "Error: too much data in buffer" << Log::newline;
		return *this;
	}

	BinaryStream& BinaryStream::operator<<(unsigned int value)
	{
		memcpy(buffer + writeIndex, (char*)&value, 4);
		writeIndex += 4;
		if (writeIndex > BUFSIZE)
			logger << "Error: too much data in buffer" << Log::newline;
		return *this;
	}

	BinaryStream& BinaryStream::operator<<(unsigned char value)
	{
		buffer[writeIndex] = value;
		writeIndex++;
		if (writeIndex > BUFSIZE)
			logger << "Error: too much data in buffer" << Log::newline;
		return *this;
	}
	BinaryStream& BinaryStream::operator<<(char value)
	{
		buffer[writeIndex] = value;
		writeIndex++;
		if (writeIndex > BUFSIZE)
			logger << "Error: too much data in buffer" << Log::newline;
		return *this;
	}


	BinaryStream& BinaryStream::operator<<(std::string value)
	{
		*this << value.size();
		memcpy(buffer + writeIndex, value.c_str(), value.size());
		writeIndex += value.size();
		if (writeIndex > BUFSIZE)
			logger << "Error: too much data in buffer" << Log::newline;
		return *this;
	}

	BinaryStream& BinaryStream::operator<<(const glm::vec2 &value)
	{
		return *this << value[0] << value[1];
	}

	BinaryStream& BinaryStream::operator<<(const glm::vec3 &value)
	{
		return *this << value[0] << value[1] << value[2];
	}

	BinaryStream& BinaryStream::operator<<(const glm::vec4 &value)
	{
		return *this << value[0] << value[1] << value[2] << value[3];
	}

	BinaryStream& BinaryStream::operator<<(const glm::mat4 &value)
	{
		for (int i = 0; i < 16; i++)
			*this << value[i / 4][i % 4];
		return *this;
	}

	BinaryStream& BinaryStream::operator<<(SerializableObject &value)
	{
		value.writeObject(*this);
		return *this;
	}











	//////////////////


	BinaryStream& BinaryStream::operator>>(int &value)
	{
		memcpy((char*)&value, buffer + readIndex, 4);
		readIndex += 4;
		if (readIndex > writeIndex)
			logger << "Trying to read too much from binarystream!" << Log::newline;
		return *this;

	}

	BinaryStream& BinaryStream::operator>>(float &value)
	{
		memcpy((char*)&value, buffer + readIndex, 4);
		readIndex += 4;
		if (readIndex > writeIndex)
			logger << "Trying to read too much from binarystream!" << Log::newline;
		return *this;
	}

	BinaryStream& BinaryStream::operator>>(double &value)
	{
		memcpy((char*)&value, buffer + readIndex, 8);
		readIndex += 8;
		if (readIndex > writeIndex)
			logger << "Trying to read too much from binarystream!" << Log::newline;
		return *this;
	}


	BinaryStream& BinaryStream::operator>>(unsigned int &value)
	{
		memcpy((char*)&value, buffer + readIndex, 4);
		readIndex += 4;
		if (readIndex > writeIndex)
			logger << "Trying to read too much from binarystream!" << Log::newline;
		return *this;
	}


	BinaryStream& BinaryStream::operator>>(bool &value)
	{
		value = buffer[readIndex] == '1';
		readIndex++;
		if (readIndex > writeIndex)
			logger << "Trying to read too much from binarystream!" << Log::newline;
		return *this;
	}


	BinaryStream& BinaryStream::operator>>(char &value)
	{
		value = buffer[readIndex];
		readIndex++;
		if (readIndex > writeIndex)
			logger << "Trying to read too much from binarystream!" << Log::newline;
		return *this;
	}

	BinaryStream& BinaryStream::operator>>(unsigned char &value)
	{
		value = buffer[readIndex];
		readIndex++;
		if (readIndex > writeIndex)
			logger << "Trying to read too much from binarystream!" << Log::newline;
		return *this;
	}


	BinaryStream& BinaryStream::operator>>(std::string &value)
	{
		size_t len;
		(*this) >> len;
		value = std::string(buffer + readIndex, len);
		readIndex += len;
		if (readIndex > writeIndex)
			logger << "Trying to read too much from binarystream!" << Log::newline;
		return *this;
	}


	BinaryStream& BinaryStream::operator>>(glm::vec2 &value)
	{
		return *this >> value[0] >> value[1];
	}

	BinaryStream& BinaryStream::operator>>(glm::vec3 &value)
	{
		return *this >> value[0] >> value[1] >> value[2];
	}


	BinaryStream& BinaryStream::operator>>(glm::vec4 &value)
	{
		return *this >> value[0] >> value[1] >> value[2] >> value[3];
	}

	BinaryStream& BinaryStream::operator>>(glm::mat4 &value)
	{
		for (int i = 0; i < 16; i++)
			*this >> value[i / 4][i % 4];
		return *this;
	}

	BinaryStream& BinaryStream::operator>>(SerializableObject &value)
	{
		value.readObject(*this);
		return *this;
	}

}
