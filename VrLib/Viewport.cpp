#include <VrLib/Viewport.h>
#include <VrLib/Log.h>
#include <VrLib/Device.h>
#include <VrLib/Kernel.h>
#include <VrLib/Viewports/ProjectionViewport.h>
#include <VrLib/Viewports/SimulatorViewport.h>
#include <VrLib/Viewports/RiftViewport.h>
#include <VrLib/Viewports/OpenVRViewport.h>
#include <VrLib/json.h>


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

	Viewport* Viewport::createViewport(Kernel* kernel, json::Value viewportConfig, json::Value otherConfigs)
	{
		Viewport* viewport = NULL;
		if (viewportConfig["type"].asString() == "viewport")
		{
			glm::vec3 tl(viewportConfig["topleft"][0u].asFloat(), viewportConfig["topleft"][1u].asFloat(), viewportConfig["topleft"][2u].asFloat());
			glm::vec3 tr(viewportConfig["topright"][0u].asFloat(), viewportConfig["topright"][1u].asFloat(), viewportConfig["topright"][2u].asFloat());
			glm::vec3 bl(viewportConfig["bottomleft"][0u].asFloat(), viewportConfig["bottomleft"][1u].asFloat(), viewportConfig["bottomleft"][2u].asFloat());
			glm::vec3 br(viewportConfig["bottomright"][0u].asFloat(), viewportConfig["bottomright"][1u].asFloat(), viewportConfig["bottomright"][2u].asFloat());

			int eye = 0;
			if (viewportConfig["eye"].asString() == "left")
				eye = 1;
			if (viewportConfig["eye"].asString() == "right")
				eye = 2;

			ProjectionViewport* port = new ProjectionViewport(kernel->getUser(viewportConfig["user"].asString()), eye, tl, tr, bl, br);
			viewport = port;
		}
		else if (viewportConfig["type"].asString() == "simulator")
		{
			PositionalDevice* simCamera = new PositionalDevice();
			simCamera->init(viewportConfig["camera"].asString());
			SimulatorViewport* port = new SimulatorViewport(kernel->getUser(viewportConfig["user"].asString()), simCamera);
			port->setWindowSize(kernel->windowWidth, kernel->windowHeight);
			for (size_t ii = 0; ii < otherConfigs.size(); ii++)
			{
				for (size_t iii = 0; iii < otherConfigs[ii]["viewports"].size(); iii++)
				{
					json::Value viewportConfig = otherConfigs[ii]["viewports"][iii];
					if (viewportConfig["type"].asString() == "viewport")
					{
						glm::vec3 tl(otherConfigs[ii]["viewports"][iii]["topleft"][0u].asFloat(), otherConfigs[ii]["viewports"][iii]["topleft"][1u].asFloat(), otherConfigs[ii]["viewports"][iii]["topleft"][2u].asFloat());
						glm::vec3 tr(otherConfigs[ii]["viewports"][iii]["topright"][0u].asFloat(), otherConfigs[ii]["viewports"][iii]["topright"][1u].asFloat(), otherConfigs[ii]["viewports"][iii]["topright"][2u].asFloat());
						glm::vec3 bl(otherConfigs[ii]["viewports"][iii]["bottomleft"][0u].asFloat(), otherConfigs[ii]["viewports"][iii]["bottomleft"][1u].asFloat(), otherConfigs[ii]["viewports"][iii]["bottomleft"][2u].asFloat());
						glm::vec3 br(otherConfigs[ii]["viewports"][iii]["bottomright"][0u].asFloat(), otherConfigs[ii]["viewports"][iii]["bottomright"][1u].asFloat(), otherConfigs[ii]["viewports"][iii]["bottomright"][2u].asFloat());
						port->addViewPort(tl, tr, bl, br);
					}
				}
			}
			viewport = port;
		}
		else if (viewportConfig["type"].asString() == "riftviewport")
		{
			RiftViewport* port = new RiftViewport(kernel->getUser(viewportConfig["user"].asString()), kernel->oculusDriver, kernel);
			viewport = port;
		}
		else if (viewportConfig["type"].asString() == "openvr")
		{
			OpenVRViewport* port = new OpenVRViewport(kernel->getUser(viewportConfig["user"].asString()), kernel->openvrDriver, kernel);
			viewport = port;
		}
		else
			logger << "Unknown viewport type: " << viewportConfig["type"].asString() << Log::newline;
		if (viewport)
		{
			viewport->setScreenPosition(viewportConfig["x"].asFloat(), viewportConfig["y"].asFloat());
			viewport->setScreenSize(viewportConfig["width"].asFloat(), viewportConfig["height"].asFloat());
		}

		return viewport;
	}
}