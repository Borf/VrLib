#include "Camera.h"

#include <VrLib/gl/FBO.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			Camera::Camera()
			{
				target = new vrlib::gl::FBO(2048, 2048);
			}

			void Camera::render()
			{

			}
		}
	}
}