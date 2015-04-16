#pragma once

#include <VrLib/VrLib.h>
#include <VrLib/BinaryStream.h>

namespace vrlib
{
	class ClusterDataBase
	{
	public:
		void init();
		bool isLocal();

		virtual void updateDataMaster(BinaryStream &stream) = 0;
		virtual void updateDataSlave(BinaryStream &stream) = 0;
		virtual int getEstimatedSize() = 0;
	};

	template <class T>
	class ClusterData : public ClusterDataBase
	{
		T* data;
	public:
		ClusterData()
		{
			data = new T();
		}
		~ClusterData()
		{
			delete data;
		}

		T* operator ->()
		{
			return data;
		}

		T& operator *()
		{
			return *data;
		}

		virtual void updateDataMaster(BinaryStream &stream)
		{
			stream << *data;
		}

		virtual void updateDataSlave(BinaryStream &stream)
		{
			stream >> *data;
		}

		virtual int getEstimatedSize()
		{
			return data->getEstimatedSize();
		}

	};

	template<>
	int ClusterData<std::string>::getEstimatedSize()
	{
		return 4 + data->size();
	}
}