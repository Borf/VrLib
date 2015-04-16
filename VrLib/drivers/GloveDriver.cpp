#include <VrLib/drivers/GloveDriver.h>
#include <VrLib/Log.h>

#include <fglove.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace vrlib
{

	DeviceDriverAdaptor* GloveDeviceDriver::getAdaptor(std::string options)
	{
		GloveFinger  selectedSensor;
		if (options == "LeftLittle")
			selectedSensor = LeftLittle;
		else if (options == "LeftRing")
			selectedSensor = LeftRing;
		else if (options == "LeftMiddle")
			selectedSensor = LeftMiddle;
		else if (options == "LeftIndex")
			selectedSensor = LeftIndex;
		else if (options == "LeftThumb")
			selectedSensor = LeftThumb;
		else if (options == "RightLittle")
			selectedSensor = RightLittle;
		else if (options == "RightRing")
			selectedSensor = RightRing;
		else if (options == "RightMiddle")
			selectedSensor = RightMiddle;
		else if (options == "RightIndex")
			selectedSensor = RightIndex;
		else if (options == "RightThumb")
			selectedSensor = RightThumb;
		else
			logger << "Invalid  " + options << Log::newline;

		return new GloveDeviceDriverAdaptor(this, selectedSensor);
	}

	GloveDeviceDriver::GloveDeviceDriver()
	{
		pGloveLeft = NULL;
		pGloveRight = NULL;
		pGlove = NULL;
		usbGloveLeft = 0;
		usbGloveRight = 0;

		char szPortToOpen[100];
		unsigned short aPID[5];
		int nNumFound = 5;
		char gloves[5];
		int GloveCount = 0;
		for (int idx = 0; idx < 10; idx++) data[idx] = 0;
		for (int idx = 0; idx < fingerpos->length(); idx++) fingerpos[idx] = glm::mat4();


		fdScanUSB(aPID, nNumFound);
		for (int c = 0; c < nNumFound; c++)
		{
			logger << "Available USB Gloves:" << Log::newline;
			switch (aPID[c])
			{
			case DG14U_R:
				//printf("Data Glove 14 Ultra Right\n");
				gloves[GloveCount] = 'R';
				GloveCount++;
				break;
			case DG14U_L:
				//printf("Data Glove 14 Ultra Left\n");
				gloves[GloveCount] = 'L';
				GloveCount++;
				break;
			case DG5U_R:
				//printf("Data Glove 5 Ultra Right \n");
				gloves[GloveCount] = 'r';
				GloveCount++;
				usbGloveRight = c;
				break;
			case DG5U_L:
				//printf("Data Glove 5 Ultra Left  \n");
				gloves[GloveCount] = 'l';
				GloveCount++;
				usbGloveLeft = c;
				break;
			default:
				printf("Unknown\n");
			}
		}


		for (int i = 0; i < GloveCount; i++)
		{
			if (gloves[i] == 'r')
				sprintf(szPortToOpen, "USB%i", usbGloveRight);
			else
				sprintf(szPortToOpen, "USB%i", usbGloveLeft);
			pGlove = fdOpen(szPortToOpen);

			switch (gloves[i])
			{
			case 'l': pGloveLeft = pGlove; break;
			case 'r': pGloveRight = pGlove; break;
			}
		}
	}

	void GloveDeviceDriver::update()
	{
		glm::mat4 mat = glm::mat4();
		data[(int)LeftLittle] = (fdGetSensorScaled(pGloveLeft, FD_LITTLENEAR));
		data[(int)LeftRing] = (fdGetSensorScaled(pGloveLeft, FD_RINGNEAR));
		data[(int)LeftMiddle] = (fdGetSensorScaled(pGloveLeft, FD_MIDDLENEAR));
		data[(int)LeftIndex] = (fdGetSensorScaled(pGloveLeft, FD_INDEXNEAR));
		data[(int)LeftThumb] = (fdGetSensorScaled(pGloveLeft, FD_THUMBNEAR));

		data[(int)RightLittle] = (fdGetSensorScaled(pGloveRight, FD_LITTLENEAR));
		data[(int)RightRing] = (fdGetSensorScaled(pGloveRight, FD_RINGNEAR));
		data[(int)RightMiddle] = (fdGetSensorScaled(pGloveRight, FD_MIDDLENEAR));
		data[(int)RightIndex] = (fdGetSensorScaled(pGloveRight, FD_INDEXNEAR));
		data[(int)RightThumb] = (fdGetSensorScaled(pGloveRight, FD_THUMBNEAR));

		//Right
		glm::mat4 tr = glm::translate(mat, glm::vec3(0, 0, 0));
		glm::mat4 ro = glm::rotate(tr, glm::radians(90.0f), glm::vec3(-1, 0, 0));
		glm::mat4 sc = glm::scale(ro, glm::vec3(0.009, 0.009, 0.009));
		fingerpos[(int)RightLittle] = calcFinger(sc, data[(int)RightLittle] * 90, -3.579f, 1.412f, 1.438f, false);
		fingerpos[(int)RightRing] = calcFinger(sc, data[(int)RightRing] * 90, -1.858f, 2.292f, 1.9f, false);
		fingerpos[(int)RightMiddle] = calcFinger(sc, data[(int)RightMiddle] * 90, 0, 3.138f, 2.105f, false);
		fingerpos[(int)RightIndex] = calcFinger(sc, data[(int)RightIndex] * 90, 2.174f, 2.754f, 1.693f, false);
		fingerpos[(int)RightThumb] = calcFinger(sc, data[(int)RightThumb] * 90, 5.546f, -3.95f, 2.303f, true);
		//Left
		mat = glm::translate(mat, glm::vec3(-1, 0.5, -1));
		mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(-1, 0, 0));
		mat = glm::scale(mat, glm::vec3(-0.009, 0.009, 0.009));
		fingerpos[(int)LeftLittle] = calcFinger(mat, data[(int)LeftLittle] * 90, -3.579f, 1.412f, 1.438f, false);
		fingerpos[(int)LeftRing] = calcFinger(mat, data[(int)LeftRing] * 90, -1.858f, 2.292f, 1.9f, false);
		fingerpos[(int)LeftMiddle] = calcFinger(mat, data[(int)LeftMiddle] * 90, 0, 3.138f, 2.105f, false);
		fingerpos[(int)LeftIndex] = calcFinger(mat, data[(int)LeftIndex] * 90, 2.174f, 2.754f, 1.693f, false);
		fingerpos[(int)LeftThumb] = calcFinger(mat, data[(int)LeftThumb] * 90, 5.546f, -3.95f, 2.303f, true);
	}

	glm::mat4 GloveDeviceDriver::calcFinger(const glm::mat4 &mat, float rot, float T1x, float T1y, float T2y, bool thumb)
	{
		glm::mat4 ret = mat;
		if (!thumb){
			ret = glm::rotate(ret, glm::radians(rot), glm::vec3(-1, 0, 0));
			ret = glm::translate(ret, glm::vec3(T1x, T1y, 0));
			ret = glm::rotate(ret, glm::radians(rot), glm::vec3(-1, 0, 0));
			ret = glm::translate(ret, glm::vec3(0, T2y, 0));
			ret = glm::rotate(ret, glm::radians(rot), glm::vec3(-1, 0, 0));
			ret = glm::translate(ret, glm::vec3(0, 1, 0));
		}
		else
		{
			ret = glm::translate(ret, glm::vec3(T1x*0.47f, T1y, 0));
			ret = glm::rotate(ret, glm::radians(rot), glm::vec3(0, 1, 0));
			ret = glm::translate(ret, glm::vec3(T1x*0.21f, T2y*0.35f, 0));
			ret = glm::rotate(ret, glm::radians(rot), glm::vec3(0, 1, 0));
			ret = glm::translate(ret, glm::vec3(T1x*0.32f, T2y*0.65f, 0));
			ret = glm::rotate(ret, glm::radians(rot), glm::vec3(0, 1, 0));
			ret = glm::translate(ret, glm::vec3(0, 1, 0));
		}
		return (ret);
	}

	GloveDeviceDriver::GloveDeviceDriverAdaptor::GloveDeviceDriverAdaptor(GloveDeviceDriver* driver, GloveFinger finger)
	{
		this->driver = driver;
		this->finger = finger;
	}

	glm::mat4 GloveDeviceDriver::GloveDeviceDriverAdaptor::getData()
	{
		return driver->fingerpos[(int)finger];
	}
}