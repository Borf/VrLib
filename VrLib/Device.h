#pragma once

#include <VrLib/VrLib.h>

#include <string>
#include <glm/glm.hpp>

namespace vrlib
{

	class DigitalDeviceDriverAdaptor;
	class AnalogDeviceDriverAdaptor;
	class PositionalDeviceDriverAdaptor;
	class TwoDimensionDeviceDriverAdaptor;
	class BinaryStream;


	enum DigitalState
	{
		OFF,
		ON,
		TOGGLE_ON,
		TOGGLE_OFF
	};
	BinaryStream& operator << (BinaryStream& stream, DigitalState &value); //todo: move these somewhere cooler?
	BinaryStream& operator >> (BinaryStream& stream, DigitalState &value);

	class Device
	{
	public:
		std::string name;

		virtual void init(std::string name) = 0;
		virtual bool isInitialized() const = 0;
	};



	template <class R, class T>
	class DeviceImpl : public Device
	{
		T*	driver;
		R	data;
	public:
		virtual void init(std::string name);
		virtual R getData() const;
		virtual bool isInitialized() const { return driver != NULL; }
	};




	class DigitalDevice : public DeviceImpl < DigitalState, DigitalDeviceDriverAdaptor >
	{};
	class AnalogDevice : public DeviceImpl < float, AnalogDeviceDriverAdaptor >
	{};
	class PositionalDevice : public DeviceImpl < glm::mat4, PositionalDeviceDriverAdaptor >
	{};
	class TwoDimensionDevice : public DeviceImpl < glm::vec2, TwoDimensionDeviceDriverAdaptor > {};



	class DeviceDriverAdaptor
	{
	public:
		virtual void updateDataMaster(BinaryStream& stream) = 0;
		virtual void updateDataSlave(BinaryStream& stream) = 0;
	};

	class DigitalDeviceDriverAdaptor : public DeviceDriverAdaptor
	{
		virtual DigitalState getData() = 0;
	public:
		DigitalState data;

		virtual void updateDataMaster(BinaryStream& stream);
		virtual void updateDataSlave(BinaryStream& stream);
	};
	class AnalogDeviceDriverAdaptor : public DeviceDriverAdaptor
	{
		virtual float getData() = 0;
	public:
		float data;

		virtual void updateDataMaster(BinaryStream& stream);
		virtual void updateDataSlave(BinaryStream& stream);
	};

	class PositionalDeviceDriverAdaptor : public DeviceDriverAdaptor
	{
		virtual glm::mat4 getData() = 0;
	public:
		glm::mat4 data;

		virtual void updateDataMaster(BinaryStream& stream);
		virtual void updateDataSlave(BinaryStream& stream);
	};

	class TwoDimensionDeviceDriverAdaptor : public DeviceDriverAdaptor {
		virtual glm::vec2 getData() = 0;
	public:
		glm::vec2 data;

		virtual void updateDataMaster(BinaryStream& stream);
		virtual void updateDataSlave(BinaryStream& stream);
	};


	class DeviceDriver
	{
	public:
		virtual void update() {};
		virtual DeviceDriverAdaptor* getAdaptor(std::string options) = 0;
	};



}