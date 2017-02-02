#include <VrLib/Viewport.h>
#include <VrLib/Log.h>
#include <VrLib/Device.h>
#include <VrLib/Kernel.h>
#include <VrLib/Viewports/ProjectionViewport.h>
#include <VrLib/Viewports/SimulatorViewport.h>
#include <VrLib/Viewports/RiftViewport.h>
#include <VrLib/Viewports/OpenVRViewport.h>
#include <VrLib/json.hpp>


namespace vrlib
{
	Viewport::Viewport(User* user)
	{
		this->user = user;
	}


	Viewport::~Viewport()
	{
	}

	void Viewport::setScreenPosition(float x, float y)
	{
		position = glm::vec2(x, y);
	}

	void Viewport::setScreenSize(float width, float height)
	{
		size = glm::vec2(width, height);
	}



	void Viewport::resetOpenGL()
	{
		glBindVertexArray(0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisable(GL_CULL_FACE);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glUseProgram(0);
		glEnable(GL_DEPTH_TEST);
		glLoadIdentity();
	}

	Viewport* Viewport::createViewport(Kernel* kernel, json viewportConfig, json otherConfigs)
	{
		Viewport* viewport = NULL;
		if (viewportConfig["type"] == "viewport")
		{
			glm::vec3 tl(viewportConfig["topleft"][0u], viewportConfig["topleft"][1u], viewportConfig["topleft"][2u]);
			glm::vec3 tr(viewportConfig["topright"][0u], viewportConfig["topright"][1u], viewportConfig["topright"][2u]);
			glm::vec3 bl(viewportConfig["bottomleft"][0u], viewportConfig["bottomleft"][1u], viewportConfig["bottomleft"][2u]);
			glm::vec3 br(viewportConfig["bottomright"][0u], viewportConfig["bottomright"][1u], viewportConfig["bottomright"][2u]);

			int eye = 0;
			if (viewportConfig["eye"] == "left")
				eye = 1;
			if (viewportConfig["eye"] == "right")
				eye = 2;

			ProjectionViewport* port = new ProjectionViewport(kernel->getUser(viewportConfig["user"]), eye, tl, tr, bl, br);
			viewport = port;
		}
		else if (viewportConfig["type"] == "simulator")
		{
			PositionalDevice* simCamera = new PositionalDevice();
			simCamera->init(viewportConfig["camera"]);
			SimulatorViewport* port = new SimulatorViewport(kernel->getUser(viewportConfig["user"]), simCamera);
			port->setWindowSize(kernel->windowWidth, kernel->windowHeight);
			for (size_t ii = 0; ii < otherConfigs.size(); ii++)
			{
				for (size_t iii = 0; iii < otherConfigs[ii]["viewports"].size(); iii++)
				{
					json viewportConfig = otherConfigs[ii]["viewports"][iii];
					if (viewportConfig["type"] == "viewport")
					{
						glm::vec3 tl(otherConfigs[ii]["viewports"][iii]["topleft"][0u], otherConfigs[ii]["viewports"][iii]["topleft"][1u], otherConfigs[ii]["viewports"][iii]["topleft"][2u]);
						glm::vec3 tr(otherConfigs[ii]["viewports"][iii]["topright"][0u], otherConfigs[ii]["viewports"][iii]["topright"][1u], otherConfigs[ii]["viewports"][iii]["topright"][2u]);
						glm::vec3 bl(otherConfigs[ii]["viewports"][iii]["bottomleft"][0u], otherConfigs[ii]["viewports"][iii]["bottomleft"][1u], otherConfigs[ii]["viewports"][iii]["bottomleft"][2u]);
						glm::vec3 br(otherConfigs[ii]["viewports"][iii]["bottomright"][0u], otherConfigs[ii]["viewports"][iii]["bottomright"][1u], otherConfigs[ii]["viewports"][iii]["bottomright"][2u]);
						port->addViewPort(tl, tr, bl, br);
					}
				}
			}
			viewport = port;
		}
		else if (viewportConfig["type"] == "riftviewport")
		{
			RiftViewport* port = new RiftViewport(kernel->getUser(viewportConfig["user"]), kernel->oculusDriver, kernel);
			viewport = port;
		}
		else if (viewportConfig["type"] == "openvr")
		{
			OpenVRViewport* port = new OpenVRViewport(kernel->getUser(viewportConfig["user"]), kernel->openvrDriver, kernel);
			viewport = port;
		}
		else
			logger << "Unknown viewport type: " << viewportConfig["type"] << Log::newline;
		if (viewport)
		{
			viewport->setScreenPosition(viewportConfig["x"], viewportConfig["y"]);
			viewport->setScreenSize(viewportConfig["width"], viewportConfig["height"]);
		}

		return viewport;
	}
}