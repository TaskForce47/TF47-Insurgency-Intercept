#include "mission_config.hpp"

using namespace mission;

mission_config* mission_config::_instance;

mission_config::mission_config() = default;

mission_config::~mission_config()
{
	instance_flag_ = false;
}

mission::mission_config* mission_config::get()
{
	if (!instance_flag_)
	{
		_instance = new mission_config();
		instance_flag_ = true;
		return _instance;
	}
	return _instance;
}

