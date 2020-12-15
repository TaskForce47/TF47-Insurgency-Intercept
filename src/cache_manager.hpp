#pragma once

#include "intercept.hpp"
#include "sector.hpp"
#define FMT_HEADER_ONLY
#include "../fmt/include/fmt/format.h"

namespace mission
{

	typedef struct cache
	{
		vector3 cache_pos;
		bool state;
		std::vector<vector3> hint_markers;
		object cache_object;
		intercept::client::EHIdentifierHandle event_handler;
	}cache;
	
	class cache_manager
	{
		
	public:
		std::vector<cache*> caches;
		
		cache_manager();
		~cache_manager();

		void generate_caches(std::vector<sector*> sectors);
		void handle_cache_destroyed(object unit, object source, float damage, object investigator);
		void spawn_cache(vector3 pos);
		void create_hint();
		bool is_completed();
		void load();
		void save();
	};
}
