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
	template<>
	int ClusterData<std::string>::getEstimatedSize()
	{
		return 4 + data->size();
	}
	
}