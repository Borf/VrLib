#include "ClusterData.h"


#include <VrLib/ClusterManager.h>
#include <VrLib/Kernel.h>

namespace vrlib
{
	void ClusterDataBase::init()
	{
		Kernel::getInstance()->registerClusterData(this);
	}

	bool ClusterDataBase::isLocal()
	{
		return Kernel::getInstance()->isMaster();
	}
}