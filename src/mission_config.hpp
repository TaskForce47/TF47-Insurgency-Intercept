#pragma once

#include "intercept.hpp"

namespace mission
{
	class mission_config
	{
	private:
		inline static bool instance_flag_ = false;
		static mission_config* _instance;
		mission_config();

	public:
		static mission_config* get();

		~mission_config();
		int scan_loop_interval{};
		int cache_count{};
		int sector_size = 100;
		int sector_building_threshold = 2;
		int max_hints = 5;
		int max_caches = 10;
		std::string cache_classname = "";
		game_data_side player_side;
		game_data_side enemy_side;
	};
}
