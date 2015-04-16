#include "Device.h"
#include <VrLib/Kernel.h>
#include <VrLib/Log.h>
#include <VrLib/BinaryStream.h>

namespace vrlib
{
	BinaryStream& operator<<(BinaryStream& stream, DigitalState &value)
	{
		return stream << (int)value;
	}
	BinaryStream& operator>>(BinaryStream& stream, DigitalState &value)
	{
		int &tmp = (int&)value;
		return (stream >> tmp);
	}

	void DigitalDeviceDriverAdaptor::updateDataMaster(BinaryStream& stream)
	{
		data = getData();
		stream << data;
	}

	void DigitalDeviceDriverAdaptor::updateDataSlave(BinaryStream& stream)
	{
		stream >> data;
	}

	void PositionalDeviceDriverAdaptor::updateDataMaster(BinaryStream& stream)
	{
		data = getData();
		stream << data;
	}

	void PositionalDeviceDriverAdaptor::updateDataSlave(BinaryStream& stream)
	{
		stream >> data;
	}

	void AnalogDeviceDriverAdaptor::updateDataMaster(BinaryStream& stream)
	{
		data = getData();
		stream << data;
	}

	void AnalogDeviceDriverAdaptor::updateDataSlave(BinaryStream& stream)
	{
		stream >> data;
	}

	void TwoDimensionDeviceDriverAdaptor::updateDataMaster(BinaryStream& stream) {
		data = getData();
		stream << data;
	}

	void TwoDimensionDeviceDriverAdaptor::updateDataSlave(BinaryStream& stream) {
		stream >> data;
	}



	template <class R, class T>
	void DeviceImpl<R, T>::init(std::string name)
	{
		this->name = name;
		driver = (T*)Kernel::getInstance()->getDeviceDriverAdaptor(name);
		//Kernel::getInstance()->registerDevice(this);
	}

	template <class R, class T>
	R DeviceImpl<R, T>::getData()
	{
		if (driver != NULL)
			return driver->data;
		throw("No Driver Loader"); //have to throw something, otherwise I get scolded on by the compiler
	}
}